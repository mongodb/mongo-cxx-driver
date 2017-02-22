// Copyright 2017 MongoDB Inc.
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

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/json.hpp>

#include <bsoncxx/config/private/prelude.hh>

// In order to have catch pretty-print documents used in REQUIRE macros, we need to overload
// Catch::toString for document::view. Due to how overload resolution works, this needs to be done
// before the inclusion of catch.hpp. To that end, this file should be included in place of
// catch.hpp.
namespace Catch {
inline std::string toString(bsoncxx::document::view document) {
    return bsoncxx::to_json(document);
}
}  // namespace Catch

#include <bsoncxx/config/private/postlude.hh>

#include "catch.hpp"
