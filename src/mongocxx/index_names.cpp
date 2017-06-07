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

#include <mongocxx/index_names.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

index_names::index_names() {}

index_names::iterator index_names::begin() {
    return index_names::iterator(this);
}

index_names::iterator index_names::end() {
    return index_names::iterator(nullptr);
}

const bsoncxx::stdx::string_view& index_names::iterator::operator*() const {
    return bsoncxx::stdx::string_view{"foo"};
}

const bsoncxx::stdx::string_view* index_names::iterator::operator->() const {
    return nullptr;
}

index_names::iterator& index_names::iterator::operator++() {
    return *this;
}

void index_names::iterator::operator++(int) {}

bool MONGOCXX_CALL operator==(const index_names::iterator&, const index_names::iterator&) {
    return false;
}

bool MONGOCXX_CALL operator!=(const index_names::iterator&, const index_names::iterator&) {
    return true;
}

MONGOCXX_PRIVATE bool index_names::iterator::is_exhausted() const {
    return true;
}

index_names::iterator::iterator(index_names* index_names) : _index_names{index_names} {}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx