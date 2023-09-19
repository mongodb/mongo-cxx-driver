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

#include "libmongoc.hh"

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace libmongoc {

#ifdef MONGOCXX_TESTING

#if defined(__GNUC__) && (__GNUC__ >= 6) && !defined(__clang__)
// See libmongoc.hh for details on this diagnostic suppression
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
#endif

#define MONGOCXX_LIBMONGOC_SYMBOL(name)               \
    test_util::mock<decltype(&mongoc_##name)>& name = \
        *new test_util::mock<decltype(&mongoc_##name)>(mongoc_##name);
#include "libmongoc_symbols.hh"
#undef MONGOCXX_LIBMONGOC_SYMBOL

#if defined(__GNUC__) && (__GNUC__ >= 6) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

#endif  // MONGOCXX_TESTING

}  // namespace libmongoc
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
