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

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>

#include <bsoncxx/private/make_unique.hh>

namespace mongocxx {
namespace v_noabi {
namespace options {

transaction::transaction() : _impl{bsoncxx::make_unique<impl>()} {}

transaction::transaction(transaction&&) noexcept = default;
transaction& transaction::operator=(transaction&&) noexcept = default;

transaction::transaction(transaction const& other)
    : _impl{bsoncxx::make_unique<impl>(other._get_impl().get_transaction_opt_t())} {}

transaction& transaction::operator=(transaction const& other) {
    _impl = bsoncxx::make_unique<impl>(other._get_impl().get_transaction_opt_t());
    return *this;
}

transaction::~transaction() noexcept = default;

transaction& transaction::read_concern(mongocxx::v_noabi::read_concern const& rc) {
    _impl->read_concern(rc);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::read_concern> transaction::read_concern() const {
    return _impl->read_concern();
}

transaction& transaction::write_concern(mongocxx::v_noabi::write_concern const& wc) {
    _impl->write_concern(wc);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> transaction::write_concern() const {
    return _impl->write_concern();
}

transaction& transaction::read_preference(mongocxx::v_noabi::read_preference const& rp) {
    _impl->read_preference(rp);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::read_preference> transaction::read_preference() const {
    return _impl->read_preference();
}

transaction& transaction::max_commit_time_ms(std::chrono::milliseconds ms) {
    _impl->max_commit_time_ms(ms);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<std::chrono::milliseconds> transaction::max_commit_time_ms() const {
    return _impl->max_commit_time_ms();
}

transaction::impl const& transaction::_get_impl() const {
    if (!_impl) {
        throw logic_error{error_code::k_invalid_transaction_options_object};
    }
    return *_impl;
}

transaction::impl& transaction::_get_impl() {
    auto cthis = const_cast<transaction const*>(this);
    return const_cast<transaction::impl&>(cthis->_get_impl());
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
