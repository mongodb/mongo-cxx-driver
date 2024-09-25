// Copyright 2009-present MongoDB, Inc.
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

// Disable macro definition check: this is not a component.
#define EXAMPLES_COMPONENT_NAME

#include <examples/api/runner.hh>

//

#include <algorithm>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <queue>
#include <random>
#include <thread>
#include <vector>

#include <examples/macros.hh>

namespace {

class runner_type {
   public:
    using fn_type = void (*)();

   private:
    std::vector<fn_type> fns;
    std::minstd_rand::result_type seed = 0u;
    std::minstd_rand gen;
    unsigned int jobs = 0;

   public:
    void add(fn_type fn) {
        fns.push_back(fn);
    }

    void set_seed(std::minstd_rand::result_type seed) {
        this->seed = seed;
    }

    void set_jobs(unsigned int jobs) {
        if (jobs == 0u) {
            this->jobs = std::thread::hardware_concurrency();
        } else {
            this->jobs = jobs;
        }
    }

    int run() {
        ASSERT(jobs > 0u);

        std::cout << "seed: " << seed << std::endl;

        gen.seed(seed);

        // Prevent ordering dependencies across examples.
        std::shuffle(fns.begin(), fns.end(), gen);

        std::queue<std::thread> threads;

        // Rudimentary job scheduler.
        for (auto fn : fns) {
            while (threads.size() >= jobs) {
                threads.front().join();
                threads.pop();
            }

            threads.emplace(fn);
        }

        while (!threads.empty()) {
            threads.front().join();
            threads.pop();
        }

        return 0;
    }
};

runner_type runner;

}  // namespace

void runner_register_fn(void (*fn)()) {
    runner.add(fn);
}

int EXAMPLES_CDECL main(int argc, char** argv) {
    bool set_seed = false;
    bool set_jobs = false;

    // Simple command-line argument parser.
    for (int i = 1; i < argc; ++i) {
        // Permit using a custom seed for reproducibility.
        if (strcmp(argv[i], "--seed") == 0) {
            if (i + 1 >= argc) {
                std::cerr << "missing argument to --seed" << std::endl;
                return 1;
            }

            char* const seed_str = argv[++i];  // Next argument.
            char* end = nullptr;

            const auto seed =
                static_cast<std::minstd_rand::result_type>(std::strtoul(seed_str, &end, 10));

            if (static_cast<std::size_t>(end - seed_str) != std::strlen(seed_str)) {
                std::cerr << "invalid seed string" << std::endl;
                return 1;
            }

            runner.set_seed(seed);
            set_seed = true;
        }

        if (strcmp(argv[i], "--jobs") == 0) {
            if (i + 1 >= argc) {
                std::cerr << "missing argument to --jobs" << std::endl;
                return 1;
            }

            char* const jobs_str = argv[++i];  // Next argument.
            char* end = nullptr;

            const auto jobs = std::strtoul(jobs_str, &end, 10);

            if (static_cast<std::size_t>(end - jobs_str) != std::strlen(jobs_str)) {
                std::cerr << "invalid jobs string" << std::endl;
                return 1;
            }

            if (jobs >= UINT_MAX) {
                std::cerr << "invalid jobs string (too large): " << jobs_str << std::endl;
            }

            runner.set_jobs(static_cast<unsigned int>(jobs));
            set_jobs = true;
        }
    }

    // Default: use a random seed.
    if (!set_seed) {
        runner.set_seed(static_cast<std::minstd_rand::result_type>(std::random_device()()));
    }

    // Default: request maximum job count.
    if (!set_jobs) {
        runner.set_jobs(0);
    }

    return runner.run();  // Return directly from forked processes.
}
