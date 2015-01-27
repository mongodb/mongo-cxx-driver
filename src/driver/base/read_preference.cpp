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

#include "driver/base/read_preference.hpp"

#include "bson/libbson.hpp"
#include "driver/base/private/read_preference.hpp"

#include "stdx/make_unique.hpp"

#include "driver/private/libmongoc.hpp"

namespace mongo {
namespace driver {
namespace base {

read_preference::read_preference(read_preference&&) noexcept = default;
read_preference& read_preference::operator=(read_preference&&) noexcept = default;

read_preference::read_preference(const read_preference& other)
    : _impl(stdx::make_unique<impl>(libmongoc::read_prefs_copy(other._impl->read_preference_t))) {}

read_preference& read_preference::operator=(const read_preference& other) {
    _impl.reset(
        stdx::make_unique<impl>(libmongoc::read_prefs_copy(other._impl->read_preference_t)).release());
    return *this;
}

read_preference::read_preference(std::unique_ptr<impl>&& implementation) {
    _impl.reset(implementation.release());
}

read_preference::read_preference(read_mode mode)
    : _impl(stdx::make_unique<impl>(libmongoc::read_prefs_new(static_cast<mongoc_read_mode_t>(mode)))) {
}

read_preference::read_preference(read_mode mode, bson::document::view tags)
    : read_preference(mode) {
    read_preference::tags(tags);
}

read_preference::~read_preference() = default;

void read_preference::mode(read_mode mode) {
    libmongoc::read_prefs_set_mode(_impl->read_preference_t, static_cast<mongoc_read_mode_t>(mode));
}

void read_preference::tags(bson::document::view tags) {
    bson::libbson::scoped_bson_t scoped_bson_tags(tags);
    libmongoc::read_prefs_set_tags(_impl->read_preference_t, scoped_bson_tags.bson());
}

read_preference::read_mode read_preference::mode() const {
    return static_cast<read_mode>(libmongoc::read_prefs_get_mode(_impl->read_preference_t));
}

optional<bson::document::view> read_preference::tags() const {
    const bson_t* bson_tags = libmongoc::read_prefs_get_tags(_impl->read_preference_t);

    if (bson_count_keys(bson_tags))
        return bson::document::view(bson_get_data(bson_tags), bson_tags->len);

    return optional<bson::document::view>{};
}

}  // namespace base
}  // namespace driver
}  // namespace mongo
