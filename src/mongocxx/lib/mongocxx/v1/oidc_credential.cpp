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

#include <mongocxx/v1/oidc_credential.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <chrono>
#include <string>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class oidc_credential::impl {
   public:
    std::string _access_token;
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> _expires_in;

    static impl const& with(oidc_credential const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl& with(oidc_credential& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl const* with(oidc_credential const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl* with(oidc_credential* self) {
        return static_cast<impl*>(self->_impl);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory)

oidc_credential::~oidc_credential() {
    delete static_cast<impl*>(_impl);
}

oidc_credential::oidc_credential(oidc_credential&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

oidc_credential& oidc_credential::operator=(oidc_credential&& other) noexcept {
    if (this != &other) {
        delete static_cast<impl*>(exchange(_impl, exchange(other._impl, nullptr)));
    }
    return *this;
}

oidc_credential::oidc_credential(oidc_credential const& other) : _impl{new impl{impl::with(other)}} {}

oidc_credential& oidc_credential::operator=(oidc_credential const& other) {
    if (this != &other) {
        delete static_cast<impl*>(exchange(_impl, new impl{impl::with(other)}));
    }
    return *this;
}

oidc_credential::oidc_credential(std::string access_token)
    : _impl{new impl{std::move(access_token), bsoncxx::v1::stdx::nullopt}} {}

oidc_credential::oidc_credential(std::string access_token, std::chrono::milliseconds expires_in)
    : _impl{new impl{std::move(access_token), expires_in}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

bsoncxx::v1::stdx::string_view oidc_credential::access_token() const {
    return impl::with(this)->_access_token;
}

bsoncxx::v1::stdx::optional<std::chrono::milliseconds> oidc_credential::expires_in() const {
    return impl::with(this)->_expires_in;
}

mongoc_oidc_credential_t* oidc_credential::internal::to_mongoc(oidc_credential const& cred) {
    auto const& imp = impl::with(cred);
    if (imp._expires_in) {
        return libmongoc::oidc_credential_new_with_expires_in(imp._access_token.c_str(), imp._expires_in->count());
    }
    return libmongoc::oidc_credential_new(imp._access_token.c_str());
}

} // namespace v1
} // namespace mongocxx
