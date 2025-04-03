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

#include <cstring>

#include <bsoncxx/builder/core.hpp>
#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/itoa.hh>
#include <bsoncxx/private/make_unique.hh>
#include <bsoncxx/private/stack.hh>
#include <bsoncxx/private/suppress_deprecation_warnings.hh>

namespace bsoncxx {
namespace v_noabi {
namespace builder {

namespace {

void bson_free_deleter(std::uint8_t* ptr) {
    bson_free(ptr);
}

//
// Class providing RAII semantics for bson_t.
//
class managed_bson_t {
   public:
    managed_bson_t() {
        bson_init(&bson);
    }

    managed_bson_t(managed_bson_t&&) = delete;
    managed_bson_t& operator=(managed_bson_t&&) = delete;

    managed_bson_t(managed_bson_t const&) = delete;
    managed_bson_t& operator=(managed_bson_t const&) = delete;

    ~managed_bson_t() {
        bson_destroy(&bson);
    }

    bson_t* get() {
        return &bson;
    }

   private:
    bson_t bson;
};

} // namespace

class core::impl {
   public:
    impl(bool is_array) : _depth(0), _root_is_array(is_array), _n(0), _has_user_key(false) {}

    void reinit() {
        while (!_stack.empty()) {
            _stack.pop_back();
        }

        bson_reinit(_root.get());

        _depth = 0;

        _n = 0;

        _has_user_key = false;
    }

    // Throws bsoncxx::v_noabi::exception if the top-level BSON datum is an array.
    bsoncxx::v_noabi::document::value steal_document() {
        if (_root_is_array) {
            throw bsoncxx::v_noabi::exception{error_code::k_cannot_perform_document_operation_on_array};
        }

        uint32_t buf_len;
        uint8_t* buf_ptr = bson_destroy_with_steal(_root.get(), true, &buf_len);
        bson_init(_root.get());

        return bsoncxx::v_noabi::document::value{buf_ptr, buf_len, bson_free_deleter};
    }

    // Throws bsoncxx::v_noabi::exception if the top-level BSON datum is a document.
    bsoncxx::v_noabi::array::value steal_array() {
        if (!_root_is_array) {
            throw bsoncxx::v_noabi::exception{error_code::k_cannot_perform_array_operation_on_document};
        }

        uint32_t buf_len;
        uint8_t* buf_ptr = bson_destroy_with_steal(_root.get(), true, &buf_len);
        bson_init(_root.get());

        return bsoncxx::v_noabi::array::value{buf_ptr, buf_len, bson_free_deleter};
    }

    bson_t* back() {
        if (_stack.empty()) {
            return _root.get();
        } else {
            return &_stack.back().bson;
        }
    }

    void push_back_document(char const* key, std::int32_t len) {
        _depth++;
        _stack.emplace_back(back(), key, len, false);
    }

    void push_back_array(char const* key, std::int32_t len) {
        _depth++;
        _stack.emplace_back(back(), key, len, true);
    }

    void pop_back() {
        _depth--;
        _stack.pop_back();
    }

    // Throws bsoncxx::v_noabi::exception if the current BSON datum is a document that is waiting
    // for a key to be appended to start a new key/value pair.
    stdx::string_view next_key() {
        if (is_array()) {
            _itoa_key =
                _stack.empty() ? static_cast<std::uint32_t>(_n++) : static_cast<std::uint32_t>(_stack.back().n++);
            _user_key_view = stdx::string_view{_itoa_key.c_str(), _itoa_key.length()};
        } else if (!_has_user_key) {
            throw bsoncxx::v_noabi::exception{error_code::k_need_key};
        }

        _has_user_key = false;

        return _user_key_view;
    }

    void push_key(stdx::string_view str) {
        if (_has_user_key) {
            throw bsoncxx::v_noabi::exception{error_code::k_unmatched_key_in_builder};
        }

        _user_key_view = std::move(str);
        _has_user_key = true;
    }

    void push_key(std::string str) {
        if (_has_user_key) {
            throw bsoncxx::v_noabi::exception{error_code::k_unmatched_key_in_builder};
        }

        _user_key_owned = std::move(str);
        _user_key_view = _user_key_owned;
        _has_user_key = true;
    }

    // Throws bsoncxx::v_noabi::exception if the top-level BSON datum is an array.
    bson_t* root_document() {
        if (_root_is_array) {
            throw bsoncxx::v_noabi::exception{error_code::k_cannot_perform_document_operation_on_array};
        }

        return _root.get();
    }

