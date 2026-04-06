#include <cstdlib>

#include <benchmark/benchmark.h>

using namespace benchmark;

int main(int argc, const char* argv[])
{
    char** const argv_nonconst = const_cast<char**>(argv);
    Initialize(&argc, argv_nonconst);
    if (ReportUnrecognizedArguments(argc, argv_nonconst))
        return 1;
    RunSpecifiedBenchmarks();

    return 0;
}
