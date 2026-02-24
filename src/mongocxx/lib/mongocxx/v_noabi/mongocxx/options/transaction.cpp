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

#include <mongocxx/options/transaction.hh>

//

#include <mongocxx/v1/transaction_options.hh>

#include <chrono>
#include <utility>

#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>

#include <mongocxx/read_concern.hh>
#include <mongocxx/read_preference.hh>
#include <mongocxx/write_concern.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {
namespace options {

namespace {

template <typename Transaction>
Transaction& check_moved_from(Transaction& txn) {
    if (!v1::transaction_options::internal::as_mongoc(txn)) {
        throw logic_error{v_noabi::error_code::k_invalid_transaction_options_object};
    }
    return txn;
}

} // namespace

transaction::transaction(transaction const& other) : _txn{check_moved_from(other._txn)} {}

// NOLINTNEXTLINE(cert-oop54-cpp): handled by v1::transaction.
transaction& transaction::operator=(transaction const& other) {
    _txn = check_moved_from(other._txn);
    return *this;
}

transaction& transaction::read_concern(v_noabi::read_concern const& rc) {
    v1::transaction_options::internal::set_read_concern(
        check_moved_from(_txn), v_noabi::read_concern::internal::as_mongoc(rc));
    return *this;
}

bsoncxx::v_noabi::stdx::optional<v_noabi::read_concern> transaction::read_concern() const {
    return check_moved_from(_txn).read_concern();
}

transaction& transaction::write_concern(v_noabi::write_concern const& wc) {
    v1::transaction_options::internal::set_write_concern(
        check_moved_from(_txn), v_noabi::write_concern::internal::as_mongoc(wc));
    return *this;
}

bsoncxx::v_noabi::stdx::optional<v_noabi::write_concern> transaction::write_concern() const {
    return check_moved_from(_txn).write_concern();
}

transaction& transaction::read_preference(v_noabi::read_preference const& rp) {
    v1::transaction_options::internal::set_read_preference(
        check_moved_from(_txn), v_noabi::read_preference::internal::as_mongoc(rp));
    return *this;
}

bsoncxx::v_noabi::stdx::optional<v_noabi::read_preference> transaction::read_preference() const {
    return check_moved_from(_txn).read_preference();
}

transaction& transaction::max_commit_time_ms(std::chrono::milliseconds ms) {
    check_moved_from(_txn).max_commit_time_ms(ms);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<std::chrono::milliseconds> transaction::max_commit_time_ms() const {
    return check_moved_from(_txn).max_commit_time_ms();
}

v1::transaction_options const& transaction::internal::as_v1(transaction const& self) {
    return check_moved_from(self._txn);
}

mongoc_transaction_opt_t const* transaction::internal::as_mongoc(transaction const& self) {
    return v1::transaction_options::internal::as_mongoc(check_moved_from(self._txn));
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
