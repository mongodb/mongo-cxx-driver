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

#include <mongocxx/v1/tls.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <string>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class tls::impl {
   public:
    bsoncxx::v1::stdx::optional<std::string> _pem_file;
    bsoncxx::v1::stdx::optional<std::string> _pem_password;
    bsoncxx::v1::stdx::optional<std::string> _ca_file;
    bsoncxx::v1::stdx::optional<std::string> _ca_dir;
    bsoncxx::v1::stdx::optional<std::string> _crl_file;
    bsoncxx::v1::stdx::optional<bool> _allow_invalid_certificates;

    static impl const& with(tls const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(tls const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(tls& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(tls* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

tls::~tls() {
    delete impl::with(this);
}

tls::tls(tls&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

tls& tls::operator=(tls&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }
    return *this;
}

tls::tls(tls const& other) : _impl{new impl{impl::with(other)}} {}

tls& tls::operator=(tls const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }
    return *this;
}

tls::tls() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

tls& tls::pem_file(std::string v) {
    impl::with(this)->_pem_file = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> tls::pem_file() const {
    return impl::with(this)->_pem_file;
}

tls& tls::pem_password(std::string v) {
    impl::with(this)->_pem_password = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> tls::pem_password() const {
    return impl::with(this)->_pem_password;
}

tls& tls::ca_file(std::string v) {
    impl::with(this)->_ca_file = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> tls::ca_file() const {
    return impl::with(this)->_ca_file;
}

tls& tls::ca_dir(std::string v) {
    impl::with(this)->_ca_dir = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> tls::ca_dir() const {
    return impl::with(this)->_ca_dir;
}

tls& tls::crl_file(std::string v) {
    impl::with(this)->_crl_file = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> tls::crl_file() const {
    return impl::with(this)->_crl_file;
}

tls& tls::allow_invalid_certificates(bool v) {
    impl::with(this)->_allow_invalid_certificates = v;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> tls::allow_invalid_certificates() const {
    return impl::with(this)->_allow_invalid_certificates;
}

bsoncxx::v1::stdx::optional<std::string> const& tls::internal::pem_file(tls const& self) {
    return impl::with(self)._pem_file;
}

bsoncxx::v1::stdx::optional<std::string> const& tls::internal::pem_password(tls const& self) {
    return impl::with(self)._pem_password;
}

bsoncxx::v1::stdx::optional<std::string> const& tls::internal::ca_file(tls const& self) {
    return impl::with(self)._ca_file;
}

bsoncxx::v1::stdx::optional<std::string> const& tls::internal::ca_dir(tls const& self) {
    return impl::with(self)._ca_dir;
}

bsoncxx::v1::stdx::optional<std::string> const& tls::internal::crl_file(tls const& self) {
    return impl::with(self)._crl_file;
}

bsoncxx::v1::stdx::optional<std::string>& tls::internal::pem_file(tls& self) {
    return impl::with(self)._pem_file;
}

bsoncxx::v1::stdx::optional<std::string>& tls::internal::pem_password(tls& self) {
    return impl::with(self)._pem_password;
}

bsoncxx::v1::stdx::optional<std::string>& tls::internal::ca_file(tls& self) {
    return impl::with(self)._ca_file;
}

bsoncxx::v1::stdx::optional<std::string>& tls::internal::ca_dir(tls& self) {
    return impl::with(self)._ca_dir;
}

bsoncxx::v1::stdx::optional<std::string>& tls::internal::crl_file(tls& self) {
    return impl::with(self)._crl_file;
}

mongoc_ssl_opt_t tls::internal::to_mongoc(v1::tls const& self) {
    mongoc_ssl_opt_t ret = {};

    if (auto const& opt = v1::tls::internal::pem_file(self)) {
        ret.pem_file = opt->c_str();
    }

    if (auto const& opt = v1::tls::internal::pem_password(self)) {
        ret.pem_pwd = opt->c_str();
    }

    if (auto const& opt = v1::tls::internal::ca_file(self)) {
        ret.ca_file = opt->c_str();
    }

    if (auto const& opt = v1::tls::internal::ca_dir(self)) {
        ret.ca_dir = opt->c_str();
    }

    if (auto const& opt = v1::tls::internal::crl_file(self)) {
        ret.crl_file = opt->c_str();
    }

    ret.weak_cert_validation = self.allow_invalid_certificates().value_or(false);

    return ret;
}

} // namespace v1
} // namespace mongocxx
