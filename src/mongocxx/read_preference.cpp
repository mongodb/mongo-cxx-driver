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

#include <mongocxx/read_preference.hpp>

#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>
#include <mongocxx/private/read_preference.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

read_preference::read_preference(read_preference&&) noexcept = default;
read_preference& read_preference::operator=(read_preference&&) noexcept = default;

read_preference::read_preference(const read_preference& other)
    : _impl(stdx::make_unique<impl>(libmongoc::read_prefs_copy(other._impl->read_preference_t))) {
}

read_preference& read_preference::operator=(const read_preference& other) {
    _impl.reset(stdx::make_unique<impl>(libmongoc::read_prefs_copy(other._impl->read_preference_t))
                    .release());
    return *this;
}

read_preference::read_preference(std::unique_ptr<impl>&& implementation) {
    _impl.reset(implementation.release());
}

read_preference::read_preference(read_mode mode)
    : _impl(stdx::make_unique<impl>(
          libmongoc::read_prefs_new(static_cast<mongoc_read_mode_t>(mode)))) {
}

read_preference::read_preference(read_mode mode, bsoncxx::document::view_or_value tags)
    : read_preference(mode) {
    read_preference::tags(std::move(tags));
}

read_preference::~read_preference() = default;

void read_preference::mode(read_mode mode) {
    libmongoc::read_prefs_set_mode(_impl->read_preference_t, static_cast<mongoc_read_mode_t>(mode));
}

void read_preference::tags(bsoncxx::document::view_or_value tags) {
    libbson::scoped_bson_t scoped_bson_tags(std::move(tags));
    libmongoc::read_prefs_set_tags(_impl->read_preference_t, scoped_bson_tags.bson());
}

read_preference::read_mode read_preference::mode() const {
    return static_cast<read_mode>(libmongoc::read_prefs_get_mode(_impl->read_preference_t));
}

stdx::optional<bsoncxx::document::view> read_preference::tags() const {
    const bson_t* bson_tags = libmongoc::read_prefs_get_tags(_impl->read_preference_t);

    if (bson_count_keys(bson_tags))
        return bsoncxx::document::view(bson_get_data(bson_tags), bson_tags->len);

    return stdx::optional<bsoncxx::document::view>{};
}

void read_preference::max_staleness(std::chrono::seconds max_staleness) {
    auto max_staleness_sec = max_staleness.count();
    if (max_staleness_sec <= 0) {
        throw logic_error{error_code::k_invalid_parameter};
    }
    libmongoc::read_prefs_set_max_staleness_seconds(_impl->read_preference_t, max_staleness_sec);
}

stdx::optional<std::chrono::seconds> read_preference::max_staleness() const {
    auto staleness = libmongoc::read_prefs_get_max_staleness_seconds(_impl->read_preference_t);

    // libmongoc signals "disabled" with the value -1.
    if (staleness == -1) {
        return stdx::nullopt;
    }

    return std::chrono::seconds{staleness};
}

bool operator==(const read_preference& lhs, const read_preference& rhs) {
    return (lhs.mode() == rhs.mode()) && (lhs.tags() == rhs.tags()) &&
           (lhs.max_staleness() == rhs.max_staleness());
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
