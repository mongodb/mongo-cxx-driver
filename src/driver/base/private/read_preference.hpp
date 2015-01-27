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

#include "driver/base/read_preference.hpp"

#include "driver/private/libmongoc.hpp"

namespace mongo {
namespace driver {
namespace base {

class read_preference::impl {

   public:
    impl(mongoc_read_prefs_t* read_pref)
        : read_preference_t(read_pref)
    {}

    ~impl() {
        libmongoc::read_prefs_destroy(read_preference_t);
    }

    mongoc_read_prefs_t* read_preference_t;

}; // class impl

}  // namespace base
}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
