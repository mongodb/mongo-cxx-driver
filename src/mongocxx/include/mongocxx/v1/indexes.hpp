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

#include <mongocxx/v1/indexes-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/types/value-fwd.hpp>
#include <bsoncxx/v1/types/view-fwd.hpp>

#include <mongocxx/v1/client_session-fwd.hpp>
#include <mongocxx/v1/cursor-fwd.hpp>
#include <mongocxx/v1/write_concern-fwd.hpp>

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <chrono>
#include <cstdint>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>
#include <vector>

namespace mongocxx {
namespace v1 {

///
/// Support for MongoDB indexes.
///
/// @note Not to be confused with Atlas Search indexes or Atlas Vector Search indexes.
///
/// @see
/// - [Indexes (MongoDB Manual)](https://www.mongodb.com/docs/manual/indexes/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class indexes {
    // This class implements `IndexView` ("Index View API"):
    //  - https://specifications.readthedocs.io/en/latest/index-management/index-management/

   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Options for @ref mongocxx::v1::indexes::model.
    ///
    /// Supported fields include:
    /// - `background`
    /// - `collation`
    /// - `default_language` ("defaultLanguage")
    /// - `expire_after` ("expireAfterSeconds")
    /// - `hidden`
    /// - `language_override` ("languageOverride")
    /// - `name`
    /// - `partial_filter_expression` ("partialFilterExpression")
    /// - `sparse`
    /// - `storage_engine` ("storageEngine")
    /// - `text_index_version` ("textIndexVersion")
    /// - `twod_bits_precision` ("bits")
    /// - `twod_location_max` ("max")
    /// - `twod_location_min` ("min")
    /// - `twod_sphere_version` ("2dsphereIndexVersion")
    /// - `unique`
    /// - `version` ("v")
    /// - `weights`
    /// - `wildcard_projection` ("wildcardProjection")
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    class options {
        // This class implements `IndexOptions` ("Index View API"):
        //  - https://specifications.readthedocs.io/en/latest/index-management/index-management/

       private:
        class impl;
        void* _impl;

       public:
        ///
        /// Destroy this object.
        ///
        /// @warning Invalidates all associated views.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() ~options();

        ///
        /// Move constructor.
        ///
        /// @par Postconditions:
        /// - `other` is in an assign-or-destroy-only state.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() options(options&& other) noexcept;

        ///
        /// Move assignment.
        ///
        /// @par Postconditions:
        /// - `other` is in an assign-or-destroy-only state.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) operator=(options&& other) noexcept;

        ///
        /// Copy construction.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() options(options const& other);

        ///
        /// Copy assignment.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) operator=(options const& other);

        ///
        /// Default initialization.
        ///
        /// @par Postconditions:
        /// - All supported fields are "unset" or zero-initialized.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() options();

        ///
        /// Return the current "commitQuorum" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> const) commit_quorum() const;

        ///
        /// Set the "background" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) background(bool v);

        ///
        /// Return the current "background" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) background() const;

        ///
        /// Set the "collation" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) collation(bsoncxx::v1::document::value v);

        ///
        /// Return the current "collation" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) collation() const;

        ///
        /// Set the "default_language" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) default_language(std::string v);

        ///
        /// Return the current "default_language" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) default_language() const;

        ///
        /// Set the "expireAfterSeconds" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) expire_after(std::chrono::seconds v);

        ///
        /// Return the current "expireAfterSeconds" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::seconds>) expire_after() const;

        ///
        /// Set the "hidden" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) hidden(bool v);

        ///
        /// Return the current "hidden" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) hidden() const;

        ///
        /// Set the "language_override" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) language_override(std::string v);

        ///
        /// Return the current "language_override" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) language_override()
            const;

        ///
        /// Set the "name" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) name(std::string v);

        ///
        /// Return the current "name" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) name() const;

        ///
        /// Set the "partialFilterExpression" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) partial_filter_expression(bsoncxx::v1::document::value v);

        ///
        /// Return the current "partialFilterExpression" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) partial_filter_expression()
            const;

        ///
        /// Set the "sparse" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) sparse(bool v);

        ///
        /// Return the current "sparse" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) sparse() const;

        ///
        /// Set the "storageEngine" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) storage_engine(bsoncxx::v1::document::value v);

        ///
        /// Return the current "storageEngine" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) storage_engine() const;

        ///
        /// Set the "textIndexVersion" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) text_index_version(std::int32_t v);

        ///
        /// Return the current "textIndexVersion" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) text_index_version() const;

        ///
        /// Set the "bits" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) twod_bits_precision(std::uint8_t v);

        ///
        /// Return the current "bits" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::uint8_t>) twod_bits_precision() const;

        ///
        /// Set the "max" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) twod_location_max(double v);

        ///
        /// Return the current "max" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<double>) twod_location_max() const;

        ///
        /// Set the "min" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) twod_location_min(double v);

        ///
        /// Return the current "min" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<double>) twod_location_min() const;

        ///
        /// Set the "2dsphereIndexVersion" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) twod_sphere_version(std::uint8_t v);

        ///
        /// Return the current "2dsphereIndexVersion" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::uint8_t>) twod_sphere_version() const;

        ///
        /// Set the "unique" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) unique(bool v);

        ///
        /// Return the current "unique" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) unique() const;

        ///
        /// Set the "version" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) version(std::int32_t v);

        ///
        /// Return the current "version" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) version() const;

        ///
        /// Set the "wildcardProjection" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) wildcard_projection(bsoncxx::v1::document::value v);

        ///
        /// Return the current "wildcardProjection" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) wildcard_projection() const;

        ///
        /// Set the "weights" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) weights(bsoncxx::v1::document::value v);

        ///
        /// Return the current "weights" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) weights() const;

        ///
        /// Return these index management options as a document.
        ///
        /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() operator bsoncxx::v1::document::value() const;
    };

