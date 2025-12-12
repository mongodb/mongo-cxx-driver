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

#include <mongocxx/change_stream.hpp>

//

#include <mongocxx/v1/detail/macros.hpp>
#include <mongocxx/v1/server_error.hpp>

#include <mongocxx/v1/change_stream.hh>

#include <type_traits>

#include <bsoncxx/document/view.hpp>

#include <mongocxx/exception/query_exception.hpp>

#include <mongocxx/mongoc_error.hh>

#include <bsoncxx/private/make_unique.hh>

namespace mongocxx {
namespace v_noabi {

// Requirements for concept Iterator:
// https://en.cppreference.com/w/cpp/named_req/Iterator
static_assert(std::is_copy_constructible<change_stream::iterator>::value, "");
static_assert(std::is_copy_assignable<change_stream::iterator>::value, "");
static_assert(std::is_destructible<change_stream::iterator>::value, "");

// Below basically assert that we have the traits on change_stream::iterator
// so they can't be accidentally removed.
static_assert(std::is_integral<change_stream::iterator::difference_type>::value, "");
static_assert(std::is_class<change_stream::iterator::value_type>::value, "");
static_assert(std::is_pointer<change_stream::iterator::pointer>::value, "");
static_assert(std::is_reference<change_stream::iterator::reference>::value, "");

change_stream::change_stream(change_stream&&) noexcept = default;

change_stream& change_stream::operator=(change_stream&&) noexcept = default;

change_stream::~change_stream() = default;

change_stream::iterator change_stream::begin() const {
    if (v1::change_stream::internal::is_dead(_stream)) {
        return this->end();
    }
    // Backward compatibility: `begin()` is not logically const.
    return iterator{const_cast<change_stream*>(this), false};
}

change_stream::iterator change_stream::end() const {
    // Backward compatibility: `begin()` is not logically const.
    return iterator{const_cast<change_stream*>(this), true};
}

change_stream::iterator::iterator() : iterator{nullptr, true} {}

bsoncxx::v_noabi::document::view const& change_stream::iterator::operator*() const {
    return _change_stream->_doc;
}

bsoncxx::v_noabi::document::view const* change_stream::iterator::operator->() const {
    return std::addressof(_change_stream->_doc);
}

change_stream::iterator& change_stream::iterator::operator++() {
    if (_change_stream) {
        try {
            v1::change_stream::internal::advance_iterator(_change_stream->_stream);
        } catch (v1::exception const& ex) {
            _change_stream->_doc = {};
            throw_exception<v_noabi::query_exception>(ex);
        }

        // Backward compatibility: support `*iter -> T const&`.
        _change_stream->_doc = v1::change_stream::internal::doc(_change_stream->_stream);
    }
    return *this;
}

change_stream::iterator::iterator(change_stream* change_stream, bool is_end)
    : _change_stream{change_stream},
      _is_end{change_stream ? is_end : true} // `_change_stream == nullptr` implies `_is_end == true`.
{
    if (!is_end) {
        // Do not advance on consecutive calls to `.begin()`.
        if (v1::change_stream::internal::has_doc(_change_stream->_stream)) {
            return;
        }

        try {
            // Advance to first event on begin() to keep operator*() state-machine-free.
            v1::change_stream::internal::advance_iterator(_change_stream->_stream);
        } catch (v1::exception const& ex) {
            _change_stream->_doc = {};
            throw_exception<v_noabi::query_exception>(ex);
        }

        // Backward compatibility: support `*iter -> T const&`.
        _change_stream->_doc = v1::change_stream::internal::doc(_change_stream->_stream);
    }
}

bool operator==(change_stream::iterator const& lhs, change_stream::iterator const& rhs) noexcept {
    // Backward compatibility: different underlying streams never compare equal, even for end iterators.
    if (lhs._change_stream != rhs._change_stream) {
        return false;
    }

    // One is an end iterator and the other is exhausted.
    if (!lhs._is_end != !rhs._is_end) {
        return (lhs._change_stream ? lhs : rhs).is_exhausted();
    }

    // Both are end iterators (null) or both are active iterators.
    return true;
}

bool change_stream::iterator::is_exhausted() const {
    if (_change_stream) {
        return !v1::change_stream::internal::has_doc(_change_stream->_stream);
    }

    MONGOCXX_PRIVATE_UNREACHABLE; // scan-build: warning: Forming reference to null pointer [core.NonNullParamChecker]
}

} // namespace v_noabi
} // namespace mongocxx
