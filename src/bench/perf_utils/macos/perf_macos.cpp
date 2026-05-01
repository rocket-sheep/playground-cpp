// ----------------------------------------------------------------------
// NOTE: Freely adapted from <https://gist.github.com/ibireme/173517c208c7dc333ba962c1f0d67d12>
// ----------------------------------------------------------------------

#if __APPLE__

#include "perf_utils/macos/perf_macos.hpp"

#include <array>
#include <iostream>

// NOTE: Header exposes functions as static function pointers, but not all of
// them are used => Disabling warning
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#include "perf_utils/macos/kperf.h"
#pragma clang diagnostic pop

namespace perf_utils {

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

/// All information required to process a given performance counter
struct EventSpec
{
    /// Human-friendly name for the counter
    const char* display_name;

    /// Associated field in the `PerfCounters` class
    uint64_t PerfCounters::* target;

    /// Possible names of the even in the MPC DB.
    /// Maps this abstract performance counter to concrete event names.
    /// NOTE: They are tried sequentially until one succeeds
    static constexpr size_t MAX_EVENT_NAMES = 4;
    std::array<const char*, MAX_EVENT_NAMES> event_names;
};

/// All supported counters
/// WARNING: Indices *must* correspond to `PerfCounter` enum values!
static const EventSpec _all_event_specs[] = {
    {
        "cycles", &PerfCounters::cycles, {
            "FIXED_CYCLES",                 // Apple A7-A15
            "CPU_CLK_UNHALTED.THREAD",      // Intel Core 1th-10th
            "CPU_CLK_UNHALTED.CORE",        // Intel Yonah, Merom
        },
    },
    {
        "instructions", &PerfCounters::instructions, {
            "FIXED_INSTRUCTIONS",           // Apple A7-A15
            "INST_RETIRED.ANY"              // Intel Yonah, Merom, Core 1th-10th
        },
    },
    {
        "branches", &PerfCounters::branches, {
            "INST_BRANCH",                  // Apple A7-A15
            "BR_INST_RETIRED.ALL_BRANCHES", // Intel Core 1th-10th
            "BR_INST_RETIRED.ANY",          // Intel Yonah, Merom
        },
    },
    {
        "branch_misses", &PerfCounters::branch_misses, {
            "BRANCH_MISPRED_NONSPEC",       // Apple A7-A15, since iOS 15, macOS 12
            "BRANCH_MISPREDICT",            // Apple A7-A14
            "BR_MISP_RETIRED.ALL_BRANCHES", // Intel Core 2th-10th
            "BR_INST_RETIRED.MISPRED",      // Intel Yonah, Merom
        },
    },
    {
        "l1_misses", &PerfCounters::l1_misses, {
            "MEM_LOAD_RETIRED.L1_MISS",
            "MEM_LOAD_UOPS_RETIRED.L1_MISS",
        },
    },
    {
        "l2_misses", &PerfCounters::l2_misses, {
            "MEM_LOAD_RETIRED.L2_MISS",
            "MEM_LOAD_UOPS_RETIRED.L2_MISS",
        },
    },
    {
        "l3_misses", &PerfCounters::l3_misses, {
            "MEM_LOAD_RETIRED.L3_MISS",
            "MEM_LOAD_UOPS_RETIRED.L3_MISS",
        },
    },
};

/// Retrieve an event from the DB, trying all its possible names in turn
static kpep_event* _get_event(kpep_db* db, const EventSpec& event_spec)
{
    for (const auto& event_name : event_spec.event_names) {
        if (event_name == nullptr)
            break;
        kpep_event* ev = nullptr;
        if (kpep_db_event(db, event_name, &ev) == 0) {
            return ev;
        }
    }
    return nullptr;
}

/// Report a configuration error the standard error stream
static void _report_config_error(int rc, const char* message)
{
    std::cerr << message << ": " << kpep_config_error_desc(rc) << " (" << rc << ")" << std::endl;
}

// -----------------------------------------------------------------------------
// PerfCounterProfiler
// -----------------------------------------------------------------------------

PerfCounterProfiler PerfCounterProfiler::_instance;

// Perf counters enabled by default
std::vector<PerfCounter> PerfCounterProfiler::_enabled_counters = {
    PerfCounter::CYCLES, PerfCounter::INSTRUCTIONS,
    PerfCounter::BRANCHES, PerfCounter::BRANCH_MISSES,
};

PerfCounterProfiler::Status PerfCounterProfiler::_init()
{
    for (auto& perf_counter : _enabled_counters) {
        _enabled_event_specs.push_back(&_all_event_specs[static_cast<size_t>(perf_counter)]);
    }

    // Load dynamic library
    if (!lib_init()) {
        std::cerr << "Error loading: " << lib_err_msg << std::endl;
        return Status::LIB_INIT_FAILED;
    }

    // Check permissions
    int force_ctrs = 0;
    if (kpc_force_all_ctrs_get(&force_ctrs) != 0) {
        std::cerr << "Permission denied, xnu/kpc requires root privileges" << std::endl;
        return Status::PERMISSION_DENIED;
    }
    
    // Load PMC DB
    kpep_db* db = nullptr;
    int rc;
    if ((rc = kpep_db_create(nullptr, &db)) != 0) {
        std::cerr << "Error: cannot load pmc database: " << rc << std::endl;
        return Status::DB_LOAD_FAILED;
    }

    // Create a configuration
    kpep_config* config = nullptr;
    if ((rc = kpep_config_create(db, &config)) != 0) {
        _report_config_error(rc, "Failed to create kpep config");
        return Status::CONFIGURATION_FAILED;
    }
    if ((rc = kpep_config_force_counters(config)) != 0) {
        _report_config_error(rc, "Failed to force counters");
        return Status::CONFIGURATION_FAILED;
    }
    
    // Get events for enabled counters
    std::vector<kpep_event*> events;
    events.reserve(_enabled_event_specs.size());
    for (const EventSpec* event_spec : _enabled_event_specs) {
        auto event = _get_event(db, *event_spec);
        if (event == nullptr) {
            std::cerr << "Cannot find event '" << event_spec->display_name << "'" << std::endl;
            return Status::CONFIGURATION_FAILED;
        }
        events.push_back(event);
    }
    
    // Add events to configuration
    for (auto event : events) {
        if ((rc = kpep_config_add_event(config, &event, 0, nullptr))) {
            _report_config_error(rc, "Failed to add event");
            return Status::CONFIGURATION_FAILED;
        }
    }
    
    // Prepare buffer and config
    if ((rc = kpep_config_kpc_classes(config, &classes))) {
        _report_config_error(rc, "Failed get kpc classes");
        return Status::CONFIGURATION_FAILED;
    }
    size_t reg_count = 0;
    if ((rc = kpep_config_kpc_count(config, &reg_count))) {
        _report_config_error(rc, "Failed get kpc count");
        return Status::CONFIGURATION_FAILED;
    }
    if ((rc = kpep_config_kpc_map(config, _counter_map, sizeof(_counter_map)))) {
        _report_config_error(rc, "Failed get kpc map");
        return Status::CONFIGURATION_FAILED;
    }
    uint64_t regs[KPC_MAX_COUNTERS] = { 0 };
    if ((rc = kpep_config_kpc(config, regs, sizeof(regs)))) {
        _report_config_error(rc, "Failed get kpc registers");
        return Status::CONFIGURATION_FAILED;
    }
    
    // Set config to kernel
    if ((rc = kpc_force_all_ctrs_set(1))) {
        std::cerr << "Failed force all ctrs: " << rc << std::endl;
        return Status::CONFIGURATION_FAILED;
    }
    if ((classes & KPC_CLASS_CONFIGURABLE_MASK) && reg_count) {
        if ((rc = kpc_set_config(classes, regs))) {
            std::cerr << "Failed set kpc config: " << rc << std::endl;
            return Status::CONFIGURATION_FAILED;
        }
    }

    return Status::OK;
}

PerfCounterProfiler::Status PerfCounterProfiler::start()
{
    int rc;

    // Start counting
    if ((rc = kpc_set_counting(classes))) {
        std::cerr << "Failed to set counting (" << rc << ")" << std::endl;
        return Status::COUNTING_FAILED;
    }
    if ((rc = kpc_set_thread_counting(classes))) {
        std::cerr << "Failed to set thread counting (" << rc << ")" << std::endl;
        return Status::COUNTING_FAILED;
    }

    // Get counters
    if ((rc = kpc_get_thread_counters(0, KPC_MAX_COUNTERS, _counters_before))) {
        std::cerr << "Failed to get thread counters (" << rc << ")" << std::endl;
        return Status::COUNTING_FAILED;
    }

    return Status::OK;
}

PerfCounterProfiler::Status PerfCounterProfiler::stop()
{
    int rc;

    // Get counters
    if ((rc = kpc_get_thread_counters(0, KPC_MAX_COUNTERS, _counters_after))) {
        std::cerr << "Failed to get thread counters (" << rc << ")" << std::endl;
        return Status::COUNTING_FAILED;
    }
    
    // Stop counting
    kpc_set_counting(0);
    kpc_set_thread_counting(0);
    kpc_force_all_ctrs_set(0);

    return Status::OK;
}

PerfCounters PerfCounterProfiler::get_counters() const
{
    PerfCounters perf_counters;
    for (size_t i = 0; i < _enabled_event_specs.size(); ++i) {
        const auto& event_spec = *_enabled_event_specs[i];
        const size_t counter_index = _counter_map[i];
        const uint64_t value = _counters_after[counter_index] - _counters_before[counter_index];
        perf_counters.*event_spec.target = value;
    }
    return perf_counters;
}

} // namespace perf_utils

#endif // __APPLE__
