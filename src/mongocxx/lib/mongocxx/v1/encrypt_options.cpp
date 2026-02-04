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

#include <mongocxx/v1/encrypt_options.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/range_options.hpp>
#include <mongocxx/v1/text_options.hpp>

#include <cstdint>
#include <string>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class encrypt_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _key_id;
    bsoncxx::v1::stdx::optional<std::string> _key_alt_name;
    bsoncxx::v1::stdx::optional<encryption_algorithm> _algorithm;
    bsoncxx::v1::stdx::optional<std::int64_t> _contention_factor;
    bsoncxx::v1::stdx::optional<encryption_query_type> _query_type;
    bsoncxx::v1::stdx::optional<v1::range_options> _range_opts;
    bsoncxx::v1::stdx::optional<v1::text_options> _text_opts;

    static impl const& with(encrypt_options const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(encrypt_options const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(encrypt_options& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(encrypt_options* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

encrypt_options::~encrypt_options() {
    delete impl::with(_impl);
}

encrypt_options::encrypt_options(encrypt_options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

encrypt_options& encrypt_options::operator=(encrypt_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

encrypt_options::encrypt_options(encrypt_options const& other) : _impl{new impl{impl::with(other)}} {}

encrypt_options& encrypt_options::operator=(encrypt_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

encrypt_options::encrypt_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

encrypt_options& encrypt_options::key_id(bsoncxx::v1::types::value v) {
    impl::with(this)->_key_id = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> encrypt_options::key_id() const {
    return impl::with(this)->_key_id;
}

encrypt_options& encrypt_options::key_alt_name(std::string v) {
    impl::with(this)->_key_alt_name = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> encrypt_options::key_alt_name() const {
    return impl::with(this)->_key_alt_name;
}

encrypt_options& encrypt_options::algorithm(encryption_algorithm v) {
    impl::with(this)->_algorithm = v;
    return *this;
}

bsoncxx::v1::stdx::optional<encrypt_options::encryption_algorithm> encrypt_options::algorithm() const {
    return impl::with(this)->_algorithm;
}

encrypt_options& encrypt_options::contention_factor(std::int64_t v) {
    impl::with(this)->_contention_factor = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int64_t> encrypt_options::contention_factor() const {
    return impl::with(this)->_contention_factor;
}

encrypt_options& encrypt_options::query_type(encryption_query_type v) {
    impl::with(this)->_query_type = v;
    return *this;
}

bsoncxx::v1::stdx::optional<encrypt_options::encryption_query_type> encrypt_options::query_type() const {
    return impl::with(this)->_query_type;
}

encrypt_options& encrypt_options::range_opts(v1::range_options v) {
    impl::with(this)->_range_opts = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::range_options> encrypt_options::range_opts() const {
    return impl::with(this)->_range_opts;
}

encrypt_options& encrypt_options::text_opts(v1::text_options v) {
    impl::with(this)->_text_opts = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::text_options> encrypt_options::text_opts() const {
    return impl::with(this)->_text_opts;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> const& encrypt_options::internal::key_id(
    encrypt_options const& self) {
    return impl::with(self)._key_id;
}

bsoncxx::v1::stdx::optional<std::string> const& encrypt_options::internal::key_alt_name(encrypt_options const& self) {
    return impl::with(self)._key_alt_name;
}

bsoncxx::v1::stdx::optional<encrypt_options::encryption_algorithm> const& encrypt_options::internal::algorithm(
    encrypt_options const& self) {
    return impl::with(self)._algorithm;
}

bsoncxx::v1::stdx::optional<v1::range_options> const& encrypt_options::internal::range_opts(
    encrypt_options const& self) {
    return impl::with(self)._range_opts;
}

bsoncxx::v1::stdx::optional<v1::text_options> const& encrypt_options::internal::text_opts(encrypt_options const& self) {
    return impl::with(self)._text_opts;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& encrypt_options::internal::key_id(encrypt_options& self) {
    return impl::with(self)._key_id;
}

bsoncxx::v1::stdx::optional<std::string>& encrypt_options::internal::key_alt_name(encrypt_options& self) {
    return impl::with(self)._key_alt_name;
}

bsoncxx::v1::stdx::optional<encrypt_options::encryption_algorithm>& encrypt_options::internal::algorithm(
    encrypt_options& self) {
    return impl::with(self)._algorithm;
}

bsoncxx::v1::stdx::optional<v1::range_options>& encrypt_options::internal::range_opts(encrypt_options& self) {
    return impl::with(self)._range_opts;
}

bsoncxx::v1::stdx::optional<v1::text_options>& encrypt_options::internal::text_opts(encrypt_options& self) {
    return impl::with(self)._text_opts;
}

} // namespace v1
} // namespace mongocxx
