# Contributing Guidelines

### Lifecycle Methods

 - default-or-argument-bearing 'user' constructors

 - declaration-or-deletion-of-copy-contructor
 - declaration-or-deletetion-of-move-constructor

 - declaration-or-deletion-of-copy-assignment-operator
 - declaration-or-deletion-of-move-assignment-operator

 - declaration-of-dtor

### Headers

 - License
 - Include Guard (`#pragma once`)
 - Header Prelude
 - System Headers `<vector>` (alphabetical order)
 - Driver Headers `<path/to/header.hpp>` (alphabetical order)
 - Open Namespace mongocxx 
 - `MONGOCXX_INLINE_NAMESPACE_BEGIN`
 -    Code
 - `MONGOCXX_INLINE_NAMESPACE_END`
 - Close Namespace mongocxx
 - Header Postlude

Example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
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

#pragma once

#include <driver/config/prelude.hpp>

#include <vector>

#include <driver/blah.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

// Declarations

// Inline Implementations

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <driver/config/postlude.hpp>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Class Declarations

Guidelines:

 - Blank line at beginning and end of class declaration
 - Public section up top / private at bottom
 - Lifecycle methods first (see rules above)
 - Private Member Ordering
   - Friendships
   - Private Constructors
   - Private Methods
   - Private Variables

Example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
class foo {

    public:
      foo();

      foo(foo&& other) noexcept;
      foo& operator=(foo&& other) noexcept;

      ~foo();

    private:
      friend baz;

      class MONGOCXX_PRIVATE impl;
      std::unique_ptr<impl> _impl;

};
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Inlines
 - Define outside of class declaration
 - Specify inline keyword in declaration and definition (for clarity)

### Relational Operators
 - Prefer to use free functions
