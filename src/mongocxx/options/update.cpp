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

#include <mongocxx/options/update.hpp>

#include <mongocxx/config/private/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

update& update::upsert(bool upsert) {
    _upsert = upsert;
    return *this;
}

update& update::bypass_document_validation(bool bypass_document_validation) {
    _bypass_document_validation = bypass_document_validation;
    return *this;
}

update& update::write_concern(class write_concern wc) {
    _write_concern = std::move(wc);
    return *this;
}

const stdx::optional<bool>& update::upsert() const {
    return _upsert;
}

const stdx::optional<bool>& update::bypass_document_validation() const {
    return _bypass_document_validation;
}

const stdx::optional<class write_concern>& update::write_concern() const {
    return _write_concern;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
