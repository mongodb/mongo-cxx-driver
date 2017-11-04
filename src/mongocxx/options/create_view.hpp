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

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/pipeline.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/write_concern.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

///
/// Class representing the optional arguments to a view creation operation.
///
/// @see
///   https://docs.mongodb.com/master/core/views/
///
class MONGOCXX_API create_view {
   public:
    create_view() = default;

    create_view(const create_view& other) = delete;
    create_view& operator=(const create_view& other) = delete;

    ///
    /// Sets the default collation for this view.
    ///
    /// @param collation
    ///   The default collation for the view.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    ///   https://docs.mongodb.com/master/reference/collation/
    ///
    create_view& collation(bsoncxx::document::view_or_value collation);

    ///
    /// Gets the default collation for this view.
    ///
    /// @return
    ///   The default collation for the view.
    ///
    /// @see
    ///   https://docs.mongodb.com/master/reference/collation/
    ///
    const stdx::optional<bsoncxx::document::view_or_value>& collation() const;

    ///
    /// Sets the pipeline that will be used to compute this view.
    ///
    /// @param pipeline
    ///   Pipeline that will be used to compute the view.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    create_view& pipeline(pipeline pipeline);

    ///
    /// Gets the pipeline that will be used to compute this view.
    ///
    /// @return
    ///   Pipeline that will be used to compute the view.
    ///
    const stdx::optional<class pipeline>& pipeline() const;

    ///
    /// Sets the write concern that will be used when computing this view.
    ///
    /// @param write_concern
    ///   Write concern that will be used when computing the view.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    create_view& write_concern(mongocxx::write_concern write_concern);

    ///
    /// Gets the write concern that will be used when computing this view.
    ///
    /// @return
    ///   Write concern that will be used when computing the view.
    ///
    const stdx::optional<class mongocxx::write_concern>& write_concern() const;

    ///
    /// Return a bson document representing the options set on this object.
    ///
    /// @deprecated
    ///   This method is deprecated. To determine which options are set on this object, use the
    ///   provided accessors instead.
    ///
    /// @return Options, as a document.
    ///
    MONGOCXX_DEPRECATED bsoncxx::document::value to_document() const;
    bsoncxx::document::value to_document_deprecated() const;

    ///
    /// @deprecated
    ///   This method is deprecated. To determine which options are set on this object, use the
    ///   provided accessors instead.
    ///
    MONGOCXX_DEPRECATED MONGOCXX_INLINE operator bsoncxx::document::value() const;

   private:
    stdx::optional<bsoncxx::document::view_or_value> _collation;
    stdx::optional<class pipeline> _pipeline;
    stdx::optional<class write_concern> _write_concern;

    friend MONGOCXX_API bool MONGOCXX_CALL operator==(const create_view&, const create_view&);
    friend MONGOCXX_API bool MONGOCXX_CALL operator!=(const create_view&, const create_view&);
};

MONGOCXX_INLINE create_view::operator bsoncxx::document::value() const {
    return to_document_deprecated();
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
