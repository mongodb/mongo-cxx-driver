// Copyright 2018-present MongoDB Inc.
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

#include <mongocxx/options/replace.hpp>

#include <bsoncxx/array/view_or_value.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

replace& replace::bypass_document_validation(bool bypass_document_validation) {
    _bypass_document_validation = bypass_document_validation;
    return *this;
}

replace& replace::collation(bsoncxx::document::view_or_value collation) {
    _collation = std::move(collation);
    return *this;
}

replace& replace::upsert(bool upsert) {
    _upsert = upsert;
    return *this;
}

replace& replace::write_concern(class write_concern wc) {
    _write_concern = std::move(wc);
    return *this;
}

const stdx::optional<bool>& replace::bypass_document_validation() const {
    return _bypass_document_validation;
}

const stdx::optional<bsoncxx::document::view_or_value>& replace::collation() const {
    return _collation;
}

const stdx::optional<bool>& replace::upsert() const {
    return _upsert;
}

const stdx::optional<class write_concern>& replace::write_concern() const {
    return _write_concern;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