    ///
    /// A description of a MongoDB index.
    ///
    /// Supported fields include:
    /// - `keys`
    /// - `options`
    ///
    /// @see
    /// - [Indexes (MongoDB Manual)](https://www.mongodb.com/docs/manual/indexes/)
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    class model {
        // This class implements `IndexModel` ("Index View API"):
        //  - https://specifications.readthedocs.io/en/latest/index-management/index-management/

       private:
        class impl;
        void* _impl;

       public:
        ///
        /// Destroy this object.
        ///
        /// @warning Invalidates all associated views.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() ~model();

        ///
        /// Move constructor.
        ///
        /// @par Postconditions:
        /// - `other` is in an assign-or-destroy-only state.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() model(model&& other) noexcept;

        ///
        /// Move assignment.
        ///
        /// @par Postconditions:
        /// - `other` is in an assign-or-destroy-only state.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(model&) operator=(model&& other) noexcept;

        ///
        /// Copy constructor.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() model(model const& other);

        ///
        /// Copy assignment.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(model&) operator=(model const& other);

        ///
        /// Initialize with the given "keys" and "options".
        ///
        /// @{
        model(bsoncxx::v1::document::value keys, options const& opts)
            : model{std::move(keys), bsoncxx::v1::document::value{opts}} {}

        MONGOCXX_ABI_EXPORT_CDECL() model(bsoncxx::v1::document::value keys, bsoncxx::v1::document::value options);
        /// @}

        ///
        /// Initialize with the given "keys" document.
        ///
        /// @par Postconditions:
        /// `this->options().empty() == true`.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() model(bsoncxx::v1::document::value keys);

        ///
        /// Return the current "keys" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::view) keys() const;

        ///
        /// Return the current "options" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::view) options() const;
    };

    ///
    /// Options for @ref mongocxx::v1::indexes::create_one.
    ///
    /// Supported fields include:
    /// - `comment`
    /// - `commit_quorum` ("commitQuorum")
    /// - `max_time` ("maxTimeMS")
    /// - `write_concern` ("writeConcern")
    ///
    class create_one_options {
        // This class implements `CreateOneIndexOptions` ("Index View API"):
        //  - https://specifications.readthedocs.io/en/latest/index-management/index-management

       private:
        class impl;
        void* _impl;

       public:
        ///
        /// Destroy this object.
        ///
        /// @warning Invalidates all associated views.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() ~create_one_options();

        ///
        /// Move constructor.
        ///
        /// @par Postconditions:
        /// - `other` is in an assign-or-destroy-only state.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() create_one_options(create_one_options&& other) noexcept;

        ///
        /// Move assignment.
        /// @par Postconditions:
        /// - `other` is in an assign-or-destroy-only state.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(create_one_options&) operator=(create_one_options&& other) noexcept;

        ///
        /// Copy construction.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() create_one_options(create_one_options const& other);

        ///
        /// Copy assignment.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(create_one_options&) operator=(create_one_options const& other);

        ///
        /// Default initialization.
        ///
        /// @par Postconditions:
        /// - All supported fields are "unset" or zero-initialized.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() create_one_options();

        ///
        /// Set the "comment" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(create_one_options&) comment(bsoncxx::v1::types::value v);

        ///
        /// Return the current "comment" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view>) comment() const;

        ///
        /// Set the "commitQuorum" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(create_one_options&) commit_quorum(bsoncxx::v1::document::value v);

        ///
        /// Return the current "commitQuorum" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) commit_quorum() const;

        ///
        /// Set the "maxTimeMS" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(create_one_options&) max_time(std::chrono::milliseconds v);

        ///
        /// Return the current "maxTimeMS" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::milliseconds>) max_time() const;

        ///
        /// Set the "writeConcern" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(create_one_options&) write_concern(v1::write_concern v);

        ///
        /// Return the current "writeConcern" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::write_concern>) write_concern() const;
    };

    ///
    /// Options for @ref mongocxx::v1::indexes::create_many.
    ///
    /// Supported fields include:
    /// - `comment`
    /// - `commit_quorum` ("commitQuorum")
    /// - `max_time` ("maxTimeMS")
    /// - `write_concern` ("writeConcern")
    ///
    class create_many_options {
        // This class implements `CreateManyIndexesOptions` ("Index View API"):
        //  - https://specifications.readthedocs.io/en/latest/index-management/index-management

       private:
        void* _impl; // create_index_options_impl

       public:
        ///
        /// Destroy this object.
        ///
        /// @warning Invalidates all associated views.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() ~create_many_options();

        ///
        /// Move constructor.
        ///
        /// @par Postconditions:
        /// - `other` is in an assign-or-destroy-only state.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() create_many_options(create_many_options&& other) noexcept;

        ///
        /// Move assignment.
        /// @par Postconditions:
        /// - `other` is in an assign-or-destroy-only state.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(create_many_options&) operator=(create_many_options&& other) noexcept;

        ///
        /// Copy construction.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() create_many_options(create_many_options const& other);

        ///
        /// Copy assignment.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(create_many_options&) operator=(create_many_options const& other);

        ///
        /// Default initialization.
        ///
        /// @par Postconditions:
        /// - All supported fields are "unset" or zero-initialized.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() create_many_options();

        ///
        /// Set the "comment" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(create_many_options&) comment(bsoncxx::v1::types::value v);

        ///
        /// Return the current "comment" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view>) comment() const;

        ///
        /// Set the "commitQuorum" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(create_many_options&) commit_quorum(bsoncxx::v1::document::value v);

        ///
        /// Return the current "commitQuorum" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) commit_quorum() const;

        ///
        /// Set the "maxTimeMS" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(create_many_options&) max_time(std::chrono::milliseconds v);

        ///
        /// Return the current "maxTimeMS" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::milliseconds>) max_time() const;

        ///
        /// Set the "writeConcern" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(create_many_options&) write_concern(v1::write_concern v);

        ///
        /// Return the current "writeConcern" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::write_concern>) write_concern() const;
    };

