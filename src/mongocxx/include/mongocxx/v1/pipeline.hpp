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

#include <mongocxx/v1/pipeline-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/array/view-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>

#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <cstdint>
#include <memory>

namespace mongocxx {
namespace v1 {

///
/// A MongoDB aggregation pipeline.
///
/// @see
/// - [Aggregation Operations (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/aggregates/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class pipeline {
   private:
    class impl;
    std::unique_ptr<impl> _impl;

   public:
    ///
    /// Destroy this object.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~pipeline();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() pipeline(pipeline&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) operator=(pipeline&& other) noexcept;

    ///
    /// Copy constructor.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() pipeline(pipeline const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) operator=(pipeline const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - `this->view().empty() == true`
    ///
    MONGOCXX_ABI_EXPORT_CDECL() pipeline();

    ///
    /// Return the current array of aggregation stages.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::array::view) view_array() const;

    ///
    /// Append the given aggregation stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) append_stage(bsoncxx::v1::document::view v);

    ///
    /// Append an array of aggregation stages.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) append_stages(bsoncxx::v1::array::view v);

    ///
    /// Append the "$addFields" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) add_fields(bsoncxx::v1::document::view v);

    ///
    /// Append the "$bucket" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) bucket(bsoncxx::v1::document::view v);

    ///
    /// Append the "$bucketAuto" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) bucket_auto(bsoncxx::v1::document::view v);

    ///
    /// Append the "$collStats" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) coll_stats(bsoncxx::v1::document::view v);

    ///
    /// Append the "$collStats" stage with an empty document.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) coll_stats();

    ///
    /// Append the "$count" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) count(bsoncxx::v1::stdx::string_view v);

    ///
    /// Append the "$currentOp" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) current_op(bsoncxx::v1::document::view v);

    ///
    /// Append the "$facet" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) facet(bsoncxx::v1::document::view v);

    ///
    /// Append the "$geoNear" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) geo_near(bsoncxx::v1::document::view v);

    ///
    /// Append the "$graphLookup" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) graph_lookup(bsoncxx::v1::document::view v);

    ///
    /// Append the "$group" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) group(bsoncxx::v1::document::view v);

    ///
    /// Append the "$indexStats" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) index_stats();

    ///
    /// Append the "$limit" stage.
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) limit(std::int32_t v);
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) limit(std::int64_t v);
    /// @}
    ///

    ///
    /// Append the "$listLocalSessions" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) list_local_sessions(bsoncxx::v1::document::view v);

    ///
    /// Append the "$listSessions" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) list_sessions(bsoncxx::v1::document::view v);

    ///
    /// Append the "$lookup" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) lookup(bsoncxx::v1::document::view v);

    ///
    /// Append the "$match" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) match(bsoncxx::v1::document::view v);

    ///
    /// Append the "$merge" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) merge(bsoncxx::v1::document::view v);

    ///
    /// Append the "$out" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) out(bsoncxx::v1::stdx::string_view v);

    ///
    /// Append the "$project" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) project(bsoncxx::v1::document::view v);

    ///
    /// Append the "$redact" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) redact(bsoncxx::v1::document::view v);

    ///
    /// Append the "$replaceRoot" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) replace_root(bsoncxx::v1::document::view v);

    ///
    /// Append the "$sample" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) sample(std::int32_t v);

    ///
    /// Append the "$skip" stage.
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) skip(std::int32_t v);
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) skip(std::int64_t v);
    /// @}
    ///

    ///
    /// Append the "$sort" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) sort(bsoncxx::v1::document::view v);

    ///
    /// Append the "$sortByCount" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) sort_by_count(bsoncxx::v1::document::view v);

    ///
    /// Append the "$sortByCount" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) sort_by_count(bsoncxx::v1::stdx::string_view v);

    ///
    /// Append the "$unwind" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) unwind(bsoncxx::v1::document::view v);

    ///
    /// Append the "$unwind" stage.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pipeline&) unwind(bsoncxx::v1::stdx::string_view v);
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::pipeline.
///
