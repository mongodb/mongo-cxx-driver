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

#include "mongoc.h"

#include "bson/document.hpp"

#include "stdx/optional.hpp"

namespace bson {
namespace libbson {

class scoped_bson_t {
   public:
    scoped_bson_t(const mongo::driver::optional<bson::document::view>& doc);
    scoped_bson_t(const bson::document::view& doc);
    scoped_bson_t();

    scoped_bson_t(const scoped_bson_t& rhs) = delete;
    scoped_bson_t& operator=(const scoped_bson_t& rhs) = delete;
    scoped_bson_t(scoped_bson_t&& rhs) = delete;
    scoped_bson_t& operator=(scoped_bson_t&& rhs) = delete;

    void init_from_static(const mongo::driver::optional<bson::document::view>& doc);
    void init_from_static(const bson::document::view& doc);
    void init();
    void flag_init();

    ~scoped_bson_t();

    bson_t* bson();

    bson::document::view view();
    bson::document::value steal();

   private:
    bson_t _bson;
    bool _is_initialized;
};

}  // namespace libbson
}  // namespace bson