    ///
    /// Options for @ref mongocxx::v1::indexes::drop_one.
    ///
    /// Supported fields include:
    /// - `comment`
    /// - `max_time` ("maxTimeMS")
    /// - `write_concern` ("writeConcern")
    ///
    class drop_one_options {
        // This class implements `DropOneIndexOptions` ("Index View API"):
        //  - https://specifications.readthedocs.io/en/latest/index-management/index-management

       private:
        void* _impl; // drop_index_options_impl

       public:
        ///
        /// Destroy this object.
        ///
        /// @warning Invalidates all associated views.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() ~drop_one_options();

        ///
        /// Move constructor.
        ///
        /// @par Postconditions:
        /// - `other` is in an assign-or-destroy-only state.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() drop_one_options(drop_one_options&& other) noexcept;

        ///
        /// Move assignment.
        /// @par Postconditions:
        /// - `other` is in an assign-or-destroy-only state.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(drop_one_options&) operator=(drop_one_options&& other) noexcept;

        ///
        /// Copy construction.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() drop_one_options(drop_one_options const& other);

        ///
        /// Copy assignment.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(drop_one_options&) operator=(drop_one_options const& other);

        ///
        /// Default initialization.
        ///
        /// @par Postconditions:
        /// - All supported fields are "unset" or zero-initialized.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() drop_one_options();

        ///
        /// Set the "comment" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(drop_one_options&) comment(bsoncxx::v1::types::value v);

        ///
        /// Return the current "comment" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view>) comment() const;

        ///
        /// Set the "maxTimeMS" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(drop_one_options&) max_time(std::chrono::milliseconds v);

        ///
        /// Return the current "maxTimeMS" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::milliseconds>) max_time() const;

        ///
        /// Set the "writeConcern" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(drop_one_options&) write_concern(v1::write_concern v);

        ///
        /// Return the current "writeConcern" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::write_concern>) write_concern() const;
    };

    ///
    /// Options for @ref mongocxx::v1::indexes::drop_all.
    ///
    /// Supported fields include:
    /// - `comment`
    /// - `max_time` ("maxTimeMS")
    /// - `write_concern` ("writeConcern")
    ///
    class drop_all_options {
        // This class implements `DropAllIndexesOptions` ("Index View API"):
        //  - https://specifications.readthedocs.io/en/latest/index-management/index-management

       private:
        void* _impl; // drop_index_options_impl

       public:
        ///
        /// Destroy this object.
        ///
        /// @warning Invalidates all associated views.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() ~drop_all_options();

        ///
        /// Move constructor.
        ///
        /// @par Postconditions:
        /// - `other` is in an assign-or-destroy-only state.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() drop_all_options(drop_all_options&& other) noexcept;

        ///
        /// Move assignment.
        /// @par Postconditions:
        /// - `other` is in an assign-or-destroy-only state.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(drop_all_options&) operator=(drop_all_options&& other) noexcept;

        ///
        /// Copy construction.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() drop_all_options(drop_all_options const& other);

        ///
        /// Copy assignment.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(drop_all_options&) operator=(drop_all_options const& other);

        ///
        /// Default initialization.
        ///
        /// @par Postconditions:
        /// - All supported fields are "unset" or zero-initialized.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() drop_all_options();

        ///
        /// Set the "comment" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(drop_all_options&) comment(bsoncxx::v1::types::value v);

        ///
        /// Return the current "comment" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view>) comment() const;

        ///
        /// Set the "maxTimeMS" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(drop_all_options&) max_time(std::chrono::milliseconds v);

        ///
        /// Return the current "maxTimeMS" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::milliseconds>) max_time() const;

        ///
        /// Set the "writeConcern" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(drop_all_options&) write_concern(v1::write_concern v);

        ///
        /// Return the current "writeConcern" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::write_concern>) write_concern() const;
    };

    ///
    /// Options for @ref mongocxx::v1::indexes::list.
    ///
    /// Supported fields include:
    /// - `batch_size` ("batchSize")
    /// - `comment` ("maxTimeMS")
    ///
    class list_options {
        // This class implements `ListIndexesOptions` ("Index View API"):
        //  - https://specifications.readthedocs.io/en/latest/index-management/index-management

