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

//

#include <mongocxx/v1/exception.hpp>

#include <mongocxx/v1/cursor.hh>

#include <bsoncxx/document/view.hpp>

#include <mongocxx/exception/query_exception.hpp>

#include <mongocxx/mongoc_error.hh>

#include <bsoncxx/private/make_unique.hh>

namespace mongocxx {
namespace v_noabi {

cursor::cursor(cursor&&) noexcept = default;

cursor& cursor::operator=(cursor&&) noexcept = default;

cursor::~cursor() = default;

cursor::iterator cursor::begin() {
    if (v1::cursor::internal::is_dead(_cursor)) {
        return this->end();
    }
    return iterator{this};
}

cursor::iterator cursor::end() {
    return iterator(nullptr);
}

bsoncxx::v_noabi::document::view const& cursor::iterator::operator*() const {
    return _cursor->_doc;
}

bsoncxx::v_noabi::document::view const* cursor::iterator::operator->() const {
    return std::addressof(_cursor->_doc);
}

cursor::iterator& cursor::iterator::operator++() {
    if (_cursor) {
        try {
            v1::cursor::internal::advance_iterator(_cursor->_cursor);
        } catch (v1::exception const& ex) {
            _cursor->_doc = {};
            throw_exception<v_noabi::query_exception>(ex);
        }

        // Backward compatibility: support `*iter -> T const&`.
        _cursor->_doc = v1::cursor::internal::doc(_cursor->_cursor);
    }
    return *this;
}

cursor::iterator::iterator(cursor* cursor) : _cursor{cursor} {
    if (_cursor) {
        // Do not advance on consecutive calls to `.begin()`.
        if (v1::cursor::internal::has_doc(_cursor->_cursor)) {
            return;
        }

        try {
            // Advance to first event on begin() to keep operator*() state-machine-free.
            v1::cursor::internal::advance_iterator(_cursor->_cursor);
        } catch (v1::exception const& ex) {
            _cursor->_doc = {};
            throw_exception<v_noabi::query_exception>(ex);
        }

        // Backward compatibility: support `*iter -> T const&`.
        _cursor->_doc = v1::cursor::internal::doc(_cursor->_cursor);
    }
}

bool operator==(cursor::iterator const& lhs, cursor::iterator const& rhs) {
    return (rhs.is_exhausted() && lhs.is_exhausted()) || lhs._cursor == rhs._cursor;
}

bool cursor::iterator::is_exhausted() const {
    return !_cursor || !v1::cursor::internal::has_doc(_cursor->_cursor);
}

} // namespace v_noabi
} // namespace mongocxx
