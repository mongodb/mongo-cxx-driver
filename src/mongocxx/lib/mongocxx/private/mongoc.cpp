// Copyright 2009-present MongoDB, Inc.
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

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace libmongoc {

#ifdef MONGOCXX_TESTING

SILENCE_IGNORED_ATTRIBUTES_BEGIN();

#pragma push_macro("X")
#define X(name) \
    test_util::mock<decltype(&mongoc_##name)>& name = *new test_util::mock<decltype(&mongoc_##name)>(mongoc_##name);
MONGOC_SYMBOLS_XMACRO(X)
#pragma pop_macro("X")

mongocxx::test_util::mock<log_set_handler_cdecl_t>& log_set_handler =
    *new test_util::mock<log_set_handler_cdecl_t>(mongoc_log_set_handler);

SILENCE_IGNORED_ATTRIBUTES_END();

#endif // MONGOCXX_TESTING

} // namespace libmongoc
} // namespace mongocxx