       private:
        class impl;
        void* _impl;

       public:
        ///
        /// Destroy this object.
        ///
        /// @warning Invalidates all associated views.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() ~list_options();

        ///
        /// Move constructor.
        ///
        /// @par Postconditions:
        /// - `other` is in an assign-or-destroy-only state.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() list_options(list_options&& other) noexcept;

        ///
        /// Move assignment.
        /// @par Postconditions:
        /// - `other` is in an assign-or-destroy-only state.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(list_options&) operator=(list_options&& other) noexcept;

        ///
        /// Copy construction.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() list_options(list_options const& other);

        ///
        /// Copy assignment.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(list_options&) operator=(list_options const& other);

        ///
        /// Default initialization.
        ///
        /// @par Postconditions:
        /// - All supported fields are "unset" or zero-initialized.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() list_options();

        ///
        /// Set the "batchSize" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(list_options&) batch_size(std::int32_t v);

        ///
        /// Return the current "batchSize" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) batch_size() const;

        ///
        /// Set the "comment" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(list_options&) comment(bsoncxx::v1::types::value v);

        ///
        /// Return the current "comment" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view>) comment() const;
    };

    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~indexes();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() indexes(indexes&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(indexes&) operator=(indexes&& other) noexcept;

    ///
    /// Copy constructor.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() indexes(indexes const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(indexes&) operator=(indexes const& other);

    ///
    /// Return all indexes in the associated collection.
    ///
    /// @param list_opts "listIndexes" command options."
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) list(list_options const& list_opts = {});

    ///
    /// Return all indexes in the associated collection.
    ///
    /// @param session The session with which this operation is associated.
    /// @param list_opts "listIndexes" command options.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) list(v1::client_session const& session, list_options const& list_opts = {});

    ///
    /// Equivalent to `this->create_one(model(keys, index_opts), create_opts)`.
    ///
    bsoncxx::v1::stdx::optional<std::string> create_one(
        bsoncxx::v1::document::value keys,
        bsoncxx::v1::document::value index_opts = {},
        create_one_options const& create_opts = {}) {
        return this->create_one(model{std::move(keys), std::move(index_opts)}, create_opts);
    }

    ///
    /// Equivalent to `this->create_one(session, model(keys, index_opts), create_opts)`.
    ///
    bsoncxx::v1::stdx::optional<std::string> create_one(
        v1::client_session const& session,
        bsoncxx::v1::document::value keys,
        bsoncxx::v1::document::value index_opts = {},
        create_one_options const& create_opts = {}) {
        return this->create_one(session, model{std::move(keys), std::move(index_opts)}, create_opts);
    }

    ///
    /// Create a single index in the associated collection.
    ///
    /// Equivalent to `this->create_many(...)` with a single index model.
    ///
    /// @param index The definition of the new index. The options must be applicable to the "createIndexes" command.
    /// @param create_opts "createIndexes" command options.
    ///
    /// @returns The name of the created index. Empty when the index already exists.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::string>) create_one(
        model const& index,
        create_one_options const& create_opts = {});

    ///
    /// Create a single index in the associated collection.
    ///
    /// Equivalent to `this->create_many(...)` with a single index model.
    ///
    /// @param session The session with which this operation is associated.
    /// @param index The definition of the new index. The options must be applicable to the "createIndexes" command.
    /// @param create_opts "createIndexes" command options.
    ///
    /// @returns The name of the created index. Empty when the index already exists.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::string>)
    create_one(v1::client_session const& session, model const& index, create_one_options const& create_opts = {});

