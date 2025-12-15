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

#pragma once

#include <bsoncxx/v1/array/view.hpp>
#include <bsoncxx/v1/detail/type_traits.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/server_error.hpp>

#include <mongocxx/v1/exception.hh>

#include <cstdint>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>

#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/server_error_code.hpp>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/scoped_bson.hh>

namespace mongocxx {
namespace v_noabi {

inline std::error_code make_error_code(int code, int) {
    // Domain is ignored. We simply issue the code.
    return {code, server_error_category()};
}

inline std::error_code make_error_code(bson_error_t const& error) {
    return v_noabi::make_error_code(static_cast<int>(error.code), static_cast<int>(error.domain));
}

inline void set_bson_error_message(bson_error_t* error, char const* msg) {
    bson_strncpy(error->message, msg, BSON_ERROR_BUFFER_SIZE);
}

inline void make_bson_error(bson_error_t* error, operation_exception const& e) {
    // No way to get the domain back out of the exception, so zero out.
    error->code = static_cast<std::uint32_t>(e.code().value());
    error->domain = 0;
    set_bson_error_message(error, e.what());
}

inline void make_generic_bson_error(bson_error_t* error) {
    // CDRIVER-3524 Zero these out since we don't have them.
    error->code = 0;
    error->domain = 0;
    set_bson_error_message(error, "unknown error");
}

template <typename exception_type>
[[noreturn]] void throw_exception(bson_error_t const& error) {
    throw exception_type{v_noabi::make_error_code(error), error.message};
}

template <typename exception_type>
[[noreturn]] void throw_exception(bsoncxx::v_noabi::document::value raw_server_error, bson_error_t const& error) {
    throw exception_type{v_noabi::make_error_code(error), std::move(raw_server_error), error.message};
}

inline std::string strip_ec_msg(std::string what, std::error_code code) {
    auto const what_view = bsoncxx::v1::stdx::string_view{what};
    auto const msg = code.message();
    auto const pos = what.find(msg);
    auto const delim = bsoncxx::v1::stdx::string_view(": ");

    if (pos != what_view.npos) {
        // "abc: msg: def" -> "abc: def"
        if (what_view.find(delim, pos) == pos) {
            what.erase(pos, msg.size() + delim.size());
        }

        // "abc: msg" -> "abc"
        else if (pos >= delim.size() && what_view.rfind(delim, pos) == pos - delim.size()) {
            what.erase(pos - delim.size(), msg.size() + delim.size());
        }
    }

    return what;
}

template <
    typename exception_type,
    bsoncxx::detail::enable_if_t<std::is_base_of<operation_exception, exception_type>::value>* = nullptr>
[[noreturn]] void throw_exception(v1::exception const& ex) {
    using bsoncxx::v_noabi::from_v1;

    // `v1::server_error_category()` -> `v_noabi::server_error_category()`.
    auto const code = ex.code() == v1::source_errc::server
                          ? std::error_code{ex.code().value(), v_noabi::server_error_category()}
                          : ex.code();

    // Server-side error.
    if (auto const ptr = dynamic_cast<v1::server_error const*>(&ex)) {
        throw exception_type{
            code,
            bsoncxx::v_noabi::document::value{from_v1(ptr->raw())},
            strip_ec_msg(ptr->what(), ptr->code()).c_str()};
    }

    // Propagate the original mongoc reply document as the "raw server error" document.
    {
        auto const& reply = v1::exception::internal::get_reply(ex);

        if (!reply.empty()) {
            throw exception_type{code, from_v1(reply), strip_ec_msg(ex.what(), ex.code()).c_str()};
        }
    }

    // No "raw server error" document is required.
    throw exception_type{code, strip_ec_msg(ex.what(), ex.code())};
}

template <
    typename exception_type,
    bsoncxx::detail::enable_if_t<!std::is_base_of<operation_exception, exception_type>::value>* = nullptr>
[[noreturn]] void throw_exception(v1::exception const& ex) {
    // `v1::server_error_category()` -> `v_noabi::server_error_category()`.
    auto const code = ex.code() == v1::source_errc::server
                          ? std::error_code{ex.code().value(), v_noabi::server_error_category()}
                          : ex.code();

    // No "raw server error" document is required.
    throw exception_type{code, strip_ec_msg(ex.what(), ex.code())};
}

} // namespace v_noabi
} // namespace mongocxx
