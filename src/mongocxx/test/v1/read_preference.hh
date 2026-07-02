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

#include <mongocxx/v1/read_preference.hpp> // IWYU pragma: export

//

#include <catch2/catch_tostring.hpp>

CATCH_REGISTER_ENUM(
    mongocxx::v1::read_preference::read_mode,
    mongocxx::v1::read_preference::read_mode::k_primary,
    mongocxx::v1::read_preference::read_mode::k_primary_preferred,
    mongocxx::v1::read_preference::read_mode::k_secondary,
    mongocxx::v1::read_preference::read_mode::k_secondary_preferred,
    mongocxx::v1::read_preference::read_mode::k_nearest)