    ///
    /// Create multiple indexes in the associated collection.
    ///
    /// @param indexes The definitions of the new indexes. The options must be applicable to the "createIndexes"
    /// command.
    /// @param create_opts "createIndexes" command options.
    ///
    /// @returns The raw server response.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::value) create_many(
        std::vector<model> const& indexes,
        create_many_options const& create_opts = {});

    ///
    /// Create multiple indexes in the associated collection.
    ///
    /// @param session The session with which this operation is associated.
    /// @param indexes The definitions of the new indexes. The options must be applicable to the "createIndexes"
    /// command.
    /// @param create_opts "createIndexes" command options.
    ///
    /// @returns The raw server response.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::value)
    create_many(
        v1::client_session const& session,
        std::vector<model> const& indexes,
        create_many_options const& create_opts = {});

    ///
    /// Equivalent to `this->drop_one(mongocxx::v1::indexes::model(keys, index_opts), drop_opts)`.
    ///
    void drop_one(
        bsoncxx::v1::document::value keys,
        bsoncxx::v1::document::value index_opts = {},
        drop_one_options const& drop_opts = {}) {
        return this->drop_one(model{std::move(keys), std::move(index_opts)}, drop_opts);
    }

    ///
    /// Equivalent to `this->drop_one(session, mongocxx::v1::indexes::model(keys, index_opts), drop_opts)`.
    ///
    void drop_one(
        client_session const& session,
        bsoncxx::v1::document::value keys,
        bsoncxx::v1::document::value index_opts = {},
        drop_one_options const& drop_opts = {}) {
        return this->drop_one(session, model{std::move(keys), std::move(index_opts)}, drop_opts);
    }

    ///
    /// Drop a single index in the associated collection.
    ///
    /// @param index The name of index to drop. The options must be applicable to the "dropIndexes" command.
    /// @param drop_opts "dropIndexes" command options.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::indexes::errc::invalid_name if `name` is equal to "*".
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) drop_one(
        bsoncxx::v1::stdx::string_view index,
        drop_one_options const& drop_opts = {});

    ///
    /// Drop a single index in the associated collection.
    ///
    /// @param session The session with which this operation is associated.
    /// @param index The name of index to drop. The options must be applicable to the "dropIndexes" command.
    /// @param drop_opts "dropIndexes" command options.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::indexes::errc::invalid_name if `name` is equal to "*".
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    drop_one(
        v1::client_session const& session,
        bsoncxx::v1::stdx::string_view index,
        drop_one_options const& drop_opts = {});

    ///
    /// Drop a single index in the associated collection.
    ///
    /// @param index The description of the index to drop. The options must be applicable to the "dropIndexes" command.
    /// @param drop_opts "dropIndexes" command options.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::indexes::errc::invalid_name if `name` is equal to "*".
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) drop_one(model const& index, drop_one_options const& drop_opts = {});

    ///
    /// Drop a single index in the associated collection.
    ///
    /// @param session The session with which this operation is associated.
    /// @param index The description of the index to drop.
    /// @param drop_opts "dropIndexes" command options.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    drop_one(v1::client_session const& session, model const& index, drop_one_options const& drop_opts = {});

    ///
    /// Drop all indexes in the associated collection.
    ///
    /// @param drop_opts "dropIndexes" command options.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) drop_all(drop_all_options const& drop_opts = {});

    ///
    /// Drop all indexes in the associated collection.
    ///
    /// @param session The session with which this operation is associated.
    /// @param drop_opts "dropIndexes" command options.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) drop_all(v1::client_session const& session, drop_all_options const& drop_opts = {});

    ///
    /// Errors codes which may be returned by @ref mongocxx::v1::indexes.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    enum class errc {
        zero,              ///< Zero.
        expired_after_i32, ///< The "expiredAfterSeconds" field must be representable as an `std::int32_t`.
        invalid_name,      ///< "*" is not a permitted index name.
    };

    ///
    /// The error category for @ref mongocxx::v1::instance::errc.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    static MONGOCXX_ABI_EXPORT_CDECL(std::error_category const&) error_category();

    ///
    /// Support implicit conversion to `std::error_code`.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    friend std::error_code make_error_code(errc v) {
        return {static_cast<int>(v), error_category()};
    }
};

} // namespace v1
} // namespace mongocxx

namespace std {

template <>
struct is_error_code_enum<mongocxx::v1::indexes::errc> : true_type {};

} // namespace std

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::indexes.
///
