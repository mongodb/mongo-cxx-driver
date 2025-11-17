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

#include <mongocxx/v1/read_concern.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <cstring>
#include <string>
#include <unordered_map>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

namespace {

mongoc_read_concern_t* to_mongoc(void* ptr) {
    return static_cast<mongoc_read_concern_t*>(ptr);
}

} // namespace

read_concern::~read_concern() {
    libmongoc::read_concern_destroy(to_mongoc(_impl));
}

read_concern::read_concern(read_concern&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

read_concern& read_concern::operator=(read_concern&& other) noexcept {
    if (this != &other) {
        libmongoc::read_concern_destroy(to_mongoc(exchange(_impl, exchange(other._impl, nullptr))));
    }
    return *this;
}

read_concern::read_concern(read_concern const& other) : _impl{libmongoc::read_concern_copy(to_mongoc(other._impl))} {}

read_concern& read_concern::operator=(read_concern const& other) {
    if (this != &other) {
        libmongoc::read_concern_destroy(
            to_mongoc(exchange(_impl, libmongoc::read_concern_copy(to_mongoc(other._impl)))));
    }
    return *this;
}

read_concern::read_concern() : _impl{libmongoc::read_concern_new()} {}

read_concern& read_concern::acknowledge_level(level v) {
    // Return value is always true: no input validation by mongoc.
    switch (v) {
        case level::k_local:
            (void)libmongoc::read_concern_set_level(to_mongoc(_impl), MONGOC_READ_CONCERN_LEVEL_LOCAL);
            break;
        case level::k_majority:
            (void)libmongoc::read_concern_set_level(to_mongoc(_impl), MONGOC_READ_CONCERN_LEVEL_MAJORITY);
            break;
        case level::k_linearizable:
            (void)libmongoc::read_concern_set_level(to_mongoc(_impl), MONGOC_READ_CONCERN_LEVEL_LINEARIZABLE);
            break;

        case level::k_server_default:
            (void)libmongoc::read_concern_set_level(to_mongoc(_impl), nullptr);
            break;

        case level::k_available:
            (void)libmongoc::read_concern_set_level(to_mongoc(_impl), MONGOC_READ_CONCERN_LEVEL_AVAILABLE);
            break;
        case level::k_snapshot:
            (void)libmongoc::read_concern_set_level(to_mongoc(_impl), MONGOC_READ_CONCERN_LEVEL_SNAPSHOT);
            break;

        default:
        case level::k_unknown:
            // Precondition violation: undocumented but well-defined behavior.
            (void)libmongoc::read_concern_set_level(to_mongoc(_impl), nullptr);
            break;
    }

    return *this;
}

read_concern::level read_concern::acknowledge_level() const {
    auto const level_cstr = libmongoc::read_concern_get_level(to_mongoc(_impl));

    if (!level_cstr) {
        return level::k_server_default;
    }

    std::unordered_map<bsoncxx::v1::stdx::string_view, level> const map = {
        {MONGOC_READ_CONCERN_LEVEL_LOCAL, level::k_local},
        {MONGOC_READ_CONCERN_LEVEL_MAJORITY, level::k_majority},
        {MONGOC_READ_CONCERN_LEVEL_LINEARIZABLE, level::k_linearizable},
        {MONGOC_READ_CONCERN_LEVEL_AVAILABLE, level::k_available},
        {MONGOC_READ_CONCERN_LEVEL_SNAPSHOT, level::k_snapshot},
    };

    auto const iter = map.find(level_cstr);

    return iter != map.end() ? iter->second : level::k_unknown;
}

read_concern& read_concern::acknowledge_string(bsoncxx::v1::stdx::string_view v) {
    // Return value is always true: no input validation by mongoc.
    (void)libmongoc::read_concern_set_level(to_mongoc(_impl), v.empty() ? nullptr : std::string{v}.c_str());
    return *this;
}

bsoncxx::v1::stdx::string_view read_concern::acknowledge_string() const {
    bsoncxx::v1::stdx::string_view ret;

    if (auto const level_cstr = libmongoc::read_concern_get_level(to_mongoc(_impl))) {
        ret = bsoncxx::v1::stdx::string_view{level_cstr};
    }

    return ret;
}

bsoncxx::v1::document::value read_concern::to_document() const {
    scoped_bson doc;

    if (auto const level_cstr = libmongoc::read_concern_get_level(to_mongoc(_impl))) {
        doc += scoped_bson{BCON_NEW("level", BCON_UTF8(level_cstr))};
    }

    return std::move(doc).value();
}

bool operator==(read_concern const& lhs, read_concern const& rhs) {
    auto const lhs_cstr = libmongoc::read_concern_get_level(to_mongoc(lhs._impl));
    auto const rhs_cstr = libmongoc::read_concern_get_level(to_mongoc(rhs._impl));

    if (!lhs_cstr != !rhs_cstr) {
        return false;
    }

    return !lhs_cstr || std::strcmp(lhs_cstr, rhs_cstr) == 0;
}

read_concern::read_concern(void* impl) : _impl{impl} {}

read_concern read_concern::internal::make(mongoc_read_concern_t* rc) {
    return {rc};
}

mongoc_read_concern_t const* read_concern::internal::as_mongoc(read_concern const& self) {
    return to_mongoc(self._impl);
}

} // namespace v1
} // namespace mongocxx
