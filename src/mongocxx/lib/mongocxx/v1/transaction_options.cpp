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

#include <mongocxx/v1/transaction_options.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/read_concern.hh>
#include <mongocxx/v1/read_preference.hh>
#include <mongocxx/v1/write_concern.hh>

#include <chrono>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

namespace {

mongoc_transaction_opt_t* to_mongoc(void* ptr) {
    return static_cast<mongoc_transaction_opt_t*>(ptr);
}

} // namespace

transaction_options::~transaction_options() {
    libmongoc::transaction_opts_destroy(to_mongoc(_impl));
}

transaction_options::transaction_options(transaction_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

transaction_options& transaction_options::operator=(transaction_options&& other) noexcept {
    if (this != &other) {
        libmongoc::transaction_opts_destroy(to_mongoc(exchange(_impl, exchange(other._impl, nullptr))));
    }
    return *this;
}

transaction_options::transaction_options(transaction_options const& other)
    : _impl{libmongoc::transaction_opts_clone(to_mongoc(other._impl))} {}

transaction_options& transaction_options::operator=(transaction_options const& other) {
    if (this != &other) {
        libmongoc::transaction_opts_destroy(
            to_mongoc(exchange(_impl, libmongoc::transaction_opts_clone(to_mongoc(other._impl)))));
    }
    return *this;
}

transaction_options::transaction_options() : _impl{libmongoc::transaction_opts_new()} {}

transaction_options& transaction_options::max_commit_time_ms(std::chrono::milliseconds v) {
    libmongoc::transaction_opts_set_max_commit_time_ms(to_mongoc(_impl), v.count());
    return *this;
}

bsoncxx::v1::stdx::optional<std::chrono::milliseconds> transaction_options::max_commit_time_ms() const {
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> ret;

    // DEFAULT_MAX_COMMIT_TIME_MS (0) is equivalent to "unset".
    if (auto const v = libmongoc::transaction_opts_get_max_commit_time_ms(to_mongoc(_impl))) {
        ret.emplace(v);
    }

    return ret;
}

transaction_options& transaction_options::read_concern(v1::read_concern const& v) {
    internal::set_read_concern(*this, v1::read_concern::internal::as_mongoc(v));
    return *this;
}

bsoncxx::v1::stdx::optional<v1::read_concern> transaction_options::read_concern() const {
    if (auto const rc = libmongoc::transaction_opts_get_read_concern(to_mongoc(_impl))) {
        return v1::read_concern::internal::make(libmongoc::read_concern_copy(rc));
    }

    return {};
}

transaction_options& transaction_options::read_preference(v1::read_preference const& v) {
    internal::set_read_preference(*this, v1::read_preference::internal::as_mongoc(v));
    return *this;
}

bsoncxx::v1::stdx::optional<v1::read_preference> transaction_options::read_preference() const {
    if (auto const rp = libmongoc::transaction_opts_get_read_prefs(to_mongoc(_impl))) {
        return v1::read_preference::internal::make(libmongoc::read_prefs_copy(rp));
    }

    return {};
}

transaction_options& transaction_options::write_concern(v1::write_concern const& v) {
    internal::set_write_concern(*this, v1::write_concern::internal::as_mongoc(v));
    return *this;
}

bsoncxx::v1::stdx::optional<v1::write_concern> transaction_options::write_concern() const {
    if (auto const wc = libmongoc::transaction_opts_get_write_concern(to_mongoc(_impl))) {
        return v1::write_concern::internal::make(libmongoc::write_concern_copy(wc));
    }

    return {};
}

transaction_options::transaction_options(void* impl) : _impl{impl} {}

transaction_options transaction_options::internal::make(mongoc_transaction_opt_t* impl) {
    return {impl};
}

mongoc_transaction_opt_t const* transaction_options::internal::as_mongoc(transaction_options const& self) {
    return to_mongoc(self._impl);
}

void transaction_options::internal::set_read_concern(transaction_options& self, mongoc_read_concern_t const* v) {
    libmongoc::transaction_opts_set_read_concern(to_mongoc(self._impl), v);
}

void transaction_options::internal::set_read_preference(transaction_options& self, mongoc_read_prefs_t const* v) {
    libmongoc::transaction_opts_set_read_prefs(to_mongoc(self._impl), v);
}

void transaction_options::internal::set_write_concern(transaction_options& self, mongoc_write_concern_t const* v) {
    libmongoc::transaction_opts_set_write_concern(to_mongoc(self._impl), v);
}

} // namespace v1
} // namespace mongocxx
