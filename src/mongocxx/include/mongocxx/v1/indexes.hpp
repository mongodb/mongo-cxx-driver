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
        /// Initialize with the given "keys" and "options" documents.
        ///
        MONGOCXX_ABI_EXPORT_CDECL() model(bsoncxx::v1::document::value keys, bsoncxx::v1::document::value options);

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
    /// Options for @ref mongocxx::v1::indexes.
    ///
    /// Supported fields include:
    /// - `background`
    /// - `collation`
    /// - `commit_quorum` ("commitQuorum")
    /// - `default_language`
    /// - `expire_after` ("expireAfterSeconds")
    /// - `hidden`
    /// - `language_override`
    /// - `max_time` ("maxTimeMS")
    /// - `name`
    /// - `partial_filter_expression` ("partialFilterExpression")
    /// - `sparse`
    /// - `storage_engine` ("storageEngine")
    /// - `twod_bits_precision` ("bits")
    /// - `twod_location_max` ("max")
    /// - `twod_location_min` ("min")
    /// - `twod_sphere_version` ("2dsphereIndexVersion")
    /// - `unique`
    /// - `version` ("v")
    /// - `weights`
    /// - `write_concern` ("writeConcern")
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
        /// Set the "maxTimeMS" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) max_time(std::chrono::milliseconds max_time);

        ///
        /// Return the current "maxTimeMS" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::milliseconds>) max_time() const;

        ///
        /// Set the "writeConcern" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) write_concern(v1::write_concern write_concern);

        ///
        /// Return the current "writeConcern" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<v1::write_concern>) write_concern() const;

        ///
        /// Set the "commitQuorum" field.
        ///
        /// @note Only applicable to "createIndexes" commands. Ignored by other commands.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) commit_quorum(std::int32_t commit_quorum);

        ///
        /// Set the "commitQuorum" field.
        ///
        /// @note Only applicable to "createIndexes" commands. Ignored by other commands.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) commit_quorum(bsoncxx::v1::stdx::string_view commit_quorum);

        ///
        /// Return the current "commitQuorum" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> const) commit_quorum() const;

        ///
        /// Set the "background" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) background(bool background);

        ///
        /// Return the current "background" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) background() const;

        ///
        /// Set the "unique" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) unique(bool unique);

        ///
        /// Return the current "unique" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) unique() const;

        ///
        /// Set the "hidden" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) hidden(bool hidden);

        ///
        /// Return the current "hidden" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) hidden() const;

        ///
        /// Set the "name" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) name(std::string name);

        ///
        /// Return the current "name" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) name() const;

        ///
        /// Set the "collation" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) collation(bsoncxx::v1::document::view collation);

        ///
        /// Return the current "collation" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) collation() const;

        ///
        /// Set the "sparse" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) sparse(bool sparse);

        ///
        /// Return the current "sparse" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) sparse() const;

        ///
        /// Set the "storageEngine" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) storage_engine(
            bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> storage_engine);

        ///
        /// Return the current "storageEngine" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) storage_engine() const;

        ///
        /// Set the "expireAfterSeconds" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) expire_after(std::chrono::seconds seconds);

        ///
        /// Return the current "expireAfterSeconds" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::seconds>) expire_after() const;

        ///
        /// Set the "version" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) version(std::int32_t v);

        ///
        /// Return the current "version" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) version() const;

        ///
        /// Set the "weights" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) weights(bsoncxx::v1::document::view weights);

        ///
        /// Return the current "weights" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) weights() const;

        ///
        /// Set the "default_language" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) default_language(std::string default_language);

        ///
        /// Return the current "default_language" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) default_language() const;

        ///
        /// Set the "language_override" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) language_override(std::string language_override);

        ///
        /// Return the current "language_override" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) language_override()
            const;

        ///
        /// Set the "partialFilterExpression" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) partial_filter_expression(
            bsoncxx::v1::document::value partial_filter_expression);

        ///
        /// Return the current "partialFilterExpression" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) partial_filter_expression()
            const;

        ///
        /// Set the "2dsphereIndexVersion" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) twod_sphere_version(std::uint8_t twod_sphere_version);

        ///
        /// Return the current "2dsphereIndexVersion" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::uint8_t>) twod_sphere_version() const;

        ///
        /// Set the "bits" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) twod_bits_precision(std::uint8_t twod_bits_precision);

        ///
        /// Return the current "bits" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::uint8_t>) twod_bits_precision() const;

        ///
        /// Set the "min" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) twod_location_min(double twod_location_min);

        ///
        /// Return the current "min" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<double>) twod_location_min() const;

        ///
        /// Set the "max" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(options&) twod_location_max(double twod_location_max);

        ///
        /// Return the current "max" field.
        ///
        MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<double>) twod_location_max() const;

        ///
        /// Return these index management options as a document.
        ///
        /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() operator bsoncxx::v1::document::value() const;
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
    indexes(indexes const& other);

    ///
    /// Copy assignment.
    ///
    indexes& operator=(indexes const& other);

    ///
    /// Return all indexes in the associated collection.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) list();

    ///
    /// Return all indexes in the associated collection.
    ///
    /// @param session The session with which this operation is associated.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) list(v1::client_session const& session);

    ///
    /// Equivalent to `this->create_one(model(keys, create_opts), opts)`.
    ///
    /// @param keys The "keys" field of the new index definition.
    /// @param create_opts "createIndexes" command options.
    /// @param opts Common index management options.
    ///
    bsoncxx::v1::stdx::optional<std::string> create_one(
        bsoncxx::v1::document::value keys,
        bsoncxx::v1::document::value create_opts = {},
        options const& opts = {}) {
        return this->create_one(model{std::move(keys), std::move(create_opts)}, opts);
    }

    ///
    /// Equivalent to `this->create_one(session, model(keys, create_opts), opts)`.
    ///
    /// @param session The session with which this operation is associated.
    /// @param keys The "keys" field of the new index definition.
    /// @param create_opts "createIndexes" command options.
    /// @param opts Common index management options.
    ///
    bsoncxx::v1::stdx::optional<std::string> create_one(
        v1::client_session const& session,
        bsoncxx::v1::document::value keys,
        bsoncxx::v1::document::value create_opts = {},
        options const& opts = {}) {
        return this->create_one(session, model{std::move(keys), std::move(create_opts)}, opts);
    }

    ///
    /// Create a single index in the associated collection.
    ///
    /// Equivalent to `this->create_many(...)` with a single index model.
    ///
    /// @param index The definition of the new index. The options must be applicable to the "createIndexes" command.
    /// @param opts Common index management options.
    ///
    /// @returns The name of the created index. Empty when the index already exists.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::string>) create_one(
        model const& index,
        options const& opts = {});

    ///
    /// Create a single index in the associated collection.
    ///
    /// Equivalent to `this->create_many(...)` with a single index model.
    ///
    /// @param session The session with which this operation is associated.
    /// @param index The definition of the new index. The options must be applicable to the "createIndexes" command.
    /// @param opts Common index management options.
    ///
    /// @returns The name of the created index. Empty when the index already exists.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::string>)
    create_one(v1::client_session const& session, model const& index, options const& opts = {});

    ///
    /// Create multiple indexes in the associated collection.
    ///
    /// @param indexes The definitions of the new indexes. The options must be applicable to the "createIndexes"
    /// command.
    /// @param opts Common index management options.
    ///
    /// @returns The raw server response.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::value) create_many(
        std::vector<model> const& indexes,
        options const& opts = {});

    ///
    /// Create multiple indexes in the associated collection.
    ///
    /// @param session The session with which this operation is associated.
    /// @param indexes The definitions of the new indexes. The options must be applicable to the "createIndexes"
    /// command.
    /// @param opts Common index management options.
    ///
    /// @returns The raw server response.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::value)
    create_many(v1::client_session const& session, std::vector<model> const& indexes, options const& opts = {});

    ///
    /// Equivalent to `this->drop_one(mongocxx::v1::indexes::model(keys, drop_opts), ...)`.
    ///
    /// @param keys The "keys" field of the new index definition.
    /// @param drop_opts "dropIndexes" command options.
    /// @param opts Common index management options.
    ///
    void
    drop_one(bsoncxx::v1::document::value keys, bsoncxx::v1::document::value drop_opts = {}, options const& opts = {}) {
        return this->drop_one(model{std::move(keys), std::move(drop_opts)}, opts);
    }

    ///
    /// Equivalent to `this->drop_one(session, mongocxx::v1::indexes::model(keys, drop_opts), ...)`.
    ///
    /// @param session The session with which this operation is associated.
    /// @param keys The "keys" field of the new index definition.
    /// @param drop_opts "dropIndexes" command options.
    /// @param opts Common index management options.
    ///
    void drop_one(
        client_session const& session,
        bsoncxx::v1::document::value keys,
        bsoncxx::v1::document::value drop_opts = {},
        options const& opts = {}) {
        return this->drop_one(session, model{std::move(keys), std::move(drop_opts)}, opts);
    }

    ///
    /// Drop a single index in the associated collection.
    ///
    /// @param index The name of index to drop. The options must be applicable to the "dropIndexes" command.
    /// @param opts Common index management options.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::indexes::errc::invalid_name if `name` is equal to "*".
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) drop_one(bsoncxx::v1::stdx::string_view index, options const& opts = {});

    ///
    /// Drop a single index in the associated collection.
    ///
    /// @param session The session with which this operation is associated.
    /// @param index The name of index to drop. The options must be applicable to the "dropIndexes" command.
    /// @param opts Common index management options.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::indexes::errc::invalid_name if `name` is equal to "*".
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    drop_one(v1::client_session const& session, bsoncxx::v1::stdx::string_view index, options const& opts = {});

    ///
    /// Drop a single index in the associated collection.
    ///
    /// @param index The description of the index to drop. The options must be applicable to the "dropIndexes" command.
    /// @param opts Common index management options.
    ///
    /// @throws mongocxx::v1::exception with @ref mongocxx::v1::indexes::errc::invalid_name if `name` is equal to "*".
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) drop_one(model const& index, options const& opts = {});

    ///
    /// Drop a single index in the associated collection.
    ///
    /// @param session The session with which this operation is associated.
    /// @param index The description of the index to drop.
    /// @param opts Common index management options.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    drop_one(v1::client_session const& session, model const& index, options const& opts = {});

    ///
    /// Drop all indexes in the associated collection.
    ///
    /// @param opts Common index management options.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) drop_all(options const& opts = {});

    ///
    /// Drop all indexes in the associated collection.
    ///
    /// @param session The session with which this operation is associated.
    /// @param opts Common index management options.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) drop_all(v1::client_session const& session, options const& opts = {});

    ///
    /// Errors codes which may be returned by @ref mongocxx::v1::indexes.
    ///
    /// @attention This feature is experimental! It is not ready for use!
    ///
    enum class errc {
        zero,         ///< Zero.
        invalid_name, ///< "*" is not a permitted index name.
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
