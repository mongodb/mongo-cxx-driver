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

#include <mongocxx/options/insert.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

insert& insert::bypass_document_validation(bool bypass_document_validation) {
    _bypass_document_validation = bypass_document_validation;
    return *this;
}

insert& insert::write_concern(mongocxx::v_noabi::write_concern wc) {
    _write_concern = std::move(wc);
    return *this;
}

insert& insert::ordered(bool ordered) {
    _ordered = ordered;
    return *this;
}

insert& insert::comment(bsoncxx::v_noabi::types::bson_value::view_or_value comment) {
    _comment = std::move(comment);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<bool> const& insert::bypass_document_validation() const {
    return _bypass_document_validation;
}

bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& insert::write_concern() const {
    return _write_concern;
}

bsoncxx::v_noabi::stdx::optional<bool> const& insert::ordered() const {
    return _ordered;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> const& insert::comment() const {
    return _comment;
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
