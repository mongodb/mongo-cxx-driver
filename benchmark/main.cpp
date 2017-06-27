// Copyright 2017 MongoDB Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <iostream>

#include "benchmark_runner.hpp"

using namespace benchmark;

int main() {
    benchmark_runner runner;
    runner.run_microbenches();

    std::cout << "BSONBench: " << runner.calculate_bson_bench_score() << " MB/s" << std::endl;
    std::cout << "SingleBench: " << runner.calculate_single_bench_score() << " MB/s" << std::endl;
    std::cout << "MultiBench: " << runner.calculate_multi_bench_score() << "MB/s" << std::endl;
    std::cout << "ParallelBench: " << runner.calculate_parallel_bench_score() << "MB/s"
              << std::endl;
    std::cout << "ReadBench: " << runner.calculate_read_bench_score() << "MB/s" << std::endl;
    std::cout << "WriteBench: " << runner.calculate_write_bench_score() << "MB/s" << std::endl;
    std::cout << "DriverBench: " << runner.calculate_driver_bench_score() << "MB/s" << std::endl;
}
