## Overview

This repository contains C++ code samples from [The Rocket Sheep](https://rocketsheep.org/) blog.

Each article corresponds to a tag in the repository with the same date.

## Build

You will need to pull dependencies using Git submodules. At the moment, this amounts to [Google Test](https://github.com/google/googletest) and [Google Benchmark](https://github.com/google/benchmark).

We use [CMake](https://cmake.org/) as our build system. Please refer to their documentation for usage.

The code has only been tested with [Clang](https://clang.llvm.org/), although it should work without too much trouble on any modern C++ compiler. However, we sometimes use Clang-specific pragmas to forcefully enable or disable optimizations, so that some benchmarks may not yield the same results with another compiler. In other words, Your Mileage May Vary™.
