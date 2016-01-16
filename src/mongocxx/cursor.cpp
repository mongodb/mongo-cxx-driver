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

#include <mongocxx/cursor.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <tuple>

#include <bson.h>

#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/exception/private/error_category.hpp>
#include <mongocxx/exception/private/mongoc_error.hpp>
#include <mongocxx/exception/query_exception.hpp>
#include <mongocxx/private/cursor.hpp>
#include <mongocxx/private/libmongoc.hpp>

#include <mongocxx/config/private/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

cursor::cursor(void* cursor_ptr)
    : _impl(stdx::make_unique<impl>(static_cast<mongoc_cursor_t*>(cursor_ptr))) {
}

cursor::cursor(cursor&&) noexcept = default;
cursor& cursor::operator=(cursor&&) noexcept = default;

cursor::~cursor() = default;

void cursor::iterator::operator++(int) {
    operator++();
}

cursor::iterator& cursor::iterator::operator++() {
    const bson_t* out;
    bson_error_t error;
    if (libmongoc::cursor_next(_cursor->_impl->cursor_t, &out)) {
        _doc = bsoncxx::document::view(bson_get_data(out), out->len);
    } else if (libmongoc::cursor_error(_cursor->_impl->cursor_t, &error)) {
        throw_exception<query_exception>(error);
    } else {
        _cursor = nullptr;
    };
    return *this;
}

cursor::iterator cursor::begin() {
    // Maybe this should be an exception somewhere?
    if (!_impl->cursor_t) {
        return end();
    }
    return iterator(this);
}

cursor::iterator cursor::end() {
    return iterator(nullptr);
}

cursor::iterator::iterator(cursor* cursor) : _cursor(cursor) {
    if (cursor) operator++();
}

const bsoncxx::document::view& cursor::iterator::operator*() const {
    return _doc;
}

const bsoncxx::document::view* cursor::iterator::operator->() const {
    return &_doc;
}

bool operator==(const cursor::iterator& lhs, const cursor::iterator& rhs) {
    if (lhs._cursor == rhs._cursor) return true;
    return &lhs == &rhs;
}

bool operator!=(const cursor::iterator& lhs, const cursor::iterator& rhs) {
    return !(lhs == rhs);
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
