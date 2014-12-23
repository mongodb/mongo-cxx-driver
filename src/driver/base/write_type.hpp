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

namespace mongo {
namespace driver {

enum class write_type {

    k_insert_one,
    k_delete_one,
    k_delete_many,
    k_update_one,
    k_update_many,
    k_replace_one,
    k_uninitialized,

}; // enum class write_type

}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
