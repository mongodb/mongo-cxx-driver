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

#include <mongocxx/v1/auto_encryption-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

namespace mongocxx {
namespace v1 {

///
/// Options related In-Use Encryption configuration.
///
/// @see
/// - [MongoClient Options for Queryable Encryption (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/queryable-encryption/reference/qe-options-clients/)
/// - [MongoClient Options for CSFLE](https://www.mongodb.com/docs/manual/core/csfle/reference/csfle-options-clients/)
///
class auto_encryption {};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::auto_encryption.
///
