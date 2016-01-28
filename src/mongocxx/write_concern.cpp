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

#include <mongocxx/write_concern.hpp>

#include <limits>

#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/private/error_category.hpp>
#include <mongocxx/private/libmongoc.hpp>
#include <mongocxx/private/write_concern.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/stdx.hpp>

#include <mongocxx/config/private/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

write_concern::write_concern() : _impl{stdx::make_unique<impl>(libmongoc::write_concern_new())} {
}

write_concern::write_concern(std::unique_ptr<impl>&& implementation) {
    _impl.reset(implementation.release());
}

write_concern::write_concern(write_concern&&) noexcept = default;
write_concern& write_concern::operator=(write_concern&&) noexcept = default;

write_concern::write_concern(const write_concern& other)
    : _impl(stdx::make_unique<impl>(libmongoc::write_concern_copy(other._impl->write_concern_t))) {
}

write_concern& write_concern::operator=(const write_concern& other) {
    _impl.reset(stdx::make_unique<impl>(libmongoc::write_concern_copy(other._impl->write_concern_t))
                    .release());
    return *this;
}

write_concern::~write_concern() = default;

void write_concern::journal(bool journal) {
    libmongoc::write_concern_set_journal(_impl->write_concern_t, journal);
}

void write_concern::nodes(std::int32_t confirm_from) {
    libmongoc::write_concern_set_w(_impl->write_concern_t, confirm_from);
}

void write_concern::acknowledge_level(write_concern::level confirm_level) {
    std::int32_t w;
    switch (confirm_level) {
        case write_concern::level::k_default:
            w = MONGOC_WRITE_CONCERN_W_DEFAULT;
            break;
        case write_concern::level::k_majority:
            w = MONGOC_WRITE_CONCERN_W_MAJORITY;
            break;
        case write_concern::level::k_unacknowledged:
            w = MONGOC_WRITE_CONCERN_W_UNACKNOWLEDGED;
            break;
        case write_concern::level::k_tag:
            // no exception for setting tag if it's set
            if (libmongoc::write_concern_get_w(_impl->write_concern_t) ==
                MONGOC_WRITE_CONCERN_W_TAG)
                return;
        default:
            throw exception{error_code::k_unknown_write_concern};
    }
    libmongoc::write_concern_set_w(_impl->write_concern_t, w);
}

void write_concern::tag(stdx::string_view confirm_from) {
    libmongoc::write_concern_set_wtag(_impl->write_concern_t, confirm_from.to_string().data());
}

void write_concern::majority(std::chrono::milliseconds timeout) {
    const auto count = timeout.count();
    if ((count < 0) || (count >= std::numeric_limits<std::int32_t>::max()))
        throw logic_error{error_code::k_invalid_parameter};
    libmongoc::write_concern_set_wmajority(_impl->write_concern_t,
                                           static_cast<std::int32_t>(count));
}

void write_concern::timeout(std::chrono::milliseconds timeout) {
    const auto count = timeout.count();
    if ((count < 0) || (count >= std::numeric_limits<std::int32_t>::max()))
        throw logic_error{error_code::k_invalid_parameter};
    libmongoc::write_concern_set_wtimeout(_impl->write_concern_t, static_cast<std::int32_t>(count));
}

bool write_concern::journal() const {
    return libmongoc::write_concern_get_journal(_impl->write_concern_t);
}

stdx::optional<std::int32_t> write_concern::nodes() const {
    std::int32_t w = libmongoc::write_concern_get_w(_impl->write_concern_t);
    return w >= 1 ? stdx::optional<std::int32_t>{w} : stdx::nullopt;
}

stdx::optional<write_concern::level> write_concern::acknowledge_level() const {
    stdx::optional<write_concern::level> ack_level;
    std::int32_t w = libmongoc::write_concern_get_w(_impl->write_concern_t);
    if (w >= 1) return stdx::nullopt;
    switch (w) {
        case MONGOC_WRITE_CONCERN_W_UNACKNOWLEDGED:
            return write_concern::level::k_unacknowledged;
        case MONGOC_WRITE_CONCERN_W_DEFAULT:
            return write_concern::level::k_default;
        case MONGOC_WRITE_CONCERN_W_MAJORITY:
            return write_concern::level::k_majority;
        case MONGOC_WRITE_CONCERN_W_TAG:
            return write_concern::level::k_tag;
        default:
            return write_concern::level::k_unknown;
    }
}

stdx::optional<std::string> write_concern::tag() const {
    const char* tag_str = libmongoc::write_concern_get_wtag(_impl->write_concern_t);
    return tag_str ? stdx::make_optional<std::string>(tag_str) : stdx::nullopt;
}

bool write_concern::majority() const {
    return libmongoc::write_concern_get_wmajority(_impl->write_concern_t);
}

std::chrono::milliseconds write_concern::timeout() const {
    return std::chrono::milliseconds(libmongoc::write_concern_get_wtimeout(_impl->write_concern_t));
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
