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

#include <mongocxx/options/index_view.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

index_view::index_view() : _max_time(), _write_concern() {}

const bsoncxx::stdx::optional<mongocxx::write_concern>& index_view::write_concern() const {
    return _write_concern;
}

const bsoncxx::stdx::optional<std::chrono::milliseconds>& index_view::max_time() const {
    return _max_time;
}

index_view& index_view::max_time(std::chrono::milliseconds max_time) {
    _max_time = max_time;
    return *this;
}

index_view& index_view::write_concern(mongocxx::write_concern write_concern) {
    _write_concern = std::move(write_concern);
    return *this;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
