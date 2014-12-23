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

#include <cstdint>
#include <string>
#include <memory>

#include "bson/document.hpp"
#include "stdx/optional.hpp"

namespace mongo {
namespace driver {

class client;
class collection;
class database;

// TODO: move to own file?
enum class read_mode {
    k_primary,
    k_secondary,
    k_primary_preferred,
    k_secondary_preferred,
    k_nearest,
};

class LIBMONGOCXX_EXPORT read_preference {

   public:
    explicit read_preference(read_mode rm = read_mode::k_primary);
    read_preference(read_mode, bson::document::view tags);

    read_preference(const read_preference&);
    read_preference& operator=(const read_preference&);

    read_preference(read_preference&& other) noexcept;
    read_preference& operator=(read_preference&& rhs) noexcept;

    ~read_preference();

    void mode(read_mode mode);
    read_mode mode() const;

    void tags(bson::document::view tags);
    optional<bson::document::view> tags() const;

   private:
    friend client;
    friend collection;
    friend database;

    class impl;

    read_preference(std::unique_ptr<impl>&& implementation);

    std::unique_ptr<impl> _impl;

}; // class read_preference

}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
