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

#include <mongocxx/v1/read_preference.hh>

//

#include <bsoncxx/v1/array/view.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>

#include <chrono>
#include <tuple>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/suppress_deprecation_warnings.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

namespace {

mongoc_read_prefs_t* to_mongoc(void* ptr) {
    return static_cast<mongoc_read_prefs_t*>(ptr);
}

} // namespace

read_preference::~read_preference() {
    libmongoc::read_prefs_destroy(to_mongoc(_impl));
}

read_preference::read_preference(read_preference&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

read_preference& read_preference::operator=(read_preference&& other) noexcept {
    if (this != &other) {
        libmongoc::read_prefs_destroy(to_mongoc(exchange(_impl, exchange(other._impl, nullptr))));
    }
    return *this;
}

read_preference::read_preference(read_preference const& other)
    : _impl{libmongoc::read_prefs_copy(to_mongoc(other._impl))} {}

read_preference& read_preference::operator=(read_preference const& other) {
    if (this != &other) {
        libmongoc::read_prefs_destroy(to_mongoc(exchange(_impl, libmongoc::read_prefs_copy(to_mongoc(other._impl)))));
    }
    return *this;
}

read_preference::read_preference() : _impl{libmongoc::read_prefs_new(MONGOC_READ_PRIMARY)} {}

read_preference& read_preference::hedge(bsoncxx::v1::document::view v) {
    BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_BEGIN
    libmongoc::read_prefs_set_hedge(to_mongoc(_impl), scoped_bson_view{v}.bson());
    BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_END
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> read_preference::hedge() const {
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> ret;

    BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_BEGIN
    auto const doc = libmongoc::read_prefs_get_hedge(to_mongoc(_impl));
    BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_END

    if (!bson_empty(doc)) {
        ret.emplace(bson_get_data(doc), doc->len);
    }

    return ret;
}

read_preference& read_preference::mode(read_mode v) {
    switch (v) {
        default: // Precondition violation: undocumented but well-defined behavior.
        case read_mode::k_primary:
            libmongoc::read_prefs_set_mode(to_mongoc(_impl), MONGOC_READ_PRIMARY);
            break;

        case read_mode::k_primary_preferred:
            libmongoc::read_prefs_set_mode(to_mongoc(_impl), MONGOC_READ_PRIMARY_PREFERRED);
            break;

        case read_mode::k_secondary:
            libmongoc::read_prefs_set_mode(to_mongoc(_impl), MONGOC_READ_SECONDARY);
            break;

        case read_mode::k_secondary_preferred:
            libmongoc::read_prefs_set_mode(to_mongoc(_impl), MONGOC_READ_SECONDARY_PREFERRED);
            break;

        case read_mode::k_nearest:
            libmongoc::read_prefs_set_mode(to_mongoc(_impl), MONGOC_READ_NEAREST);
            break;
    }

    return *this;
}

read_preference::read_mode read_preference::mode() const {
    switch (libmongoc::read_prefs_get_mode(to_mongoc(_impl))) {
        default:
        case MONGOC_READ_PRIMARY:
            return read_mode::k_primary;
        case MONGOC_READ_PRIMARY_PREFERRED:
            return read_mode::k_primary_preferred;
        case MONGOC_READ_SECONDARY:
            return read_mode::k_secondary;
        case MONGOC_READ_SECONDARY_PREFERRED:
            return read_mode::k_secondary_preferred;
        case MONGOC_READ_NEAREST:
            return read_mode::k_nearest;
    }
}

read_preference& read_preference::tags(bsoncxx::v1::array::view v) {
    libmongoc::read_prefs_set_tags(to_mongoc(_impl), scoped_bson_view{v}.bson());
    return *this;
}

read_preference& read_preference::tags(bsoncxx::v1::document::view v) {
    libmongoc::read_prefs_set_tags(to_mongoc(_impl), scoped_bson_view{v}.bson());
    return *this;
}

bsoncxx::v1::array::view read_preference::tags() const {
    auto const doc = libmongoc::read_prefs_get_tags(to_mongoc(_impl));
    return {bson_get_data(doc), doc->len};
}

read_preference& read_preference::max_staleness(std::chrono::seconds v) {
    libmongoc::read_prefs_set_max_staleness_seconds(to_mongoc(_impl), v.count());
    return *this;
}

bsoncxx::v1::stdx::optional<std::chrono::seconds> read_preference::max_staleness() const {
    bsoncxx::v1::stdx::optional<std::chrono::seconds> ret;

    auto const v = libmongoc::read_prefs_get_max_staleness_seconds(to_mongoc(_impl));

    if (v != MONGOC_NO_MAX_STALENESS) {
        ret.emplace(v);
    }

    return ret;
}

bool operator==(read_preference const& lhs, read_preference const& rhs) {
    BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_BEGIN

    // Lexicographic comparison of data members.
    return std::make_tuple(lhs.hedge(), lhs.mode(), lhs.tags(), lhs.max_staleness()) ==
           std::make_tuple(rhs.hedge(), rhs.mode(), rhs.tags(), rhs.max_staleness());

    BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_END
}

read_preference::read_preference(void* impl) : _impl{impl} {}

read_preference read_preference::internal::make(mongoc_read_prefs_t* rc) {
    return {rc};
}

mongoc_read_prefs_t const* read_preference::internal::as_mongoc(read_preference const& self) {
    return to_mongoc(self._impl);
}

} // namespace v1
} // namespace mongocxx
