
# Simple Benchmarking Demo

This repository contains a simple demonstration of performance benchmarking
using [Google Benchmark](https://github.com/google/benchmark) to aid in the
microbenchmarking process. The intent is to show (1) the impact that seemingly
small differences may have on overall performance as well as (2) how data
structure selection can affect performance for simple tasks.

## Dependencies

This project requires:

* Google Benchmark (Build and install in Release mode)

## Building with CMake

1. Clone the repository.

        git clone https://github.com/nsumner/simple-benchmark-demo.git

2. Create a new directory for building.

        mkdir benchbuild

3. Change into the new directory.

        cd benchbuild

4. Run CMake with the path to the source. Make sure to build in release mode.

        cmake ../simple-benchmark-demo/ -DCMAKE_BUILD_TYPE=Release

5. Run make inside the build directory:

        make

This produces benchmarks inside `benchbuild/bin/`. For instance, the benchmark
comparing row and column major traversals of a square matrix can be found in
`benchbuild/bin/matrixTest` after building.

Note, building with a tool like ninja can be done by adding `-G Ninja` to
the cmake invocation and running `ninja` instead of `make`.

## Running Benchmarks

Before running benchmarks, performance altering features of the host computer
should be disabled. For instance, with cpufrequtils installed in Linux,
dynamic frequency scaling can be disabled with

    sudo cpupower frequency-set -g performance

In addition, the benchmarks should start from a quiet state. Ideally, a fresh
boot into a non-graphical mode will provide the least interference (even if
the circumstances are unrealistic in practice).

Standard Google Benchmark invocation and options apply. Due to some static
initialization tricks played in some benchmarks, it may take a minute or two
for the `--help` option to display available options. You can run all tests
for a particular benchmark by just running `benchbuild/bin/<benchmark>`. The
set of standard options is

    [--benchmark_list_tests={true|false}]
    [--benchmark_filter=<regex>]
    [--benchmark_min_time=<min_time>]
    [--benchmark_repetitions=<num_repetitions>]
    [--benchmark_report_aggregates_only={true|false}]
    [--benchmark_display_aggregates_only={true|false}]
    [--benchmark_format=<console|json|csv>]
    [--benchmark_out=<filename>]
    [--benchmark_out_format=<json|console|csv>]
    [--benchmark_color={auto|true|false}]
    [--benchmark_counters_tabular={true|false}]
    [--v=<verbosity>]

Thus, you can list all attempted matrix tests using

    benchbuild/bin/matrixTest --benchmark_list_tests=true

The regex filters can allow you to try specific benchmark configurations and
input sizes, for instance, running the matrix benchmark with only friendly
reads for size 1024 can be done with:

    bin/matrixTest --benchmark_filter='testAccess<Friendly, Read.*>/1024$'

