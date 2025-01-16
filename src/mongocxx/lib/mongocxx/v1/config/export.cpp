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

#include <mongocxx/v1/config/export.hpp>

#if !defined(MONGOCXX_ABI_EXPORT)
#error "mongocxx/v1/config/export.hpp should define MONGOCXX_ABI_EXPORT"
#endif

#if !defined(MONGOCXX_ABI_NO_EXPORT)
#error "mongocxx/v1/config/export.hpp should define MONGOCXX_ABI_NO_EXPORT"
#endif

#if !defined(MONGOCXX_DEPRECATED)
#error "mongocxx/v1/config/export.hpp should define MONGOCXX_DEPRECATED"
#endif

#if !defined(MONGOCXX_ABI_CDECL)
#error "mongocxx/v1/config/export.hpp should define MONGOCXX_ABI_CDECL"
#endif

#if !defined(MONGOCXX_ABI_EXPORT_CDECL)
#error "mongocxx/v1/config/export.hpp should define MONGOCXX_ABI_EXPORT_CDECL"
#endif
