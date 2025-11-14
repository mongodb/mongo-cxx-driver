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

#include <mongocxx/v1/return_document-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

namespace mongocxx {
namespace v1 {

///
/// `returnDocument` from the CRUD API specification.
///
/// @see
/// - [CRUD API (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/crud/crud/)
///
enum class return_document {
    ///
    /// Return the original document.
    ///
    /// @note Equivalent to `"returnNewDocument": false`.
    ///
    k_before,

    ///
    /// Return the updated document.
    ///
    /// @note Equivalent to `"returnNewDocument": true`.
    ///
    k_after,
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::return_document.
///
