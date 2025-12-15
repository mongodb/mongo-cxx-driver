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

#include <mongocxx/v1/exception.hh>

//

#include <bsoncxx/v1/array/value.hpp>
#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/id.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <mongocxx/v1/server_error.hh>

#include <cstring>
#include <memory>
#include <string>
#include <system_error>
#include <utility>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/immortal.hh>
#include <bsoncxx/private/make_unique.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {

namespace {

bool common_equivalence(v1::source_errc errc, int v, std::error_condition const& ec) noexcept {
    if (ec.category() == v1::source_error_category()) {
        return v != 0 ? errc == static_cast<v1::source_errc>(ec.value()) : false;
    }

    if (ec.category() == v1::type_error_category()) {
        return v != 0 ? v1::type_errc::runtime_error == static_cast<v1::type_errc>(ec.value()) : false;
    }

    return false;
}

std::error_category const& mongoc_error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "mongoc_error_code_t";
        }

        std::string message(int v) const noexcept override {
            return std::string(this->name()) + ':' + std::to_string(v);
        }

        bool equivalent(int v, std::error_condition const& ec) const noexcept override {
            return common_equivalence(v1::source_errc::mongoc, v, ec);
        }
    };

    static bsoncxx::immortal<type> const instance;

    return instance.value();
}

std::error_category const& mongocrypt_error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "mongocrypt_status_t";
        }

        std::string message(int v) const noexcept override {
            return std::string(this->name()) + ':' + std::to_string(v);
        }

        bool equivalent(int v, std::error_condition const& ec) const noexcept override {
            return common_equivalence(v1::source_errc::mongocrypt, v, ec);
        }
    };

    static bsoncxx::immortal<type> const instance;

    return instance.value();
}

std::error_category const& unknown_error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "unknown";
        }

        std::string message(int v) const noexcept override {
            return std::string(this->name()) + ':' + std::to_string(v);
        }

        bool equivalent(int v, std::error_condition const& ec) const noexcept override {
            if (ec.category() == v1::source_error_category()) {
                return false;
            }

            if (ec.category() == v1::type_error_category()) {
                return v != 0 ? v1::type_errc::runtime_error == static_cast<v1::type_errc>(ec.value()) : false;
            }

            return false;
        }
    };

    static bsoncxx::immortal<type> const instance;

    return instance.value();
}

} // namespace

std::error_category const& source_error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "mongocxx::v1::source_errc";
        }

        std::string message(int v) const noexcept override {
            using code = v1::source_errc;

            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::mongocxx:
                    return "mongocxx";
                case code::mongoc:
                    return "mongoc";
                case code::mongocrypt:
                    return "mongocrypt";
                case code::server:
                    return "server";
                default:
                    return std::string(this->name()) + ':' + std::to_string(v);
            }
        }
    };

    static bsoncxx::immortal<type> const instance;

    return instance.value();
}

std::error_category const& type_error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "mongocxx::v1::type_errc";
        }

        std::string message(int v) const noexcept override {
            using code = v1::type_errc;

            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::invalid_argument:
                    return "invalid argument";
                case code::runtime_error:
                    return "runtime error";
                default:
                    return std::string(this->name()) + ':' + std::to_string(v);
            }
        }
    };

    static bsoncxx::immortal<type> const instance;

    return instance.value();
}

class exception::impl {
   public:
    bsoncxx::v1::array::value _error_labels;
    bsoncxx::v1::document::value _reply; // For backward compatibility with v_noabi::operation_exception.
};

bool exception::has_error_label(bsoncxx::v1::stdx::string_view label) const {
    for (auto const& e : _impl->_error_labels) {
        if (e.type_view() == bsoncxx::v1::types::b_string{label}) {
            return true;
        }
    }
    return false;
}

exception::exception(std::error_code ec, char const* message, std::unique_ptr<impl> impl)
    : std::system_error{ec, message}, _impl{std::move(impl)} {}

exception::exception(std::error_code ec, std::unique_ptr<impl> impl) : std::system_error{ec}, _impl{std::move(impl)} {}

// Prevent vague linkage of the vtable and type_info object (-Wweak-vtables).
// - https://itanium-cxx-abi.github.io/cxx-abi/abi.html#vague-vtable
//   > The key function is the first non-pure virtual function that is not inline at the point of class definition.
// - https://lld.llvm.org/missingkeyfunction:
//   > It’s always advisable to ensure there is at least one eligible function that can serve as the key function.
// - https://gcc.gnu.org/onlinedocs/gcc/Vague-Linkage.html
//   > For polymorphic classes (classes with virtual functions), the ‘type_info’ object is written out along with the
//   vtable.
void exception::key_function() const {}

