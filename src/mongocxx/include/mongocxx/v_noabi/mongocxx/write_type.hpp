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

#include <mongocxx/write_type-fwd.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// Used by @ref mongocxx::v_noabi::model::write.
///
enum class write_type {
    /// Inserting a single document into a collection.
    k_insert_one,

    /// Deleting a single document from a collection.
    k_delete_one,

    /// Delete one or more documents from a collection.
    k_delete_many,

    /// Update a single document in a collection.
    k_update_one,

    /// Update one or more documents in a collection.
    k_update_many,

    /// Replace a single document in a collection with a new one.
    k_replace_one,
};

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::write_type.
///
