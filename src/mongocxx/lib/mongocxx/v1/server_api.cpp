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

#include <mongocxx/v1/server_api.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/detail/macros.hpp>

#include <mongocxx/v1/exception.hh>

#include <memory>
#include <string>
#include <system_error>

#include <bsoncxx/private/immortal.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

using code = server_api::errc;

namespace {

static_assert(
    static_cast<int>(server_api::version::k_version_1) ==
        static_cast<int>(mongoc_server_api_version_t::MONGOC_SERVER_API_V1),
    "");

server_api::version from_mongoc(mongoc_server_api_version_t v) {
    return static_cast<server_api::version>(v);
}

} // namespace

class server_api::impl {
   public:
    version _version;
    bsoncxx::v1::stdx::optional<bool> _strict;
    bsoncxx::v1::stdx::optional<bool> _deprecation_errors;

    explicit impl(version v) : _version{v} {}

    static impl const& with(server_api const& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl const* with(server_api const* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl& with(server_api& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(server_api* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

server_api::~server_api() {
    delete impl::with(this);
}

server_api::server_api(server_api&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

server_api& server_api::operator=(server_api&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

server_api::server_api(server_api const& other) : _impl{new impl{impl::with(other)}} {}

server_api& server_api::operator=(server_api const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

server_api::server_api(version v) : _impl{new impl{v}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

std::string server_api::version_to_string(version v) {
    switch (v) {
        case version::k_version_1:
            return "1";
        default:
            throw v1::exception::internal::make(code::invalid_version);
    }
}

server_api::version server_api::version_from_string(bsoncxx::v1::stdx::string_view v) {
    mongoc_server_api_version_t ver = {};

    if (libmongoc::server_api_version_from_string(std::string{v}.c_str(), &ver)) {
        return from_mongoc(ver);
    }

    throw v1::exception::internal::make(code::invalid_version);
}

server_api& server_api::strict(bool strict) {
    impl::with(this)->_strict = strict;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> server_api::strict() const {
    return impl::with(this)->_strict;
}

server_api& server_api::deprecation_errors(bool v) {
    impl::with(this)->_deprecation_errors = v;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> server_api::deprecation_errors() const {
    return impl::with(this)->_deprecation_errors;
}

server_api::version server_api::get_version() const {
    return impl::with(this)->_version;
}

std::error_category const& server_api::error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "mongocxx::v1::server_api";
        }

        std::string message(int v) const noexcept override {
            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::invalid_version:
                    return "invalid server API version";
                default:
                    return std::string(this->name()) + ':' + std::to_string(v);
            }
        }

        bool equivalent(int v, std::error_condition const& ec) const noexcept override {
            if (ec.category() == v1::source_error_category()) {
                using condition = v1::source_errc;

                auto const source = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::invalid_version:
                        return source == condition::mongocxx;

                    case code::zero:
                    default:
                        return false;
                }
            }

            if (ec.category() == v1::type_error_category()) {
                using condition = v1::type_errc;

                auto const type = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::invalid_version:
                        return type == condition::invalid_argument;

                    case code::zero:
                    default:
                        return false;
                }
            }

            return false;
        }
    };

    static bsoncxx::immortal<type> const instance;

    return instance.value();
}

std::unique_ptr<mongoc_server_api_t, server_api::internal::mongoc_server_api_deleter> server_api::internal::to_mongoc(
    v1::server_api const& api) {
    mongoc_server_api_version_t version = {};

    if (!libmongoc::server_api_version_from_string(
            v1::server_api::version_to_string(api.get_version()).c_str(), &version)) {
        // Invariant: enforced by `v1::server_api::errc::invalid_version`.
        MONGOCXX_PRIVATE_UNREACHABLE;
    }

    std::unique_ptr<mongoc_server_api_t, mongoc_server_api_deleter> ret{libmongoc::server_api_new(version)};

    auto const ptr = ret.get();

    if (auto const opt = api.strict()) {
        libmongoc::server_api_strict(ptr, *opt);
    }

    if (auto const opt = api.deprecation_errors()) {
        libmongoc::server_api_deprecation_errors(ptr, *opt);
    }

    return ret;
}

} // namespace v1
} // namespace mongocxx
