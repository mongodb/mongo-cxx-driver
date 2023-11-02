// Copyright 2023 MongoDB Inc.
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

#include <mongocxx/v1/mongocxx/config/export.hpp>

#if !defined(MONGOCXX_API_H)
#error "MONGOCXX_API_H is not defined!"
#endif

#if !defined(MONGOCXX_API)
#error "MONGOCXX_API is not defined!"
#endif

#if !defined(MONGOCXX_PRIVATE)
#error "MONGOCXX_PRIVATE is not defined!"
#endif

#if !defined(MONGOCXX_DEPRECATED)
#error "MONGOCXX_DEPRECATED is not defined!"
#endif

#if !defined(MONGOCXX_DEPRECATED_EXPORT)
#error "MONGOCXX_DEPRECATED_EXPORT is not defined!"
#endif

#if !defined(MONGOCXX_DEPRECATED_NO_EXPORT)
#error "MONGOCXX_DEPRECATED_NO_EXPORT is not defined!"
#endif
