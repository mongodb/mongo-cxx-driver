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

#include <mongocxx/v1/transaction.hh>

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

transaction::~transaction() {
    libmongoc::transaction_opts_destroy(to_mongoc(_impl));
}

transaction::transaction(transaction&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

transaction& transaction::operator=(transaction&& other) noexcept {
    if (this != &other) {
        libmongoc::transaction_opts_destroy(to_mongoc(exchange(_impl, exchange(other._impl, nullptr))));
    }
    return *this;
}

transaction::transaction(transaction const& other) : _impl{libmongoc::transaction_opts_clone(to_mongoc(other._impl))} {}

transaction& transaction::operator=(transaction const& other) {
    if (this != &other) {
        libmongoc::transaction_opts_destroy(
            to_mongoc(exchange(_impl, libmongoc::transaction_opts_clone(to_mongoc(other._impl)))));
    }
    return *this;
}

transaction::transaction() : _impl{libmongoc::transaction_opts_new()} {}

transaction& transaction::max_commit_time_ms(std::chrono::milliseconds v) {
    libmongoc::transaction_opts_set_max_commit_time_ms(to_mongoc(_impl), v.count());
    return *this;
}

bsoncxx::v1::stdx::optional<std::chrono::milliseconds> transaction::max_commit_time_ms() const {
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> ret;

    // DEFAULT_MAX_COMMIT_TIME_MS (0) is equivalent to "unset".
    if (auto const v = libmongoc::transaction_opts_get_max_commit_time_ms(to_mongoc(_impl))) {
        ret.emplace(v);
    }

    return ret;
}

transaction& transaction::read_concern(v1::read_concern const& v) {
    internal::set_read_concern(*this, v1::read_concern::internal::as_mongoc(v));
    return *this;
}

bsoncxx::v1::stdx::optional<v1::read_concern> transaction::read_concern() const {
    if (auto const rc = libmongoc::transaction_opts_get_read_concern(to_mongoc(_impl))) {
        return v1::read_concern::internal::make(libmongoc::read_concern_copy(rc));
    }

    return {};
}

transaction& transaction::read_preference(v1::read_preference const& v) {
    internal::set_read_preference(*this, v1::read_preference::internal::as_mongoc(v));
    return *this;
}

bsoncxx::v1::stdx::optional<v1::read_preference> transaction::read_preference() const {
    if (auto const rp = libmongoc::transaction_opts_get_read_prefs(to_mongoc(_impl))) {
        return v1::read_preference::internal::make(libmongoc::read_prefs_copy(rp));
    }

    return {};
}

transaction& transaction::write_concern(v1::write_concern const& v) {
    internal::set_write_concern(*this, v1::write_concern::internal::as_mongoc(v));
    return *this;
}

bsoncxx::v1::stdx::optional<v1::write_concern> transaction::write_concern() const {
    if (auto const wc = libmongoc::transaction_opts_get_write_concern(to_mongoc(_impl))) {
        return v1::write_concern::internal::make(libmongoc::write_concern_copy(wc));
    }

    return {};
}

mongoc_transaction_opt_t const* transaction::internal::as_mongoc(transaction const& self) {
    return to_mongoc(self._impl);
}

void transaction::internal::set_read_concern(transaction& self, mongoc_read_concern_t const* v) {
    libmongoc::transaction_opts_set_read_concern(to_mongoc(self._impl), v);
}

void transaction::internal::set_read_preference(transaction& self, mongoc_read_prefs_t const* v) {
    libmongoc::transaction_opts_set_read_prefs(to_mongoc(self._impl), v);
}

void transaction::internal::set_write_concern(transaction& self, mongoc_write_concern_t const* v) {
    libmongoc::transaction_opts_set_write_concern(to_mongoc(self._impl), v);
}

} // namespace v1
} // namespace mongocxx
