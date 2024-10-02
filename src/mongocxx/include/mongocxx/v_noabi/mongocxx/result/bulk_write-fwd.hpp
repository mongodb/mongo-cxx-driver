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

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {

class bulk_write;

}  // namespace result
}  // namespace v_noabi
}  // namespace mongocxx

namespace mongocxx {
namespace result {

using ::mongocxx::v_noabi::result::bulk_write;

}  // namespace result
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Declares @ref mongocxx::v_noabi::result::bulk_write.
///

#if defined(MONGOCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

namespace mongocxx {
namespace result {

/// @ref mongocxx::v_noabi::result::bulk_write
class bulk_write {};

}  // namespace result
}  // namespace mongocxx

#endif  // defined(MONGOCXX_PRIVATE_DOXYGEN_PREPROCESSOR)
