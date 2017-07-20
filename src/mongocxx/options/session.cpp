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

#include <mongocxx/options/session.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

session& session::write_concern(class write_concern write_concern) {
    _write_concern = std::move(write_concern);
    return *this;
}

const stdx::optional<mongocxx::write_concern>& session::write_concern() const {
    return _write_concern;
}

session& session::read_concern(class read_concern read_concern) {
    _read_concern = std::move(read_concern);
    return *this;
}

const stdx::optional<class read_concern>& session::read_concern() const {
    return _read_concern;
}

session& session::read_preference(class read_preference read_preference) {
    _read_pref = std::move(read_preference);
    return *this;
}

const stdx::optional<class read_preference>& session::read_preference() const {
    return _read_pref;
}

bool MONGOCXX_CALL operator==(const session& lhs, const session& rhs) {
    return lhs._read_pref == rhs._read_pref && lhs._read_concern == rhs._read_concern &&
           lhs._write_concern == rhs._write_concern;
}

bool MONGOCXX_CALL operator!=(const session& lhs, const session& rhs) {
    return !(lhs == rhs);
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx