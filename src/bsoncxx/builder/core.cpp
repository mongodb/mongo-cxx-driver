// Copyright 2014 MongoDB Inc.
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

#include <bsoncxx/builder/core.hpp>

#include <cstring>

#include <bson.h>

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/private/itoa.hpp>
#include <bsoncxx/private/stack.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>

#include <bsoncxx/config/private/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {

namespace {

void bson_free_deleter(std::uint8_t* ptr) {
    bson_free(ptr);
}

}  // namespace

class core::impl {
   public:
    impl(bool is_array) : _depth(0), _root_is_array(is_array), _n(0), _has_user_key(false) {
        bson_init(&_root);
    }

    ~impl() {
        while (!_stack.empty()) {
            _stack.pop_back();
        }

        bson_destroy(&_root);
    }

    void reinit() {
        while (!_stack.empty()) {
            _stack.pop_back();
        }

        bson_reinit(&_root);

        _depth = 0;

        _n = 0;

        _has_user_key = false;
    }

    bsoncxx::document::value steal_document() {
        if (_root_is_array) {
            throw bsoncxx::exception{error_code::k_cannot_perform_document_operation_on_array};
        }

        uint32_t buf_len;
        uint8_t* buf_ptr = bson_destroy_with_steal(&_root, true, &buf_len);
        bson_init(&_root);

        return bsoncxx::document::value{buf_ptr, buf_len, bson_free_deleter};
    }

    bsoncxx::array::value steal_array() {
        if (!_root_is_array) {
            throw bsoncxx::exception{error_code::k_cannot_perform_array_operation_on_document};
        }

        uint32_t buf_len;
        uint8_t* buf_ptr = bson_destroy_with_steal(&_root, true, &buf_len);
        bson_init(&_root);

        return bsoncxx::array::value{buf_ptr, buf_len, bson_free_deleter};
    }

    bson_t* back() {
        if (_stack.empty()) {
            return &_root;
        } else {
            return &_stack.back().bson;
        }
    }

    void push_back_document(const char* key, std::size_t len) {
        _depth++;
        _stack.emplace_back(back(), key, len, false);
    }

    void push_back_array(const char* key, std::size_t len) {
        _depth++;
        _stack.emplace_back(back(), key, len, true);
    }

    void pop_back() {
        _depth--;
        _stack.pop_back();
    }

    stdx::string_view next_key() {
        if (is_array()) {
            _itoa_key = _stack.empty() ? _n++ : _stack.back().n++;
            _user_key_view = stdx::string_view{_itoa_key.c_str(), _itoa_key.length()};
        } else if (!_has_user_key) {
            throw bsoncxx::exception{error_code::k_need_key};
        }

        _has_user_key = false;

        return _user_key_view;
    }

    void push_key(stdx::string_view str) {
        _user_key_view = std::move(str);
        _has_user_key = true;
    }

    void push_key(std::string str) {
        _user_key_owned = std::move(str);
        _user_key_view = _user_key_owned;
        _has_user_key = true;
    }

    bson_t* root() {
        return &_root;
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
        frame(bson_t* parent, const char* key, std::size_t len, bool is_array)
            : n(0), is_array(is_array), parent(parent) {
            if (is_array) {
                bson_append_array_begin(parent, key, len, &bson);
            } else {
                bson_append_document_begin(parent, key, len, &bson);
            }
        }

        ~frame() {
            if (is_array) {
                bson_append_array_end(parent, &bson);
            } else {
                bson_append_document_end(parent, &bson);
            }
        }

        std::size_t n;
        bool is_array;
        bson_t bson;
        bson_t* parent;
    };

    stack<frame, 4> _stack;

    std::size_t _depth;

    bool _root_is_array;
    std::size_t _n;
    bson_t _root;

    itoa _itoa_key;

    stdx::string_view _user_key_view;
    std::string _user_key_owned;

    bool _has_user_key;
};

core::core(bool is_array) : _impl(new impl(is_array)) {
}
core::core(core&&) noexcept = default;
core& core::operator=(core&&) noexcept = default;
core::~core() = default;

void core::key_view(stdx::string_view key) {
    if (_impl->is_array()) {
        throw bsoncxx::exception{error_code::k_cannot_append_key_in_sub_array};
    }
    _impl->push_key(std::move(key));
}

void core::key_owned(std::string key) {
    if (_impl->is_array()) {
        throw bsoncxx::exception{error_code::k_cannot_append_key_in_sub_array};
    }
    _impl->push_key(std::move(key));
}

void core::append(const types::b_double& value) {
    stdx::string_view key = _impl->next_key();

    bson_append_double(_impl->back(), key.data(), key.length(), value.value);
}

void core::append(const types::b_utf8& value) {
    stdx::string_view key = _impl->next_key();

    bson_append_utf8(_impl->back(), key.data(), key.length(), value.value.data(),
                     value.value.length());
}

void core::append(const types::b_document& value) {
    stdx::string_view key = _impl->next_key();
    bson_t bson;
    bson_init_static(&bson, value.value.data(), value.value.length());

    bson_append_document(_impl->back(), key.data(), key.length(), &bson);
}

void core::append(const types::b_array& value) {
    stdx::string_view key = _impl->next_key();
    bson_t bson;
    bson_init_static(&bson, value.value.data(), value.value.length());

    bson_append_array(_impl->back(), key.data(), key.length(), &bson);
}

void core::append(const types::b_binary& value) {
    stdx::string_view key = _impl->next_key();

    bson_append_binary(_impl->back(), key.data(), key.length(),
                       static_cast<bson_subtype_t>(value.sub_type), value.bytes, value.size);
}

void core::append(const types::b_undefined&) {
    stdx::string_view key = _impl->next_key();

    bson_append_undefined(_impl->back(), key.data(), key.length());
}

void core::append(const types::b_oid& value) {
    stdx::string_view key = _impl->next_key();
    bson_oid_t oid;
    std::memcpy(&oid.bytes, value.value.bytes(), sizeof(oid.bytes));

    bson_append_oid(_impl->back(), key.data(), key.length(), &oid);
}

void core::append(const types::b_bool& value) {
    stdx::string_view key = _impl->next_key();

    bson_append_bool(_impl->back(), key.data(), key.length(), value.value);
}

void core::append(const types::b_date& value) {
    stdx::string_view key = _impl->next_key();

    bson_append_date_time(_impl->back(), key.data(), key.length(), value.to_int64());
}

void core::append(const types::b_null&) {
    stdx::string_view key = _impl->next_key();

    bson_append_null(_impl->back(), key.data(), key.length());
}

void core::append(const types::b_regex& value) {
    stdx::string_view key = _impl->next_key();

    bson_append_regex(_impl->back(), key.data(), key.length(), value.regex.to_string().data(),
                      value.options.to_string().data());
}

void core::append(const types::b_dbpointer& value) {
    stdx::string_view key = _impl->next_key();

    bson_oid_t oid;
    std::memcpy(&oid.bytes, value.value.bytes(), sizeof(oid.bytes));

    bson_append_dbpointer(_impl->back(), key.data(), key.length(),
                          value.collection.to_string().data(), &oid);
}

void core::append(const types::b_code& value) {
    stdx::string_view key = _impl->next_key();

    bson_append_code(_impl->back(), key.data(), key.length(), value.code.to_string().data());
}

void core::append(const types::b_symbol& value) {
    stdx::string_view key = _impl->next_key();

    bson_append_symbol(_impl->back(), key.data(), key.length(), value.symbol.data(),
                       value.symbol.length());
}

void core::append(const types::b_codewscope& value) {
    stdx::string_view key = _impl->next_key();

    bson_t bson;
    bson_init_static(&bson, value.scope.data(), value.scope.length());

    bson_append_code_with_scope(_impl->back(), key.data(), key.length(),
                                value.code.to_string().data(), &bson);
}

void core::append(const types::b_int32& value) {
    stdx::string_view key = _impl->next_key();

    bson_append_int32(_impl->back(), key.data(), key.length(), value.value);
}

