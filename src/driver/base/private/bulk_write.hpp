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

#include "driver/base/bulk_write.hpp"
#include "driver/private/libmongoc.hpp"

namespace mongo {
namespace driver {
namespace base {

class bulk_write::impl {

   public:
    impl(mongoc_bulk_operation_t* op)
        : operation_t(op)
    {}

    ~impl() {
        libmongoc::bulk_operation_destroy(operation_t);
    }

    mongoc_bulk_operation_t* operation_t;

}; // class impl

}  // namespace base
}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
