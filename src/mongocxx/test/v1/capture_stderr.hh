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

#pragma once

#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <string>

#include <catch2/catch_test_macros.hpp>

#if !defined(_WIN32)
#include <fcntl.h>
#include <unistd.h>
#endif

namespace mongocxx {
namespace test {

#if !defined(_WIN32)

// Redirect stderr to a pipe for the lifetime of this object so that output (e.g. from mongoc's
// default log handler) can be inspected via `read()`. Restores the original stderr on destruction.
class capture_stderr {
   private:
    int _pipes[2];
    int _stderr; // stderr

   public:
    ~capture_stderr() {
        ::dup2(_stderr, STDERR_FILENO); // Restore original stderr.
    }

    capture_stderr(capture_stderr&&) = delete;
    capture_stderr& operator=(capture_stderr&&) = delete;
    capture_stderr(capture_stderr const&) = delete;
    capture_stderr& operator=(capture_stderr const&) = delete;

    capture_stderr()
        : _stderr{::dup(STDERR_FILENO)} // Save original stderr.
    {
        ::fflush(stderr);
        REQUIRE(::pipe(_pipes) == 0);                    // Open redirection pipes.
        REQUIRE(::dup2(_pipes[1], STDERR_FILENO) != -1); // Copy stderr to input pipe.
        REQUIRE(::close(_pipes[1]) != -1);               // Close original stderr.

        REQUIRE(::fcntl(_pipes[0], F_SETFL, ::fcntl(_pipes[0], F_GETFL) | O_NONBLOCK) != -1); // Do not block on read.
    }

    std::string read() {
        std::string res;

        {
            // Capture everything up to this point.
            // Avoid recursive macro expansion of `stderr` due to Catch expression decomposition.
            auto const ret = ::fflush(stderr);
            REQUIRE(ret == 0);
        }

        while (true) {
            char buf[BUFSIZ];
            auto const n = ::read(_pipes[0], buf, std::size_t{BUFSIZ - 1});

            CHECKED_IF(n < 0) {
                REQUIRE(errno == EAGAIN); // No data left in stream.
                break;
            }
            else {
                res.append(buf, static_cast<std::size_t>(n));
            }
        }

        return res;
    }
};

#else

// Windows: stderr redirection is not supported; `read()` yields nothing.
class capture_stderr {
   public:
    std::string read() {
        return {};
    }
};

#endif // !defined(_WIN32)

} // namespace test
} // namespace mongocxx