namespace {

v1::exception make_exception(bson_error_t const& error) {
    auto const code = static_cast<int>(error.code);
    auto const raw_category = static_cast<int>(error.reserved);
    auto const message = static_cast<char const*>(error.message);
    auto const has_message = message[0] != '\0';

    // Undocumented: see mongoc-error-private.h.
    // NOLINTNEXTLINE(cppcoreguidelines-use-enum-class): compile-time constants.
    enum : int {
        MONGOC_ERROR_CATEGORY_BSON = 1, // BSON_ERROR_CATEGORY
        MONGOC_ERROR_CATEGORY = 2,
        MONGOC_ERROR_CATEGORY_SERVER = 3,
        MONGOC_ERROR_CATEGORY_CRYPT = 4,
        MONGOC_ERROR_CATEGORY_SASL = 5,
    };

    // Undocumented: see mongoc-error-private.h.
    switch (raw_category) {
        // Unlikely. Convert to MONGOC_ERROR_BSON_INVALID (18).
        case MONGOC_ERROR_CATEGORY_BSON: {
            std::string what;
            what += "bson error code ";
            what += std::to_string(code);
            if (has_message) {
                what += ": ";
                what += message;
            }
            return v1::exception::internal::make(MONGOC_ERROR_BSON_INVALID, mongoc_error_category(), what.c_str());
        }

        // Throw as a mongoc error code.
        case MONGOC_ERROR_CATEGORY: {
            if (has_message) {
                return v1::exception::internal::make(code, mongoc_error_category(), message);
            } else {
                return v1::exception::internal::make(code, mongoc_error_category());
            }
        }

        // Unlikely. Throw as `v1::exception` but use the correct error category.
        case MONGOC_ERROR_CATEGORY_SERVER: {
            if (has_message) {
                return v1::exception::internal::make(code, v1::server_error::internal::category(), message);
            } else {
                return v1::exception::internal::make(code, v1::server_error::internal::category());
            }
        }

        // Throw as a libmongocrypt error code.
        case MONGOC_ERROR_CATEGORY_CRYPT: {
            if (has_message) {
                return v1::exception::internal::make(code, mongocrypt_error_category(), message);
            } else {
                return v1::exception::internal::make(code, mongocrypt_error_category());
            }
        }

        // Unlikely. Convert to MONGOC_ERROR_CLIENT_AUTHENTICATE (11).
        case MONGOC_ERROR_CATEGORY_SASL: {
            std::string what;
            what += "sasl error code ";
            what += std::to_string(code);
            if (has_message) {
                what += ": ";
                what += message;
            }
            return v1::exception::internal::make(
                MONGOC_ERROR_CLIENT_AUTHENTICATE, mongoc_error_category(), what.c_str());
        }

        // Unlikely. Throw as an unknown error code.
        default: {
            std::string what;
            what += "unknown error category ";
            what += std::to_string(raw_category);
            if (has_message) {
                what += ": ";
                what += message;
            }

            return v1::exception::internal::make(code, unknown_error_category(), what.c_str());
        }
    }
}

} // namespace

exception exception::internal::make(int code, std::error_category const& category, char const* message) {
    return {std::error_code{code, category}, message, bsoncxx::make_unique<impl>()};
}

exception exception::internal::make(int code, std::error_category const& category) {
    return {std::error_code{code, category}, bsoncxx::make_unique<impl>()};
}

namespace {

void set_array_field(
    bsoncxx::v1::stdx::string_view name,
    bsoncxx::v1::array::value& field,
    bsoncxx::v1::document::view v) {
    auto const e = v[name];

    if (e && e.type_id() == bsoncxx::v1::types::id::k_array) {
        field = e.get_array().value;
    } else {
        field = bsoncxx::v1::array::value{};
    }
}

} // namespace

void exception::internal::set_error_labels(exception& self, bsoncxx::v1::document::view v) {
    set_array_field("errorLabels", self._impl->_error_labels, v);
}

void exception::internal::set_reply(exception& self, bsoncxx::v1::document::value v) {
    self._impl->_reply = std::move(v);
}

bsoncxx::v1::array::view exception::internal::get_error_labels(exception const& self) {
    return self._impl->_error_labels;
}

bsoncxx::v1::document::value const& exception::internal::get_reply(exception const& self) {
    return self._impl->_reply;
}

void throw_exception(bson_error_t const& error) {
    throw make_exception(error);
}

void throw_exception(bson_error_t const& error, bsoncxx::v1::document::value doc) {
    // Server-side error.
    if (auto const code = doc["code"]) {
        if (code.type_id() == bsoncxx::v1::types::id::k_int32) {
            auto const ex = make_exception(error);
            throw v1::server_error::internal::make(int{code.get_int32().value}, ex.what(), std::move(doc), ex.code());
        }
    }

    // Client-side error.
    auto ex = make_exception(error);

    exception::internal::set_error_labels(ex, doc);
    exception::internal::set_reply(ex, std::move(doc));

    throw std::move(ex);
}

} // namespace v1
} // namespace mongocxx
