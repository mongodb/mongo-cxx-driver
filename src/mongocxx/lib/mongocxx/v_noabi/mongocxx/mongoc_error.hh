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

#include <mongocxx/v1/server_error.hpp>

#include <mongocxx/v1/exception.hh>

#include <cstdint>
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

template <
    typename exception_type,
    bsoncxx::detail::enable_if_t<std::is_base_of<operation_exception, exception_type>::value>* = nullptr>
[[noreturn]] void throw_exception(v1::exception const& ex) {
    using bsoncxx::v_noabi::from_v1;

    // Server-side error.
    if (auto const ptr = dynamic_cast<v1::server_error const*>(&ex)) {
        throw exception_type{
            std::error_code{ptr->code().value(), v_noabi::server_error_category()},
            bsoncxx::v_noabi::document::value{from_v1(ptr->raw())},
            ptr->what()};
    }

    // Client-side error with array fields that must be throw as `.raw_server_error()` fields.
    {
        scoped_bson doc;

        auto const append_array_field = [&](char const* name, bsoncxx::v1::array::view field) {
            if (!field.empty()) {
                doc += scoped_bson{BCON_NEW(name, BCON_ARRAY(scoped_bson_view{field}.bson()))};
            }
        };

        append_array_field("errorLabels", v1::exception::internal::get_error_labels(ex));
        append_array_field("writeConcernErrors", v1::exception::internal::get_write_concern_errors(ex));
        append_array_field("writeErrors", v1::exception::internal::get_write_errors(ex));
        append_array_field("errorReplies", v1::exception::internal::get_error_replies(ex));

        if (!doc.view().empty()) {
            throw exception_type{ex.code(), from_v1(std::move(doc).value()), ex.what()};
        }
    }

    // Client-side error.
    throw exception_type{ex.code(), ex.what()};
}

template <
    typename exception_type,
    bsoncxx::detail::enable_if_t<!std::is_base_of<operation_exception, exception_type>::value>* = nullptr>
[[noreturn]] void throw_exception(v1::exception const& ex) {
    // Server-side error.
    if (ex.code() == v1::source_errc::server) {
        throw exception_type{std::error_code{ex.code().value(), v_noabi::server_error_category()}, ex.what()};
    }

    // Client-side error.
    throw exception_type{ex.code(), ex.what()};
}

} // namespace v_noabi
} // namespace mongocxx
