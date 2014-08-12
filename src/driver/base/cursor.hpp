/**
 * Copyright 2014 MongoDB Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "mongoc.h"

#include "bson/document.hpp"

namespace mongo {
namespace driver {

class collection;

class cursor {

    friend class collection;

   public:
    class iterator : public std::iterator<std::forward_iterator_tag, const bson::document::view&,
                                          std::ptrdiff_t, const bson::document::view*,
                                          const bson::document::view&> {

        friend class cursor;

       public:
        const bson::document::view& operator*() const;
        const bson::document::view* operator->() const;

        iterator& operator++();

        bool operator==(const iterator& rhs) const;
        bool operator!=(const iterator& rhs) const;

       private:
        iterator(mongoc_cursor_t* cursor);

        mongoc_cursor_t* _cursor;
        bson::document::view _doc;
        bool _at_end;
    };  // class iterator

    iterator begin();
    iterator end();

    cursor(cursor&& rhs);
    cursor& operator=(cursor&& rhs);
    ~cursor();

   private:
    cursor(mongoc_cursor_t* cursor);

    cursor(const cursor& cursor) = delete;
    cursor& operator=(const cursor& cursor) = delete;

    mongoc_cursor_t* _cursor;
};

}  // namespace driver
}  // namespace mongo
