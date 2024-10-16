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
#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <queue>
#include <random>
#include <thread>
#include <vector>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <examples/macros.hh>

#if !defined(_MSC_VER)

#include <unistd.h>

#include <sys/wait.h>

#endif  // !defined(_MSC_VER)

namespace {

class runner_type {
   public:
    using fn_type = void (*)();

    struct component {
        fn_type fn;
        const char* name;

        component(fn_type f, const char* n) : fn(f), name(n) {}
    };

   private:
    std::vector<component> components;

    std::vector<component> components_with_instance;

    std::vector<component> components_for_single;
    std::vector<component> components_for_replica;
    std::vector<component> components_for_sharded;

    std::vector<component> forking_components;

    std::minstd_rand::result_type seed = 0u;
    std::minstd_rand gen;
    unsigned int jobs = 0;
    bool use_fork = true;

    static void run_with_jobs(const std::vector<component>& components, unsigned int jobs) {
        if (jobs == 1) {
            for (const auto& component : components) {
                component.fn();
            }
        } else {
            std::queue<std::thread> threads;

            // Rudimentary job scheduler.
            for (const auto& component : components) {
                while (threads.size() >= jobs) {
                    threads.front().join();
                    threads.pop();
                }

                threads.emplace(component.fn);
            }

            while (!threads.empty()) {
                threads.front().join();
                threads.pop();
            }
        }
    }

    void run_components() {
        run_with_jobs(components, jobs);
    }

    enum struct action {
        succeed,
        fail,
        return_from_main,
    };

    action run_forking_components() {
        if (use_fork) {
#if !defined(_MSC_VER)
            // Forking with threads is difficult and the number of components that require forking
            // are few in number. Run forking components sequentially.
            for (const auto& component : forking_components) {
                const auto& fn = component.fn;
                const auto& name = component.name;

                const pid_t pid = ::fork();

                // Child: do nothing more than call the registered function.
                if (pid == 0) {
                    fn();
                    return action::return_from_main;  // Return from `main()`.
                }

                // Parent: wait for child and handle returned status values.
                else {
                    int status;

                    const int ret = ::waitpid(pid, &status, 0);

                    // For non-zero exit codes, permit continuation for example coverage.
                    if (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS) {
                        std::cout << __func__ << ": failed: " << name
                                  << " exited with a non-zero exit code: " << WEXITSTATUS(status)
                                  << std::endl;

                        return action::fail;
                    }

                    // For unexpected signals, stop immediately.
                    else if (WIFSIGNALED(status)) {
                        const int signal = WTERMSIG(status);
                        const char* const sigstr = ::strsignal(signal);

                        std::cout << __func__ << ": failed: " << name
                                  << " was killed by signal: " << signal << " ("
                                  << (sigstr ? sigstr : "") << ")" << std::endl;

                        std::exit(EXIT_FAILURE);
                    }

                    // We don't expect any other failure condition.
                    else {
                        assert(ret != -1);
                    }
                }
            }

            return action::succeed;
#endif  // !defined(_MSC_VER)
        }

        std::cout << "Skipping API examples that require forked processes" << std::endl;
        return action::succeed;
    }

    void run_components_with_instance() {
        mongocxx::instance instance;

        run_with_jobs(components_with_instance, jobs);

        try {
            mongocxx::client client{mongocxx::uri{"mongodb://localhost:27017/"}};

            const auto reply = client["admin"].run_command(bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("isMaster", 1)));

            if (reply["msg"]) {
                std::cout << "Running API examples against a live sharded server" << std::endl;
                run_with_jobs(components_for_sharded, jobs);
                run_with_jobs(components_for_replica, jobs);
                run_with_jobs(components_for_single, jobs);
            } else if (reply["setName"]) {
                std::cout << "Running API examples against a live replica server" << std::endl;
                run_with_jobs(components_for_replica, jobs);
                run_with_jobs(components_for_single, jobs);
            } else {
                std::cout << "Running API examples against a live single server" << std::endl;
                run_with_jobs(components_for_single, jobs);
            }
        } catch (const mongocxx::exception& ex) {
            std::cout << "Skipping API examples that require a live server: " << ex.what()
                      << std::endl;
        }
    }

