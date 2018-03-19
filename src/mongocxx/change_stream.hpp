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

#include <mongocxx/collection.hpp>
#include <mongocxx/options/change_stream.hpp>
#include <mongocxx/pipeline.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class MONGOCXX_API change_stream {
   public:
    class MONGOCXX_API iterator;

    change_stream(change_stream&& other);

    change_stream& operator=(change_stream&& other);

    change_stream(const change_stream& other) = delete;

    change_stream& operator=(const change_stream& other) = delete;

    ~change_stream();

    ///
    /// A change_stream::iterator that points to the beginning of any available notifications. If
    /// begin() is called more than once, the change_stream::iterator returned points to the next
    /// remaining notification, not the result of the original call to begin().
    ///
    /// When change_stream.begin() == change_stream.end(), no notifications are available. Each
    /// call to change_stream.begin() checks again for newly-available notifications.
    ///
    /// @return
    ///   The change_stream::iterator.
    ///
    /// @exception
    ///   Throws mongocxx::query_exception if the query failed.
    ///
    iterator begin();

    ///
    /// A change_stream::iterator indicating stream exhaustion, meaning that
    /// no notifications are available from the stream.
    ///
    /// @return
    ///   The change_stream::iterator indicating exhaustion.
    ///
    iterator end();

   private:
    friend collection;

    MONGOCXX_PRIVATE change_stream(const collection& coll,
                                   const pipeline& pipe,
                                   const options::change_stream& options = {});

    class MONGOCXX_PRIVATE impl;

    MONGOCXX_PRIVATE impl& _get_impl();
    MONGOCXX_PRIVATE const impl& _get_impl() const;

    std::unique_ptr<impl> _impl;
};

class MONGOCXX_API change_stream::iterator
    : public std::iterator<std::input_iterator_tag, bsoncxx::document::view> {
   public:
    ///
    /// Dereferences the notification currently being pointed to.
    ///
    const bsoncxx::document::view& operator*() const;

    ///
    /// Accesses a member of the dereferenced notification currently being pointed to.
    ///
    const bsoncxx::document::view* operator->() const;

    ///
    /// Pre-increments the iterator to move to the next notification.
    ///
    /// @throws mongocxx::query_exception if the query failed
    ///
    iterator& operator++();

    ///
    /// Post-increments the iterator to move to the next notification.
    ///
    /// @throws mongocxx::query_exception if the query failed
    ///
    void operator++(int);

   private:
    friend class change_stream;

    ///
    /// @{
    ///
    /// Compare two iterators for (in)-equality.  Iterators compare equal if
    /// they point to the same underlying change stream or if both are exhausted.
    ///
    /// @relates
    ///   iterator
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
