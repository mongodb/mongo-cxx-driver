// Copyright 2018-present MongoDB Inc.
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

#include <memory>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class collection;

class MONGOCXX_API change_stream {
   public:
    class MONGOCXX_API iterator;

    ///
    /// Move constructs a change_stream.
    ///
    change_stream(change_stream&& other) noexcept;

    ///
    /// Move assigns a change_stream.
    ///
    change_stream& operator=(change_stream&& other) noexcept;

    ///
    /// Destroys a change_stream.
    ///
    ~change_stream();

    ///
    /// A change_stream::iterator points to the beginning of any
    /// available notifications. Each call to begin() advances to the next
    /// available notification. The state of all iterators is tracked by the
    /// change_stream itself, so advancing one iterator advances all iterators.
    ///
    /// change_stream::begin() and the increment operators are blocking operations.
    /// They will not return until a notification is available, the max_await_time (from
    /// the options::change_stream) milliseconds have elapsed, or a server
    /// error is encountered.
    ///
    /// When change_stream.begin() == change_stream.end(), no notifications
    /// are available. Each call to change_stream.begin() checks again for
    /// newly-available notifications.
    ///
    /// @return
    ///   The change_stream::iterator
    /// @exception
    ///   Throws mongocxx::query_exception if the query failed.
    ///
    iterator begin();

    ///
    /// A change_stream::iterator indicating stream exhaustion, meaning that
    /// no notifications are available from the stream.
    ///
    /// @return
    ///   The change_stream::iterator indicating exhaustion
    ///
    iterator end();

   private:
    friend class collection;
    friend class change_stream::iterator;

    MONGOCXX_PRIVATE change_stream(void* change_stream_ptr);

    class MONGOCXX_PRIVATE impl;
    std::unique_ptr<impl> _impl;
};

class MONGOCXX_API change_stream::iterator {
   public:
    ///
    /// Default-construct an iterator.
    /// This is equivalent to change_stream::end()
    ///
    iterator();

    ///
    /// Dereferences the view for the document currently being pointed to.
    ///
    const bsoncxx::document::view& operator*() const noexcept;

    ///
    /// Accesses a member of the dereferenced document currently being pointed to.
    ///
    const bsoncxx::document::view* operator->() const noexcept;

    ///
    /// Pre-increments the iterator to move to the next document.
    ///
    /// change_stream::begin() and increment operators are blocking operations.
    /// They will not return until a notification is available, the max_await_time (from
    /// the options::change_stream) miliseconds have elapsed, or a server
    /// error is encountered.
    ///
    /// @throws mongocxx::query_exception if the query failed
    ///
    iterator& operator++();

    ///
    /// Post-increments the iterator to move to the next document.
    ///
    /// change_stream::begin() and increment operators are blocking operations.
    /// They will not return until a notification is available, the max_await_time (from
    /// the options::change_stream) miliseconds have elapsed, or a server
    /// error is encountered.
    ///
    /// @throws mongocxx::query_exception if the query failed
    ///
    void operator++(int);

    // Support input-iterator
    using difference_type = long;
    using value_type = bsoncxx::document::view;
    using pointer = const bsoncxx::document::view*;
    using reference = const bsoncxx::document::view&;
    using iterator_category = std::input_iterator_tag;

   private:
    friend class change_stream;

    MONGOCXX_PRIVATE explicit iterator(change_stream* change_stream);

    ///
    /// @{
    ///
    /// Compare two iterators for (in)-equality.  Iterators compare equal if
    /// they point to the same underlying change_stream or if both are exhausted.
    ///
    /// @relates iterator
    ///
    friend MONGOCXX_API bool MONGOCXX_CALL operator==(const change_stream::iterator&,
                                                      const change_stream::iterator&);

    friend MONGOCXX_API bool MONGOCXX_CALL operator!=(const change_stream::iterator&,
                                                      const change_stream::iterator&);
    ///
    /// @}
    ///

    MONGOCXX_PRIVATE bool is_exhausted() const;

    change_stream* _change_stream;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