    // Throws bsoncxx::v_noabi::exception if the top-level BSON datum is a document.
    bson_t* root_array() {
        if (!_root_is_array) {
            throw bsoncxx::v_noabi::exception{error_code::k_cannot_perform_array_operation_on_document};
        }

        return _root.get();
    }

    bool is_array() {
        return _stack.empty() ? _root_is_array : _stack.back().is_array;
    }

    bool is_viewable() {
        return _depth == 0 && !_has_user_key;
    }

    std::size_t depth() {
        return _depth;
    }

   private:
    struct frame {
        ~frame() = default;

        frame(frame&&) = delete;
        frame& operator=(frame&&) = delete;

        frame(frame const&) = delete;
        frame& operator=(frame const&) = delete;

        frame(bson_t* parent, char const* key, std::int32_t len, bool is_array)
            : n(0), is_array(is_array), parent(parent) {
            if (is_array) {
                if (!bson_append_array_begin(parent, key, len, &bson)) {
                    throw bsoncxx::v_noabi::exception{error_code::k_cannot_begin_appending_array};
                }
            } else {
                if (!bson_append_document_begin(parent, key, len, &bson)) {
                    throw bsoncxx::v_noabi::exception{error_code::k_cannot_begin_appending_document};
                }
            }
        }

        void close() {
            if (is_array) {
                if (!bson_append_array_end(parent, &bson)) {
                    throw bsoncxx::v_noabi::exception{error_code::k_cannot_end_appending_array};
                }
            } else {
                if (!bson_append_document_end(parent, &bson)) {
                    throw bsoncxx::v_noabi::exception{error_code::k_cannot_end_appending_document};
                }
            }
        }

        std::size_t n;
        bool is_array;
        bson_t bson;
        bson_t* parent;
    };

    std::size_t _depth;

    bool _root_is_array;
    std::size_t _n;
    managed_bson_t _root;

    // The bottom frame of _stack has _root as its parent.
    stack<frame, 4> _stack;

    itoa _itoa_key;

    stdx::string_view _user_key_view;
    std::string _user_key_owned;

    bool _has_user_key;
};

core::core(bool is_array) {
    _impl = make_unique<impl>(is_array);
}

core::core(core&&) noexcept = default;
core& core::operator=(core&&) noexcept = default;
core::~core() = default;

core& core::key_view(stdx::string_view key) {
    if (_impl->is_array()) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_key_in_sub_array};
    }
    _impl->push_key(std::move(key));

    return *this;
}

core& core::key_owned(std::string key) {
    if (_impl->is_array()) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_key_in_sub_array};
    }
    _impl->push_key(std::move(key));

    return *this;
}

core& core::append(types::b_double const& value) {
    stdx::string_view key = _impl->next_key();

    if (!bson_append_double(_impl->back(), key.data(), static_cast<std::int32_t>(key.length()), value.value)) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_double};
    }

    return *this;
}

core& core::append(types::b_string const& value) {
    stdx::string_view key = _impl->next_key();
    std::size_t value_length = value.value.length();

    if (value_length > std::size_t{INT32_MAX} || !bson_append_utf8(
                                                     _impl->back(),
                                                     key.data(),
                                                     static_cast<std::int32_t>(key.length()),
                                                     value.value.data(),
                                                     static_cast<std::int32_t>(value_length))) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_string};
    }

    return *this;
}

core& core::append(types::b_document const& value) {
    stdx::string_view key = _impl->next_key();
    bson_t bson;
    bson_init_static(&bson, value.value.data(), value.value.length());

    if (!bson_append_document(_impl->back(), key.data(), static_cast<std::int32_t>(key.length()), &bson)) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_document};
    }

    return *this;
}

core& core::append(types::b_array const& value) {
    stdx::string_view key = _impl->next_key();
    bson_t bson;
    bson_init_static(&bson, value.value.data(), value.value.length());

    if (!bson_append_array(_impl->back(), key.data(), static_cast<std::int32_t>(key.length()), &bson)) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_array};
    }

    return *this;
}

core& core::append(types::b_binary const& value) {
    stdx::string_view key = _impl->next_key();

    if (!bson_append_binary(
            _impl->back(),
            key.data(),
            static_cast<std::int32_t>(key.length()),
            static_cast<bson_subtype_t>(value.sub_type),
            value.bytes,
            value.size)) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_binary};
    }

    return *this;
}

uint8_t* core::append(binary_sub_type sub_type, uint32_t length) {
    stdx::string_view key = _impl->next_key();
    uint8_t* allocated_bytes;

    if (!bson_append_binary_uninit(
            _impl->back(),
            key.data(),
            static_cast<std::int32_t>(key.length()),
            static_cast<bson_subtype_t>(sub_type),
            &allocated_bytes,
            length)) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_binary};
    }

    return allocated_bytes;
}

