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

#include <mongocxx/v1/server_error.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <mongocxx/v1/exception.hh>

#include <memory>
#include <string>
#include <system_error>
#include <utility>

#include <bsoncxx/private/immortal.hh>
#include <bsoncxx/private/make_unique.hh>

namespace mongocxx {
namespace v1 {

namespace {

std::error_category const& server_error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "server";
        }

        std::string message(int v) const noexcept override {
            return "server error code " + std::to_string(v);
        }

        bool equivalent(int v, std::error_condition const& ec) const noexcept override {
            if (ec.category() == v1::source_error_category()) {
                using condition = v1::source_errc;

                auto const source = static_cast<condition>(ec.value());

                return v != 0 ? source == condition::server : false;
            }

            if (ec.category() == v1::type_error_category()) {
                using condition = v1::type_errc;

                auto const type = static_cast<condition>(ec.value());

                return v != 0 ? type == condition::runtime_error : false;
            }

            return false;
        }
    };

    static bsoncxx::immortal<type> const instance;

    return instance.value();
}

} // namespace

class server_error::impl {
   public:
    bsoncxx::v1::document::value _raw;
    std::error_code _client_code;

    explicit impl(bsoncxx::v1::document::value raw) : _raw{std::move(raw)} {}

    impl(bsoncxx::v1::document::value raw, std::error_code client_code)
        : _raw{std::move(raw)}, _client_code{client_code} {}
};

void server_error::key_function() const {}

std::error_code server_error::client_code() const {
    return _impl->_client_code;
}

bsoncxx::v1::document::view server_error::raw() const {
    return _impl->_raw;
}

server_error::server_error(int code, char const* message, std::unique_ptr<impl> impl)
    : v1::exception{v1::exception::internal::make(code, server_error_category(), message)}, _impl{std::move(impl)} {
    v1::exception::internal::set_error_labels(*this, _impl->_raw);
}

server_error server_error::internal::make(int code, char const* message, bsoncxx::v1::document::value raw) {
    return {code, message, bsoncxx::make_unique<impl>(std::move(raw))};
}

server_error server_error::internal::make(
    int code,
    char const* message,
    bsoncxx::v1::document::value raw,
    std::error_code client_code) {
    return {code, message, bsoncxx::make_unique<impl>(std::move(raw), client_code)};
}

std::error_category const& server_error::internal::category() {
    return v1::server_error_category();
}

} // namespace v1
} // namespace mongocxx
