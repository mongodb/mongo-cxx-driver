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

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/validation_criteria.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Class representing the optional arguments to a MongoDB createCollection command
///
/// @deprecated Use @ref mongocxx::v_noabi::database::create_collection overloads with a BSON
/// document option parameter instead.
///
class create_collection_deprecated {
   public:
    ///
    /// To create a capped collection, specify true.
    ///
    /// @note If you specify true, you must also set a maximum size using the size() method.
    ///
    /// @param capped
    ///   Whether or not this collection will be capped.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/glossary/#term-capped-collection
    ///
    MONGOCXX_ABI_EXPORT_CDECL(create_collection_deprecated&) capped(bool capped);

    ///
    /// Gets the current capped setting.
    ///
    /// @return
    ///   Whether or not this collection will be capped.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/glossary/#term-capped-collection
    ///
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<bool>&) capped() const;

    ///
    /// Sets the default collation for this collection.
    ///
    /// @param collation
    ///   The default collation for the collection.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/collation/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(create_collection_deprecated&)
    collation(bsoncxx::v_noabi::document::view_or_value collation);

    ///
    /// Gets the default collation for this collection.
    ///
    /// @return
    ///   The default collation for the collection.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/collation/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(
        const bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value>&)
    collation() const;

    ///
    /// The maximum number of documents allowed in the capped collection.
    ///
    /// @note The size limit takes precedence over this limit. If a capped collection reaches
    ///   the size limit before it reaches the maximum number of documents, MongoDB removes
    ///   old documents.
    ///
    /// @param max_documents
    ///   Maximum number of documents allowed in the collection (if capped).
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(create_collection_deprecated&) max(std::int64_t max_documents);

    ///
    /// Gets the current setting for the maximum number of documents allowed in the capped
    /// collection.
    ///
    /// @return
    ///   Maximum number of documents allowed in the collection (if capped).
    ///
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<std::int64_t>&) max() const;

    ///
    /// When true, disables the power of 2 sizes allocation for the collection.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/create/
    ///
    /// @param no_padding
    ///   When true, disables power of 2 sizing for this collection.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(create_collection_deprecated&) no_padding(bool no_padding);

    ///
    /// Gets the current value of the "no padding" option for the collection.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/command/create/
    ///
    /// @return
    ///   When true, power of 2 sizing is disabled for this collection.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<bool>&) no_padding() const;

    ///
    /// A maximum size, in bytes, for a capped collection.
    ///
    /// @note Once a capped collection reaches its maximum size, MongoDB removes older
    ///   documents to make space for new documents.
    ///
    /// @note Size is required for capped collections and ignored for other collections.
    ///
    /// @param max_size
    ///   Maximum size, in bytes, of this collection (if capped).
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(create_collection_deprecated&) size(std::int64_t max_size);

    ///
    /// Gets the current size setting, for a capped collection.
    ///
    /// @return
    ///   Maximum size, in bytes, of this collection (if capped).
    ///
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<std::int64_t>&) size() const;

    ///
    /// Specify configuration to the storage on a per-collection basis.
    ///
    /// @note This option is currently only available with the WiredTiger storage engine.
    ///
    /// @param storage_engine_opts
    ///   Configuration options specific to the storage engine.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(create_collection_deprecated&)
    storage_engine(bsoncxx::v_noabi::document::view_or_value storage_engine_opts);

    ///
    /// Gets the current storage engine configuration for this collection.
    ///
    /// @return
    ///   Configuration options specific to the storage engine.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(
        const bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value>&)
    storage_engine() const;

    ///
    /// Specify validation criteria for this collection.
    ///
    /// @param validation
    ///   Validation criteria for this collection.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/document-validation/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(create_collection_deprecated&)
    validation_criteria(mongocxx::v_noabi::validation_criteria validation);

    ///
    /// Gets the current validation criteria for this collection.
    ///
    /// @return
    ///   Validation criteria for this collection.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/document-validation/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(
        const bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::validation_criteria>&)
    validation_criteria() const;

    ///
    /// Return a bson document representing the options set on this object.
    ///
    /// @deprecated
    ///   This method is deprecated. To determine which options are set on this object, use the
    ///   provided accessors instead.
    ///
    /// @return Options, as a document.
    ///
    MONGOCXX_DEPRECATED MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::value)
        to_document() const;

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::value) to_document_deprecated() const;

    ///
    /// @deprecated
    ///   This method is deprecated. To determine which options are set on this object, use the
    ///   provided accessors instead.
    ///
    MONGOCXX_DEPRECATED inline operator bsoncxx::v_noabi::document::value() const {
        return to_document_deprecated();
    }

   private:
    bsoncxx::v_noabi::stdx::optional<bool> _capped;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _collation;
    bsoncxx::v_noabi::stdx::optional<std::int64_t> _max_documents;
    bsoncxx::v_noabi::stdx::optional<std::int64_t> _max_size;
    bsoncxx::v_noabi::stdx::optional<bool> _no_padding;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value>
        _storage_engine_opts;
    bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::validation_criteria> _validation;
};

///
/// Equivalent to @ref create_collection_deprecated.
///
/// @deprecated Use @ref mongocxx::v_noabi::database::create_collection overloads with a BSON
/// document option parameter instead.
///
MONGOCXX_DEPRECATED typedef create_collection_deprecated create_collection;

}  // namespace options
}  // namespace v_noabi
}  // namespace mongocxx

namespace mongocxx {
namespace options {

using ::mongocxx::v_noabi::options::create_collection;
using ::mongocxx::v_noabi::options::create_collection_deprecated;

}  // namespace options
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::create_collection.
///
/// @deprecated Use @ref mongocxx::v_noabi::database::create_collection overloads with a BSON
/// document option parameter instead.
///

#if defined(MONGOCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

namespace mongocxx {
namespace options {

/// @ref mongocxx::v_noabi::options::create_collection
class create_collection {};

/// @ref mongocxx::v_noabi::options::create_collection_deprecated
class create_collection_deprecated {};

}  // namespace options
}  // namespace mongocxx

#endif  // defined(MONGOCXX_PRIVATE_DOXYGEN_PREPROCESSOR)
