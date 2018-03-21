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

///
/// Class representing a pointer to the result set of a query on a MongoDB server.
///
/// Clients can iterate through a change_stream::iterator to retrieve results.
///
/// @note By default, change_streams timeout after 10 minutes of inactivity.
///
class MONGOCXX_API change_stream {
   public:
    enum class type { k_non_tailable, k_tailable, k_tailable_await };

    class MONGOCXX_API iterator;

    ///
    /// Move constructs a change_stream.
    ///
    change_stream(change_stream&&) noexcept;

    ///
    /// Move assigns a change_stream.
    ///
    change_stream& operator=(change_stream&&) noexcept;

    ///
    /// Destroys a change_stream.
    ///
    ~change_stream();

    ///
    /// A change_stream::iterator that points to the beginning of any available
    /// results.  If begin() is called more than once, the change_stream::iterator
    /// returned points to the next remaining result, not the result of
    /// the original call to begin().
    ///
    /// For a tailable change_stream, when change_stream.begin() == change_stream.end(), no
    /// documents are available.  Each call to change_stream.begin() checks again
    /// for newly-available documents.
    ///
    /// @return the change_stream::iterator
    ///
    /// @throws mongocxx::query_exception if the query failed
    ///
    iterator begin();

    ///
    /// A change_stream::iterator indicating change_stream exhaustion, meaning that
    /// no documents are available from the change_stream.
    ///
    /// @return the change_stream::iterator
    ///
    iterator end();

   private:
    friend class collection;
    friend class client;
    friend class database;
    friend class index_view;
    friend class change_stream;
    friend class change_stream::iterator;

    MONGOCXX_PRIVATE change_stream(void* change_stream_ptr,
                            bsoncxx::stdx::optional<type> change_stream_type = bsoncxx::stdx::nullopt);

    class MONGOCXX_PRIVATE impl;
    std::unique_ptr<impl> _impl;
};

///
/// Class representing an input iterator of documents in a MongoDB change_stream
/// result set.
///
/// All non-end iterators derived from the same mongocxx::change_stream move in
/// lock-step.  Dereferencing any non-end() iterator always gives the first
/// remaining document in the change_stream.  Incrementing one non-end iterator is
/// equivalent to incrementing them all.
///
/// An iterator is 'exhausted' when no documents are available. An
/// end-iterator is always exhausted. A non-end iterator is exhausted when the
/// originating mongocxx::change_stream has no more documents.  When an iterator is
/// exhausted, it must not be dereferenced or incremented.
///
/// For iterators of a tailable change_stream, calling change_stream.begin() may revive an
/// exhausted iterator so that it no longer compares equal to the
/// end-iterator.
///
class MONGOCXX_API change_stream::iterator
    : public std::iterator<std::input_iterator_tag, bsoncxx::document::view> {
   public:
    ///
    /// Dereferences the view for the document currently being pointed to.
    ///
    const bsoncxx::document::view& operator*() const;

    ///
    /// Accesses a member of the dereferenced document currently being pointed to.
    ///
    const bsoncxx::document::view* operator->() const;

    ///
    /// Pre-increments the iterator to move to the next document.
    ///
    /// @throws mongocxx::query_exception if the query failed
    ///
    iterator& operator++();

    ///
    /// Post-increments the iterator to move to the next document.
    ///
    /// @throws mongocxx::query_exception if the query failed
    ///
    void operator++(int);

   private:

    ///
    /// @{
    ///
    /// Compare two iterators for (in)-equality.  Iterators compare equal if
    /// they point to the same underlying change_stream or if both are exhausted.
    ///
    /// @relates iterator
    ///
    friend MONGOCXX_API bool MONGOCXX_CALL operator==(const iterator&, const iterator&);
    friend MONGOCXX_API bool MONGOCXX_CALL operator!=(const iterator&, const iterator&);
    ///
    /// @}
    ///

    MONGOCXX_PRIVATE bool is_exhausted() const;

    MONGOCXX_PRIVATE explicit iterator(change_stream* change_stream);

    // If this pointer is null, the iterator is considered "past-the-end".
    change_stream* _change_stream;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
