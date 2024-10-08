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

#pragma once

#include <mongocxx/config/private/prelude.hh>

// See src/bsoncxx/lib/bsoncxx/v_noabi/bsoncxx/test_util/export_for_testing.hh for an explanation of
// the purpose of this header.

#if defined(MONGOCXX_TESTING)
#define MONGOCXX_ABI_EXPORT_TESTING MONGOCXX_ABI_EXPORT
#define MONGOCXX_ABI_EXPORT_CDECL_TESTING(...) MONGOCXX_ABI_EXPORT_CDECL(__VA_ARGS__)
#else
#define MONGOCXX_ABI_EXPORT_TESTING
#define MONGOCXX_ABI_EXPORT_CDECL_TESTING(...) __VA_ARGS__
#endif

#include <mongocxx/config/private/postlude.hh>
