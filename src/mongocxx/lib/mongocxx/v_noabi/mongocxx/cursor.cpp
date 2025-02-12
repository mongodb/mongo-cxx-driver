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

#include <mongocxx/cursor.hpp>
#include <mongocxx/exception/query_exception.hpp>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/make_unique.hh>

#include <mongocxx/private/cursor.hh>
#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/mongoc_error.hh>

namespace mongocxx {
namespace v_noabi {

cursor::cursor(void* cursor_ptr, bsoncxx::v_noabi::stdx::optional<cursor::type> cursor_type)
    : _impl(bsoncxx::make_unique<impl>(static_cast<mongoc_cursor_t*>(cursor_ptr), cursor_type)) {}

cursor::cursor(cursor&&) noexcept = default;
cursor& cursor::operator=(cursor&&) noexcept = default;

cursor::~cursor() = default;

void cursor::iterator::operator++(int) {
    operator++();
}

cursor::iterator& cursor::iterator::operator++() {
    bson_t const* out;
    bson_t const* error_document;
    bson_error_t error;

    if (libmongoc::cursor_next(_cursor->_impl->cursor_t, &out)) {
        _cursor->_impl->doc = bsoncxx::v_noabi::document::view{bson_get_data(out), out->len};
    } else if (libmongoc::cursor_error_document(_cursor->_impl->cursor_t, &error, &error_document)) {
        _cursor->_impl->mark_dead();
        if (error_document) {
            bsoncxx::v_noabi::document::value error_doc{
                bsoncxx::v_noabi::document::view{bson_get_data(error_document), error_document->len}};
            throw_exception<query_exception>(error_doc, error);
        } else {
            throw_exception<query_exception>(error);
        }
    } else {
        _cursor->_impl->mark_nothing_left();
    }
    return *this;
}

cursor::iterator cursor::begin() {
    if (_impl->is_dead()) {
        return end();
    }
    return iterator(this);
}

cursor::iterator cursor::end() {
    return iterator(nullptr);
}

cursor::iterator::iterator(cursor* cursor) : _cursor(cursor) {
    if (_cursor == nullptr || _cursor->_impl->has_started()) {
        return;
    }

    _cursor->_impl->mark_started();
    operator++();
}

//
// An iterator is exhausted if it is the end-iterator (_cursor == nullptr)
// or if the underlying _cursor is marked exhausted.
//
bool cursor::iterator::is_exhausted() const {
    return !_cursor || _cursor->_impl->is_exhausted();
}

bsoncxx::v_noabi::document::view const& cursor::iterator::operator*() const {
    return _cursor->_impl->doc;
}

bsoncxx::v_noabi::document::view const* cursor::iterator::operator->() const {
    return &_cursor->_impl->doc;
}

//
// Iterators are equal if they point to the same underlying _cursor or if they
// both are "at the end".  We check for exhaustion first because the most
// common check is `iter != cursor.end()`.
//
bool operator==(cursor::iterator const& lhs, cursor::iterator const& rhs) {
    return ((rhs.is_exhausted() && lhs.is_exhausted()) || (lhs._cursor == rhs._cursor));
}

bool operator!=(cursor::iterator const& lhs, cursor::iterator const& rhs) {
    return !(lhs == rhs);
}

} // namespace v_noabi
} // namespace mongocxx
