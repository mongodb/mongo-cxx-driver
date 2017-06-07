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

#pragma once

#include <iterator>

#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

///
/// Class representing index_names in an index_names.
///
/// Clients can iterate through a index_names::iterator to retrieve names.
///
class MONGOCXX_API index_names {
   public:
    class MONGOCXX_API iterator;

    index_names();

    ///
    /// A index_names::iterator that points to the beginning of any available
    /// indexes.  If begin() is called more than once, the index_names::iterator
    /// returned points to the next remaining result, not the result of
    /// the original call to begin().
    ///
    /// @return
    ///   the index_names::iterator
    ///
    /// @throws
    ///   mongocxx::query_exception if the query failed
    ///
    iterator begin();

    ///
    /// A index_names::iterator indicating index_names exhaustion, meaning that
    /// no indexes are available.
    ///
    /// @return the index_names::iterator
    ///
    iterator end();
};

class MONGOCXX_API index_names::iterator
    : public std::iterator<std::input_iterator_tag, std::string> {
   public:
    ///
    ///
    const bsoncxx::stdx::string_view& operator*() const;

    ///
    /// Accesses a member of the dereferenced document currently being pointed to.
    ///
    const bsoncxx::stdx::string_view* operator->() const;

    ///
    /// Pre-increments the iterator to move to the next string.
    ///
    /// @throws mongocxx::query_exception if the query failed
    ///
    iterator& operator++();

    ///
    /// Post-increments the iterator to move to the next string.
    ///
    /// @throws mongocxx::query_exception if the query failed
    ///
    void operator++(int);

   private:
    friend class index_names;

    ///
    /// @{
    ///
    /// Compare two iterators for (in)-equality.  Iterators compare equal if
    /// they point to the same underlying index_names or if both are exhausted.
    ///
    /// @relates iterator
    ///
    friend MONGOCXX_API bool MONGOCXX_CALL operator==(const iterator&, const iterator&);
    friend MONGOCXX_API bool MONGOCXX_CALL operator!=(const iterator&, const iterator&);
    ///
    /// @}
    ///

    MONGOCXX_PRIVATE bool is_exhausted() const;

    MONGOCXX_PRIVATE iterator(index_names* index_names);

    // If this pointer is null, the iterator is considered "past-the-end".
    index_names* _index_names;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>