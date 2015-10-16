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

#include <mongocxx/private/write_concern.hpp>

#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/private/libmongoc.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

write_concern::write_concern()
    : _impl{stdx::make_unique<impl>(libmongoc::write_concern_new())} {
}

write_concern::write_concern(std::unique_ptr<impl>&& implementation) {
    _impl.reset(implementation.release());
}

write_concern::write_concern(write_concern&&) noexcept = default;
write_concern& write_concern::operator=(write_concern&&) noexcept = default;

write_concern::write_concern(const write_concern& other)
    : _impl(stdx::make_unique<impl>(
          libmongoc::write_concern_copy(other._impl->write_concern_t))) {
}

write_concern& write_concern::operator=(const write_concern& other) {
    _impl.reset(stdx::make_unique<impl>(
                    libmongoc::write_concern_copy(other._impl->write_concern_t)).release());
    return *this;
}

write_concern::~write_concern() = default;

void write_concern::fsync(bool fsync) {
    libmongoc::write_concern_set_fsync(_impl->write_concern_t, fsync);
}

void* write_concern::implementation() const {
    return _impl->write_concern_t;
}

void write_concern::journal(bool journal) {
    libmongoc::write_concern_set_journal(_impl->write_concern_t, journal);
}

void write_concern::nodes(std::int32_t confirm_from) {
    libmongoc::write_concern_set_w(_impl->write_concern_t, confirm_from);
}

void write_concern::tag(stdx::string_view confirm_from) {
    libmongoc::write_concern_set_wtag(_impl->write_concern_t, confirm_from.data());
}

void write_concern::majority(std::chrono::milliseconds timeout) {
    libmongoc::write_concern_set_wmajority(_impl->write_concern_t, timeout.count());
}

void write_concern::timeout(std::chrono::milliseconds timeout) {
    libmongoc::write_concern_set_wtimeout(_impl->write_concern_t, timeout.count());
}

bool write_concern::fsync() const {
    return libmongoc::write_concern_get_fsync(_impl->write_concern_t);
}

bool write_concern::journal() const {
    return libmongoc::write_concern_get_journal(_impl->write_concern_t);
}

std::int32_t write_concern::nodes() const {
    return libmongoc::write_concern_get_w(_impl->write_concern_t);
}

stdx::optional<std::string> write_concern::tag() const {
    stdx::optional<std::string> to_return;
    const char* tag_str = libmongoc::write_concern_get_wtag(_impl->write_concern_t);
    if (tag_str) {
        to_return = tag_str;
    }
    return to_return;
}

bool write_concern::majority() const {
    return libmongoc::write_concern_get_wmajority(_impl->write_concern_t);
}

std::chrono::milliseconds write_concern::timeout() const {
    return std::chrono::milliseconds(libmongoc::write_concern_get_wtimeout(_impl->write_concern_t));
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
