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

/// TODO: doc
class MONGOCXX_API change_stream {
   public:
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

    /// TODO: doc
    iterator begin();

    /// TODO: doc
    iterator end();

   private:
    friend class collection;
    friend class change_stream::iterator;

    MONGOCXX_PRIVATE change_stream(void* change_stream_ptr);

    class MONGOCXX_PRIVATE impl;
    std::unique_ptr<impl> _impl;
};

/// TODO: doc
class MONGOCXX_API change_stream::iterator {
   public:
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
    /// @throws mongocxx::query_exception if the query failed
    ///
    iterator& operator++();

    // TODO: do we need all these? what should be the values?
    // https://stackoverflow.com/questions/37031805/preparation-for-stditerator-being-deprecated/38103394
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
    friend MONGOCXX_API bool MONGOCXX_CALL operator==(const iterator&, const iterator&);
    friend MONGOCXX_API bool MONGOCXX_CALL operator!=(const iterator&, const iterator&);
    ///
    /// @}
    ///

    MONGOCXX_PRIVATE bool is_exhausted() const;

    // TODO: do we need this?
    // If this pointer is null, the iterator is considered "past-the-end".
    change_stream* _change_stream;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
