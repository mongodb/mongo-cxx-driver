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

int main(int argc, char* argv[]) {
    std::set<benchmark_type> types;

    if (argc > 1) {
        for (int x = 1; x < argc; ++x) {
            std::string type{argv[x]};
            auto it = names_types.find(type);

            if (it != names_types.end()) {
                types.insert(it->second);
            } else {
                std::cerr << "Invalid benchmark: " << type << std::endl;
            }
        }

        if (types.empty()) {
            std::cerr << "No valid benchmarks specified. Exiting." << std::endl;
            return 1;
        }
    }

    benchmark_runner runner{types};
    runner.run_microbenches();
    runner.print_scores();
}
