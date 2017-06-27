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

#include "benchmark_runner.hpp"

#include "bson/bson_encoding.hpp"
#include "multi_doc/bulk_insert.hpp"
#include "multi_doc/find_many.hpp"
#include "multi_doc/gridfs_download.hpp"
#include "multi_doc/gridfs_upload.hpp"
#include "single_doc/find_one_by_id.hpp"
#include "single_doc/insert_one.hpp"
#include "single_doc/run_command.hpp"

namespace benchmark {

// The task sizes and iteration numbers come from the Driver Perfomance Benchmarking Reference Doc.
benchmark_runner::benchmark_runner() {
    using bsoncxx::stdx::make_unique;

    // Bson microbenchmarks
    _microbenches.push_back(make_unique<bson_encoding>(75.31, "extended_bson/flat_bson.json"));
    _microbenches.push_back(make_unique<bson_encoding>(19.64, "extended_bson/deep_bson.json"));
    _microbenches.push_back(make_unique<bson_encoding>(57.34, "extended_bson/full_bson.json"));
    // TODO CXX-1241: Add bson_decoding equivalents.

    // Single doc microbenchmarks
    _microbenches.push_back(make_unique<run_command>());
    _microbenches.push_back(make_unique<find_one_by_id>("single_and_multi_document/tweet.json"));
    _microbenches.push_back(
        make_unique<insert_one>(2.75, 10000, "single_and_multi_document/small_doc.json"));
    _microbenches.push_back(
        make_unique<insert_one>(27.31, 10, "single_and_multi_document/large_doc.json"));

    // Multi doc microbenchmarks
    _microbenches.push_back(make_unique<find_many>("single_and_multi_document/tweet.json"));
    _microbenches.push_back(
        make_unique<bulk_insert>(2.75, 10000, "single_and_multi_document/small_doc.json"));
    _microbenches.push_back(
        make_unique<bulk_insert>(27.31, 10, "single_and_multi_document/large_doc.json"));
    _microbenches.push_back(
        make_unique<gridfs_upload>("single_and_multi_document/gridfs_large.bin"));
    _microbenches.push_back(
        make_unique<gridfs_download>("single_and_multi_document/gridfs_large.bin"));

    // TODO CXX-1378: add parallel microbenchmarks
}

void benchmark_runner::run_microbenches(benchmark_type tag) {
    mongocxx::instance instance{};

    for (std::unique_ptr<microbench>& bench : _microbenches) {
        if (tag == benchmark::benchmark_type::all_benchmarks || bench->has_tag(tag)) {
            bench->run();
        }
    }
}

double benchmark_runner::calculate_average(benchmark_type tag) {
    std::uint32_t count = 0;
    double total = 0.0;
    for (std::unique_ptr<microbench>& bench : _microbenches) {
        if (bench->has_tag(tag)) {
            count++;
            total += bench->get_results().get_score();
        }
    }
    return total / static_cast<double>(count);
}

double benchmark_runner::calculate_bson_bench_score() {
    return calculate_average(benchmark_type::bson_bench);
}

double benchmark_runner::calculate_single_bench_score() {
    return calculate_average(benchmark_type::single_bench);
}

double benchmark_runner::calculate_multi_bench_score() {
    return calculate_average(benchmark_type::multi_bench);
}

double benchmark_runner::calculate_parallel_bench_score() {
    return calculate_average(benchmark_type::parallel_bench);
}

double benchmark_runner::calculate_read_bench_score() {
    return calculate_average(benchmark_type::read_bench);
}

double benchmark_runner::calculate_write_bench_score() {
    return calculate_average(benchmark_type::write_bench);
}

double benchmark_runner::calculate_driver_bench_score() {
    return (calculate_read_bench_score() + calculate_write_bench_score()) / 2.0;
}
}