core& core::append(types::b_undefined const&) {
    stdx::string_view key = _impl->next_key();

    if (!bson_append_undefined(_impl->back(), key.data(), static_cast<std::int32_t>(key.length()))) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_undefined};
    }

    return *this;
}

core& core::append(types::b_oid const& value) {
    stdx::string_view key = _impl->next_key();
    bson_oid_t oid;
    std::memcpy(&oid.bytes, value.value.bytes(), sizeof(oid.bytes));

    if (!bson_append_oid(_impl->back(), key.data(), static_cast<std::int32_t>(key.length()), &oid)) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_oid};
    }

    return *this;
}

core& core::append(types::b_bool const& value) {
    stdx::string_view key = _impl->next_key();

    if (!bson_append_bool(_impl->back(), key.data(), static_cast<std::int32_t>(key.length()), value.value)) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_bool};
    }

    return *this;
}

core& core::append(types::b_date const& value) {
    stdx::string_view key = _impl->next_key();

    if (!bson_append_date_time(_impl->back(), key.data(), static_cast<std::int32_t>(key.length()), value.to_int64())) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_date};
    }

    return *this;
}

core& core::append(types::b_null const&) {
    stdx::string_view key = _impl->next_key();

    if (!bson_append_null(_impl->back(), key.data(), static_cast<std::int32_t>(key.length()))) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_null};
    }

    return *this;
}

core& core::append(types::b_regex const& value) {
    stdx::string_view key = _impl->next_key();

    if (!bson_append_regex(
            _impl->back(),
            key.data(),
            static_cast<std::int32_t>(key.length()),
            string::to_string(value.regex).data(),
            string::to_string(value.options).data())) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_regex};
    }

    return *this;
}

core& core::append(types::b_dbpointer const& value) {
    stdx::string_view key = _impl->next_key();

    bson_oid_t oid;
    std::memcpy(&oid.bytes, value.value.bytes(), sizeof(oid.bytes));

    if (!bson_append_dbpointer(
            _impl->back(),
            key.data(),
            static_cast<std::int32_t>(key.length()),
            string::to_string(value.collection).data(),
            &oid)) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_dbpointer};
    }

    return *this;
}

core& core::append(types::b_code const& value) {
    stdx::string_view key = _impl->next_key();

    if (!bson_append_code(
            _impl->back(), key.data(), static_cast<std::int32_t>(key.length()), string::to_string(value.code).data())) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_code};
    }

    return *this;
}

core& core::append(types::b_symbol const& value) {
    stdx::string_view key = _impl->next_key();

    if (!bson_append_symbol(
            _impl->back(),
            key.data(),
            static_cast<std::int32_t>(key.length()),
            value.symbol.data(),
            static_cast<std::int32_t>(value.symbol.length()))) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_symbol};
    }

    return *this;
}

core& core::append(types::b_codewscope const& value) {
    stdx::string_view key = _impl->next_key();

    bson_t bson;
    bson_init_static(&bson, value.scope.data(), value.scope.length());

    if (!bson_append_code_with_scope(
            _impl->back(),
            key.data(),
            static_cast<std::int32_t>(key.length()),
            string::to_string(value.code).data(),
            &bson)) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_codewscope};
    }

    return *this;
}

core& core::append(types::b_int32 const& value) {
    stdx::string_view key = _impl->next_key();

    if (!bson_append_int32(_impl->back(), key.data(), static_cast<std::int32_t>(key.length()), value.value)) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_int32};
    }

    return *this;
}

core& core::append(types::b_timestamp const& value) {
    stdx::string_view key = _impl->next_key();

    if (!bson_append_timestamp(
            _impl->back(), key.data(), static_cast<std::int32_t>(key.length()), value.timestamp, value.increment)) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_timestamp};
    }

    return *this;
}

core& core::append(types::b_int64 const& value) {
    stdx::string_view key = _impl->next_key();

    if (!bson_append_int64(_impl->back(), key.data(), static_cast<std::int32_t>(key.length()), value.value)) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_int64};
    }

    return *this;
}

core& core::append(types::b_decimal128 const& value) {
    stdx::string_view key = _impl->next_key();
    bson_decimal128_t d128;
    d128.high = value.value.high();
    d128.low = value.value.low();

    if (!bson_append_decimal128(_impl->back(), key.data(), static_cast<std::int32_t>(key.length()), &d128)) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_decimal128};
    }

    return *this;
}

core& core::append(types::b_minkey const&) {
    stdx::string_view key = _impl->next_key();

    if (!bson_append_minkey(_impl->back(), key.data(), static_cast<std::int32_t>(key.length()))) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_minkey};
    }

    return *this;
}

