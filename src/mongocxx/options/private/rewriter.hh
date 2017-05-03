// Copyright 2016 MongoDB Inc.
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

#include <mongocxx/options/find.hpp>
#include <mongocxx/test_util/export_for_testing.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

class MONGOCXX_TEST_API rewriter {
   public:
    ///
    /// Converts all modifiers from the given options::find object into their respective top-level
    /// options. Each modifier is only converted if its respective top-level option doesn't already
    /// exist.
    ///
    /// @throws
    ///   mongocxx::logic_error if any modifiers are invalid, or if the unsupported "$query" or
    ///   "$explain" modifiers are set.
    ///
    static find rewrite_find_modifiers(const find& options);
};

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