void core::append(const types::b_timestamp& value) {
    stdx::string_view key = _impl->next_key();

    bson_append_timestamp(_impl->back(), key.data(), key.length(), value.increment,
                          value.timestamp);
}

void core::append(const types::b_int64& value) {
    stdx::string_view key = _impl->next_key();

    bson_append_int64(_impl->back(), key.data(), key.length(), value.value);
}

void core::append(const types::b_minkey&) {
    stdx::string_view key = _impl->next_key();

    bson_append_minkey(_impl->back(), key.data(), key.length());
}

void core::append(const types::b_maxkey&) {
    stdx::string_view key = _impl->next_key();

    bson_append_maxkey(_impl->back(), key.data(), key.length());
}

void core::append(std::string str) {
    append(types::b_utf8{std::move(str)});
}

void core::append(stdx::string_view str) {
    append(types::b_utf8{std::move(str)});
}

void core::append(double value) {
    append(types::b_double{value});
}

void core::append(std::int32_t value) {
    append(types::b_int32{value});
}

void core::append(const oid& value) {
    append(types::b_oid{value});
}

void core::append(const document::view view) {
    append(types::b_document{view});
}

void core::append(const array::view view) {
    append(types::b_array{view});
}

void core::append(std::int64_t value) {
    append(types::b_int64{value});
}

void core::append(bool value) {
    append(types::b_bool{value});
}

void core::open_document() {
    stdx::string_view key = _impl->next_key();

    _impl->push_back_document(key.data(), key.length());
}

void core::open_array() {
    stdx::string_view key = _impl->next_key();

    _impl->push_back_array(key.data(), key.length());
}

void core::concatenate(const bsoncxx::document::view& view) {
    bson_t other;
    bson_init_static(&other, view.data(), view.length());

    if (_impl->is_array()) {
        bson_iter_t iter;
        bson_iter_init(&iter, &other);

        while (bson_iter_next(&iter)) {
            stdx::string_view key = _impl->next_key();

            bson_append_iter(_impl->back(), key.data(), key.length(), &iter);
        }

    } else {
        bson_concat(_impl->back(), &other);
    }
}

void core::append(const bsoncxx::types::value& value) {
    switch (static_cast<int>(value.type())) {
#define BSONCXX_ENUM(type, val)     \
    case val:                       \
        append(value.get_##type()); \
        break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
    }
}

void core::close_document() {
    if (_impl->is_array()) {
        throw bsoncxx::exception{error_code::k_cannot_close_document_in_sub_array};
    }

    if (_impl->depth() == 0) {
        throw bsoncxx::exception{error_code::k_no_document_to_close};
    }

    _impl->pop_back();
}

void core::close_array() {
    if (!_impl->is_array()) {
        throw bsoncxx::exception{error_code::k_cannot_close_array_in_sub_document};
    }

    if (_impl->depth() == 0) {
        throw bsoncxx::exception{error_code::k_no_array_to_close};
    }

    _impl->pop_back();
}

bsoncxx::document::view core::view_document() const {
    if (!_impl->is_viewable()) {
        throw bsoncxx::exception{error_code::k_unmatched_key_in_builder};
    }

    return bsoncxx::document::view(bson_get_data(_impl->root()), _impl->root()->len);
}

bsoncxx::document::value core::extract_document() {
    if (!_impl->is_viewable()) {
        throw bsoncxx::exception{error_code::k_unmatched_key_in_builder};
    }

    return _impl->steal_document();
}

bsoncxx::array::view core::view_array() const {
    if (!_impl->is_viewable()) {
        throw bsoncxx::exception{error_code::k_unmatched_key_in_builder};
    }

    return bsoncxx::array::view(bson_get_data(_impl->root()), _impl->root()->len);
}

bsoncxx::array::value core::extract_array() {
    if (!_impl->is_viewable()) {
        throw bsoncxx::exception{error_code::k_unmatched_key_in_builder};
    }

    return _impl->steal_array();
}

void core::clear() {
    _impl->reinit();
}

}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
