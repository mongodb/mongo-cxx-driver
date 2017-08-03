// Copyright 2017 MongoDB Inc.
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

#include <mongocxx/private/change_stream.hh>
#include <mongocxx/private/collection.hh>
#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

change_stream::change_stream(const collection& coll,
                             const pipeline& pipe,
                             const options::change_stream& options)
    : _impl(
          stdx::make_unique<impl>(coll, std::move(pipe), *coll.read_preference()._impl, options)) {}

change_stream::iterator change_stream::begin() {
    return change_stream::iterator(this);
}

change_stream::iterator change_stream::end() {
    return change_stream::iterator(nullptr);
}

change_stream::change_stream(change_stream&& other) = default;

change_stream::~change_stream() = default;

change_stream& change_stream::operator=(change_stream&& other) = default;

const bsoncxx::document::view& change_stream::iterator::operator*() const {
    return _change_stream->_impl->get();
}

change_stream::iterator& change_stream::iterator::operator++() {
    _change_stream->_impl->get_more();

    return *this;
}

bool change_stream::iterator::is_exhausted() const {
    return !_change_stream || _change_stream->_impl->is_exhausted();
}

change_stream::iterator::iterator(change_stream* change_stream) : _change_stream{change_stream} {
    if (_change_stream) {
        if (_change_stream->_impl->has_started()) {
            _change_stream->_impl->get_more();
        } else {
            _change_stream->_impl->mark_started();
        }
    }
}

const bsoncxx::document::view* change_stream::iterator::operator->() const {
    return &_change_stream->_impl->get();
}

void change_stream::iterator::operator++(int) {
    operator++();
}

change_stream::impl& change_stream::_get_impl() {
    return *_impl;
}

const change_stream::impl& change_stream::_get_impl() const {
    return *_impl;
}

bool MONGOCXX_CALL operator==(const change_stream::iterator& lhs,
                              const change_stream::iterator& rhs) {
    return ((rhs.is_exhausted() && lhs.is_exhausted()) ||
            (lhs._change_stream == rhs._change_stream));
}

bool MONGOCXX_CALL operator!=(const change_stream::iterator& lhs,
                              const change_stream::iterator& rhs) {
    return !(lhs == rhs);
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