   public:
    void add_component(fn_type fn, const char* name) {
        components.emplace_back(fn, name);
    }

    void add_component_with_instance(fn_type fn, const char* name) {
        components_with_instance.emplace_back(fn, name);
    }

    void add_component_for_single(fn_type fn, const char* name) {
        components_for_single.emplace_back(fn, name);
    }

    void add_component_for_replica(fn_type fn, const char* name) {
        components_for_replica.emplace_back(fn, name);
    }

    void add_component_for_sharded(fn_type fn, const char* name) {
        components_for_sharded.emplace_back(fn, name);
    }

    void add_forking_component(fn_type fn, const char* name) {
        forking_components.emplace_back(fn, name);
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

    void set_use_fork(bool use_fork) {
        this->use_fork = use_fork;
    }

    int run() {
        assert(jobs > 0u);

        std::cout << "seed: " << seed << std::endl;

        gen.seed(seed);

        // Prevent ordering dependencies across examples.
        std::shuffle(components.begin(), components.end(), gen);
        std::shuffle(forking_components.begin(), forking_components.end(), gen);

        run_components();

        switch (run_forking_components()) {
            case action::succeed:
                break;  // Continue example coverage.
            case action::fail:
                return EXIT_FAILURE;  // A component failed.
            case action::return_from_main:
                return EXIT_SUCCESS;  // Return directly from forked processes.
        }

        run_components_with_instance();

        return EXIT_SUCCESS;
    }
};

runner_type runner;

}  // namespace

void runner_register_component(void (*fn)(), const char* name) {
    runner.add_component(fn, name);
}

void runner_register_component_with_instance(void (*fn)(), const char* name) {
    runner.add_component_with_instance(fn, name);
}

void runner_register_component_for_single(void (*fn)(), const char* name) {
    runner.add_component_for_single(fn, name);
}

void runner_register_component_for_replica(void (*fn)(), const char* name) {
    runner.add_component_for_replica(fn, name);
}

void runner_register_component_for_sharded(void (*fn)(), const char* name) {
    runner.add_component_for_sharded(fn, name);
}

void runner_register_forking_component(void (*fn)(), const char* name) {
    runner.add_forking_component(fn, name);
}

int EXAMPLES_CDECL main(int argc, char** argv) {
    bool set_seed = false;
    bool set_jobs = false;
    bool set_use_fork = false;

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
                std::cerr << "invalid seed string: " << seed_str << std::endl;
                return 1;
            }

            runner.set_seed(seed);
            set_seed = true;
        }

        // Allow setting job count (e.g. set to 1 for debugging).
        if (strcmp(argv[i], "--jobs") == 0) {
            if (i + 1 >= argc) {
                std::cerr << "missing argument to --jobs" << std::endl;
                return 1;
            }

            char* const jobs_str = argv[++i];  // Next argument.
            char* end = nullptr;

            const auto jobs = std::strtoul(jobs_str, &end, 10);

            if (static_cast<std::size_t>(end - jobs_str) != std::strlen(jobs_str)) {
                std::cerr << "invalid jobs string: " << jobs_str << std::endl;
                return 1;
            }

            if (jobs >= UINT_MAX) {
                std::cerr << "invalid jobs string (too large): " << jobs_str << std::endl;
            }

            runner.set_jobs(static_cast<unsigned int>(jobs));
            set_jobs = true;
        }

        // Allow disabling use of fork (e.g. disable for debugging).
        if (strcmp(argv[i], "--use-fork") == 0) {
            if (i + 1 >= argc) {
                std::cerr << "missing argument to --use-fork" << std::endl;
                return 1;
            }

            char* const use_fork_str = argv[++i];  // Next argument.
            char* end = nullptr;

            const auto flag = std::strtoul(use_fork_str, &end, 10);

            if (static_cast<std::size_t>(end - use_fork_str) != std::strlen(use_fork_str)) {
                std::cerr << "invalid argument: " << use_fork_str << std::endl;
                return 1;
            }

            runner.set_use_fork(flag == 0 ? false : true);
            set_use_fork = true;
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

    // Default: use fork when available.
    if (!set_use_fork) {
        runner.set_use_fork(true);
    }

    return runner.run();  // Return directly from forked processes.
}