core& core::append(types::b_maxkey const&) {
    stdx::string_view key = _impl->next_key();

    if (!bson_append_maxkey(_impl->back(), key.data(), static_cast<std::int32_t>(key.length()))) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_maxkey};
    }

    return *this;
}

core& core::append(std::string str) {
    append(types::b_string{std::move(str)});

    return *this;
}

core& core::append(stdx::string_view str) {
    append(types::b_string{std::move(str)});

    return *this;
}

core& core::append(double value) {
    append(types::b_double{value});

    return *this;
}

core& core::append(std::int32_t value) {
    append(types::b_int32{value});

    return *this;
}

core& core::append(oid const& value) {
    append(types::b_oid{value});

    return *this;
}

core& core::append(decimal128 value) {
    append(types::b_decimal128{value});

    return *this;
}

core& core::append(document::view const view) {
    append(types::b_document{view});

    return *this;
}

core& core::append(array::view const view) {
    append(types::b_array{view});

    return *this;
}

core& core::append(std::int64_t value) {
    append(types::b_int64{value});

    return *this;
}

core& core::append(bool value) {
    append(types::b_bool{value});

    return *this;
}

core& core::open_document() {
    stdx::string_view key = _impl->next_key();

    _impl->push_back_document(key.data(), static_cast<std::int32_t>(key.length()));

    return *this;
}

core& core::open_array() {
    stdx::string_view key = _impl->next_key();

    _impl->push_back_array(key.data(), static_cast<std::int32_t>(key.length()));

    return *this;
}

core& core::concatenate(bsoncxx::v_noabi::document::view const& view) {
    if (_impl->is_array()) {
        bson_iter_t iter;
        if (!bson_iter_init_from_data(&iter, view.data(), view.length())) {
            throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_document};
        }

        while (bson_iter_next(&iter)) {
            stdx::string_view key = _impl->next_key();

            if (!bson_append_iter(_impl->back(), key.data(), static_cast<std::int32_t>(key.length()), &iter)) {
                throw bsoncxx::v_noabi::exception{error_code::k_cannot_append_document};
            }
        }

    } else {
        bson_t other;
        bson_init_static(&other, view.data(), view.length());
        bson_concat(_impl->back(), &other);
    }

    return *this;
}

core& core::append(types::bson_value::view const& value) {
    switch (static_cast<int>(value.type())) {
#define BSONCXX_ENUM(type, val)     \
    case val:                       \
        append(value.get_##type()); \
        break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
    }

    return *this;
}

core& core::close_document() {
    if (_impl->is_array()) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_close_document_in_sub_array};
    }

    if (_impl->depth() == 0) {
        throw bsoncxx::v_noabi::exception{error_code::k_no_document_to_close};
    }

    _impl->pop_back();

    return *this;
}

core& core::close_array() {
    if (!_impl->is_array()) {
        throw bsoncxx::v_noabi::exception{error_code::k_cannot_close_array_in_sub_document};
    }

    if (_impl->depth() == 0) {
        throw bsoncxx::v_noabi::exception{error_code::k_no_array_to_close};
    }

    _impl->pop_back();

    return *this;
}

core& core::close_binary() {
    if (!_impl->is_viewable()) {
        throw bsoncxx::v_noabi::exception{error_code::k_unmatched_key_in_builder};
    }
    return *this;
}

bsoncxx::v_noabi::document::view core::view_document() const {
    if (!_impl->is_viewable()) {
        throw bsoncxx::v_noabi::exception{error_code::k_unmatched_key_in_builder};
    }

    return bsoncxx::v_noabi::document::view(bson_get_data(_impl->root_document()), _impl->root_document()->len);
}

bsoncxx::v_noabi::document::value core::extract_document() {
    if (!_impl->is_viewable()) {
        throw bsoncxx::v_noabi::exception{error_code::k_unmatched_key_in_builder};
    }

    return _impl->steal_document();
}

bsoncxx::v_noabi::array::view core::view_array() const {
    if (!_impl->is_viewable()) {
        throw bsoncxx::v_noabi::exception{error_code::k_unmatched_key_in_builder};
    }

    return bsoncxx::v_noabi::array::view(bson_get_data(_impl->root_array()), _impl->root_array()->len);
}

bsoncxx::v_noabi::array::value core::extract_array() {
    if (!_impl->is_viewable()) {
        throw bsoncxx::v_noabi::exception{error_code::k_unmatched_key_in_builder};
    }

    return _impl->steal_array();
}

void core::clear() {
    _impl->reinit();
}

} // namespace builder
} // namespace v_noabi
} // namespace bsoncxx
