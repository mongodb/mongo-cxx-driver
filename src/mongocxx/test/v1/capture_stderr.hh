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

#include <string>

namespace mongocxx {
namespace test {

// Redirect stderr to a pipe for the lifetime of this object so that output (e.g. from mongoc's
// default log handler) can be inspected via `read()`. Restores the original stderr on destruction.
//
// Member definitions live in capture_stderr.cpp to avoid transitively including system headers in
// test components which include this header.
class capture_stderr {
#if !defined(_WIN32)
   private:
    int _pipes[2];
    int _stderr; // stderr

   public:
    ~capture_stderr();

    capture_stderr(capture_stderr&&) = delete;
    capture_stderr& operator=(capture_stderr&&) = delete;
    capture_stderr(capture_stderr const&) = delete;
    capture_stderr& operator=(capture_stderr const&) = delete;

    capture_stderr();

    std::string read();

#else

   public:
    // Windows: stderr redirection is not supported; `read()` yields nothing.
    std::string read();

#endif // !defined(_WIN32)
};

} // namespace test
} // namespace mongocxx
