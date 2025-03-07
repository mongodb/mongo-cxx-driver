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

#include <mongocxx/model/insert_one.hpp>

namespace mongocxx {
namespace v_noabi {
namespace model {

insert_one::insert_one(bsoncxx::v_noabi::document::view_or_value document) : _document(std::move(document)) {}

bsoncxx::v_noabi::document::view_or_value const& insert_one::document() const {
    return _document;
}

} // namespace model
} // namespace v_noabi
} // namespace mongocxx
