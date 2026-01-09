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

#include <mongocxx/v1/cursor.hh>

//

#include <bsoncxx/v1/document/view.hpp>

#include <mongocxx/v1/detail/macros.hpp>

#include <mongocxx/v1/exception.hh>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

namespace {

enum class state {
    has_doc,      // The previous advancement returned a document.
    can_get_more, // No document is currently available, but can get more (end).
    is_dead,      // Cannot obtain any more documents (end).
};

} // namespace

class cursor::impl {
   public:
    mongoc_cursor_t* _cursor;
    bsoncxx::v1::document::view _doc; // The result document.
    state _state = state::can_get_more;
    type _type;

    ~impl() {
        libmongoc::cursor_destroy(_cursor);
    }

    impl(impl&&) = delete;
    impl& operator=(impl&&) = delete;
    impl(impl const&) = delete;
    impl& operator=(impl const&) = delete;

    /* explicit(false) */ impl(mongoc_cursor_t* cursor, type type) : _cursor{cursor}, _type{type} {}

    static impl const& with(cursor const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(cursor const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(cursor& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(cursor* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

cursor::~cursor() {
    delete impl::with(_impl);
}

cursor::cursor(cursor&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

cursor& cursor::operator=(cursor&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

cursor::iterator cursor::begin() {
    switch (impl::with(this)->_state) {
        case state::has_doc: {
            // Do not advance on consecutive calls to `.begin()`.
            return iterator::internal::make(this);
        }

        case state::can_get_more: {
            auto iter = iterator::internal::make(this);
            ++iter; // Obtain the first available result document.
            return iter;
        }

        default:
        case state::is_dead: {
            return this->end();
        }
    }
}

cursor::cursor(void* impl) : _impl{impl} {}

cursor cursor::internal::make(mongoc_cursor_t* cursor, type type) {
    return {new impl{cursor, type}};
}

mongoc_cursor_t const* cursor::internal::as_mongoc(cursor const& self) {
    return impl::with(self)._cursor;
}

mongoc_cursor_t* cursor::internal::as_mongoc(cursor& self) {
    return impl::with(self)._cursor;
}

bsoncxx::v1::document::view cursor::internal::doc(cursor const& self) {
    return impl::with(self)._doc;
}

bool cursor::internal::has_doc(cursor const& self) {
    return impl::with(self)._state == state::has_doc;
}

bool cursor::internal::can_get_more(cursor const& self) {
    return impl::with(self)._state == state::can_get_more;
}

bool cursor::internal::is_dead(cursor const& self) {
    return impl::with(self)._state == state::is_dead;
}

void cursor::internal::advance_iterator(cursor& self) {
    auto const _cursor = impl::with(self)._cursor;
    auto& _doc = impl::with(self)._doc;
    auto& _state = impl::with(self)._state;

    scoped_bson_view doc;
    bson_error_t error = {};

    if (libmongoc::cursor_next(_cursor, doc.out_ptr())) {
        // Result document is available.
        _state = state::has_doc;
        _doc = doc.view();
    } else if (libmongoc::cursor_error_document(_cursor, &error, doc.out_ptr())) {
        // An irrecoverable error occurred.
        _state = state::is_dead;
        _doc = {};
        throw_exception(error, doc.view());
    } else {
        switch (impl::with(self)._type) {
            case type::k_non_tailable: {
                // Cursor is dead.
                _state = state::is_dead;
                _doc = {};
            } break;

            case type::k_tailable:
            case type::k_tailable_await: {
                // Cursor is resumable.
                _state = state::can_get_more;
                _doc = {};
            } break;

            default:
                MONGOCXX_PRIVATE_UNREACHABLE;
        }
    }
}

cursor::iterator::~iterator() = default;
cursor::iterator::iterator(iterator&& other) noexcept = default;
cursor::iterator& cursor::iterator::operator=(iterator&& other) noexcept = default;
cursor::iterator::iterator(iterator const& other) = default;
cursor::iterator& cursor::iterator::operator=(iterator const& other) = default;

cursor::iterator::iterator() : _impl{nullptr} {}

cursor::iterator::value_type cursor::iterator::operator*() const {
    return _impl ? cursor::impl::with(static_cast<cursor*>(_impl))->_doc : bsoncxx::v1::document::view{};
}

cursor::iterator::pointer cursor::iterator::operator->() const {
    return &cursor::impl::with(static_cast<cursor*>(_impl))->_doc;
}

cursor::iterator& cursor::iterator::operator++() {
    if (_impl) {
        cursor::internal::advance_iterator(*static_cast<cursor*>(_impl));
    }
    return *this;
}

bool operator==(cursor::iterator const& lhs, cursor::iterator const& rhs) {
    auto const _lhs = static_cast<cursor const*>(lhs._impl);
    auto const _rhs = static_cast<cursor const*>(rhs._impl);

    // Both are end iterators (null) or both have the same underlying cursor (not null).
    if (_lhs == _rhs) {
        return true;
    }

    // One is an end iterator and the other is an inactive iterator.
    if (!_lhs != !_rhs) {
        auto const& _cursor = _lhs ? *_lhs : *_rhs;
        return !cursor::internal::has_doc(_cursor);
    }

    // Different underlying cursors and neither are end iterators.
    return false;
}

cursor::iterator::iterator(void* impl) : _impl{impl} {}

cursor::iterator cursor::iterator::internal::make(cursor* self) {
    return {self};
}

cursor const* cursor::iterator::internal::get_cursor(iterator const& self) {
    return static_cast<cursor const*>(self._impl);
}

} // namespace v1
} // namespace mongocxx
