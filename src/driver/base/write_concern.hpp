// Copyright 2014 MongoDB Inc.
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

#include "driver/config/prelude.hpp"

#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <memory>

#include "stdx/optional.hpp"

namespace mongo {
namespace driver {

class client;
class collection;
class database;
class bulk_write;

class LIBMONGOCXX_EXPORT write_concern {

   // TODO: this interface still needs work
   public:
    write_concern();

    write_concern(const write_concern&);
    write_concern& operator=(const write_concern&);

    write_concern(write_concern&& other) noexcept;
    write_concern& operator=(write_concern&& rhs) noexcept;

    ~write_concern();

    void fsync(bool fsync);
    void journal(bool journal);
    void nodes(std::int32_t confirm_from);
    void majority(std::chrono::milliseconds timeout);
    void tag(const std::string& tag);
    void timeout(std::chrono::milliseconds timeout);

    bool fsync() const;
    bool journal() const;
    std::int32_t nodes() const;
    std::string tag() const;
    bool majority() const;
    std::chrono::milliseconds timeout() const;

   private:
    friend client;
    friend collection;
    friend database;
    friend bulk_write;

    class impl;

    write_concern(std::unique_ptr<impl>&& implementation);

    std::unique_ptr<impl> _impl;

}; // class write_concern

}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
