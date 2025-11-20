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

#include <mongocxx/v1/hint.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <string>

namespace mongocxx {
namespace v1 {

class hint::internal {
   public:
    static bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& doc(hint& self);
    static bsoncxx::v1::stdx::optional<std::string>& str(hint& self);
};

} // namespace v1
} // namespace mongocxx
