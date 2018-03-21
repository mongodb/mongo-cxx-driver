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

#include <mongocxx/change_stream.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <tuple>

#include <bsoncxx/private/libbson.hh>
#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/exception/private/error_category.hh>
#include <mongocxx/exception/private/mongoc_error.hh>
#include <mongocxx/exception/query_exception.hpp>
#include <mongocxx/private/change_stream.hh>
#include <mongocxx/private/libmongoc.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
    MONGOCXX_INLINE_NAMESPACE_BEGIN

    change_stream::change_stream(void* change_stream_ptr)
        : _impl(stdx::make_unique<impl>(static_cast<mongoc_change_stream_t*>(change_stream_ptr)) {}

    change_stream::change_stream(change_stream&&) noexcept = default;
    change_stream& change_stream::operator=(change_stream&&) noexcept = default;

    change_stream::~change_stream() = default;

    void change_stream::iterator::operator++(int) {
        operator++();
    }

    change_stream::iterator& change_stream::iterator::operator++() {
        const bson_t* out;
        bson_error_t error;

        if (libmongoc::change_stream_next(_change_stream->_impl->change_stream_t, &out)) {
            _change_stream->_impl->doc = bsoncxx::document::view{bson_get_data(out), out->len};
        } else if (libmongoc::change_stream_error(_change_stream->_impl->change_stream_t, &error)) {
            _change_stream->_impl->mark_dead();
            throw_exception<query_exception>(error);
        } else {
            _change_stream->_impl->mark_nothing_left();
        }
        return *this;
    }

    change_stream::iterator change_stream::begin() {
        if (_impl->is_dead()) {
            return end();
        }
        return iterator(this);
    }

    change_stream::iterator change_stream::end() {
        return iterator(nullptr);
    }

    change_stream::iterator::iterator(change_stream* change_stream) : _change_stream(change_stream) {
        if (_change_stream == nullptr || _change_stream->_impl->has_started()) {
            return;
        }

        _change_stream->_impl->mark_started();
        operator++();
    }

//
// An iterator is exhausted if it is the end-iterator (_change_stream == nullptr)
// or if the underlying _change_stream is marked exhausted.
//
    bool change_stream::iterator::is_exhausted() const {
        return !_change_stream || _change_stream->_impl->is_exhausted();
    }

    const bsoncxx::document::view& change_stream::iterator::operator*() const {
        return _change_stream->_impl->doc;
    }

    const bsoncxx::document::view* change_stream::iterator::operator->() const {
        return &_change_stream->_impl->doc;
    }

//
// Iterators are equal if they point to the same underlying _change_stream or if they
// both are "at the end".  We check for exhaustion first because the most
// common check is `iter != change_stream.end()`.
//
    bool MONGOCXX_CALL operator==(const change_stream::iterator& lhs, const change_stream::iterator& rhs) {
        return ((rhs.is_exhausted() && lhs.is_exhausted()) || (lhs._change_stream == rhs._change_stream));
    }

    bool MONGOCXX_CALL operator!=(const change_stream::iterator& lhs, const change_stream::iterator& rhs) {
        return !(lhs == rhs);
    }

    MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
