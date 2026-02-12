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

#include <mongocxx/index_view-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/indexes.hpp> // IWYU pragma: export

#include <string>
#include <utility>
#include <vector>

#include <mongocxx/collection-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/client_session.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/index_model.hpp>
#include <mongocxx/options/index_view.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// A MongoDB index.
///
/// @note Not to be confused with a MongoDB Atlas Search Index (@ref
/// mongocxx::v_noabi::search_index_view).
///
class index_view {
   private:
    v1::indexes _indexes;

   public:
    index_view(index_view&& other) noexcept = default;
    index_view& operator=(index_view&& other) noexcept = default;

    ~index_view() = default;

    index_view(index_view const& other) = delete;
    index_view& operator=(index_view const& other) = delete;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ index_view(v1::indexes indexes) : _indexes{std::move(indexes)} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    /// @warning Invalidates all associated objects.
    ///
    explicit operator v1::indexes() && {
        return std::move(_indexes);
    }

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::indexes() const& {
        return _indexes;
    }

    ///
    /// Returns a cursor over all the indexes.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::cursor) list();

    ///
    /// Returns a cursor over all the indexes.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the list operation.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v_noabi::cursor) list(v_noabi::client_session const& session);

    ///
    /// Creates an index. A convenience method that calls create_many.
    ///
    /// @param keys
    ///    A document containing the index keys and their corresponding index types.
    /// @param index_options
    ///    A document containing set of options that controls the creation of the index. See
    ///    https://www.mongodb.com/docs/manual/reference/method/db.collection.createIndex/.
    /// @param options
    ///    Optional arguments for the overall operation, see mongocxx::v_noabi::options::index_view.
    ///
    /// @return
    ///    An optional containing the name of the created index. If and index with the same keys
    ///    already exists, an empty optional is returned.
    ///
    /// @exception
    ///    Throws operation_exception for any errors encountered by the server or if max_time_ms
    ///    option is present and the operation exceeds the time limit.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/db.collection.createIndex/
    ///
    bsoncxx::v_noabi::stdx::optional<std::string> create_one(
        bsoncxx::v_noabi::document::view_or_value const& keys,
        bsoncxx::v_noabi::document::view_or_value const& index_options = {},
        v_noabi::options::index_view const& options = {}) {
        return this->create_one(v_noabi::index_model{keys, index_options}, options);
    }

    ///
    /// Creates an index. A convenience method that calls create_many.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the operation.
    /// @param keys
    ///    A document containing the index keys and their corresponding index types.
    /// @param index_options
    ///    A document containing set of options that controls the creation of the index. See
    ///    https://www.mongodb.com/docs/manual/reference/method/db.collection.createIndex/.
    /// @param options
    ///    Optional arguments for the overall operation, see mongocxx::v_noabi::options::index_view.
    ///
    /// @return
    ///    An optional containing the name of the created index. If and index with the same keys
    ///    already exists, an empty optional is returned.
    ///
    /// @exception
    ///    Throws operation_exception for any errors encountered by the server or if max_time_ms
    ///    option is present and the operation exceeds the time limit.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/db.collection.createIndex/
    ///
    bsoncxx::v_noabi::stdx::optional<std::string> create_one(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value const& keys,
        bsoncxx::v_noabi::document::view_or_value const& index_options = {},
        v_noabi::options::index_view const& options = {}) {
        return this->create_one(session, v_noabi::index_model{keys, index_options}, options);
    }

    ///
    /// Creates an index. A convenience method that calls create_many.
    ///
    /// @param index
    ///    Index_model describing the index being created.
    /// @param options
    ///    Optional arguments for the overall operation, see mongocxx::v_noabi::options::index_view.
    ///
    /// @return
    ///    An optional containing the name of the created index. If and index with the same keys
    ///    already exists, an empty optional is returned.
    ///
    /// @exception
    ///    Throws operation_exception for any errors encountered by the server or if max_time_ms
    ///    option is present and the operation exceeds the time limit.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/cursor.maxTimeMS/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::string>)
    create_one(v_noabi::index_model const& index, v_noabi::options::index_view const& options = {});

    ///
    /// Creates an index. A convenience method that calls create_many.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the operation.
    /// @param index
    ///    Index_model describing the index being created.
    /// @param options
    ///    Optional arguments for the overall operation, see mongocxx::v_noabi::options::index_view.
    ///
    /// @return
    ///    An optional containing the name of the created index. If and index with the same keys
    ///    already exists, an empty optional is returned.
    ///
    /// @exception
    ///    Throws operation_exception for any errors encountered by the server or if max_time_ms
    ///    option is present and the operation exceeds the time limit.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/cursor.maxTimeMS/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::string>)
    create_one(
        v_noabi::client_session const& session,
        index_model const& index,
        v_noabi::options::index_view const& options = {});

    ///
    /// Adds a container of indexes to the collection.
    ///
    /// @param indexes
    ///   std::vector containing index models describing the indexes being created.
    /// @param options
    ///    Optional arguments for the overall operation, see mongocxx::v_noabi::options::index_view.
    ///
    /// @return
    ///    The result document sent back by the server as if the createIndexes command was run from
    ///    the shell.
    ///
    /// @exception
    ///     Throws operation_exception for any errors encountered by the server or if max_time_ms
    ///     option is present and the operation exceeds the time limit.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/cursor.maxTimeMS/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::value)
    create_many(std::vector<v_noabi::index_model> const& indexes, v_noabi::options::index_view const& options = {});

    ///
    /// Adds a container of indexes to the collection.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the operation.
    /// @param indexes
    ///   std::vector containing index models describing the indexes being created.
    /// @param options
    ///    Optional arguments for the overall operation, see mongocxx::v_noabi::options::index_view.
    ///
    /// @return
    ///    The result document sent back by the server as if the createIndexes command was run from
    ///    the shell.
    ///
    /// @exception
    ///     Throws operation_exception for any errors encountered by the server or if max_time_ms
    ///     option is present and the operation exceeds the time limit.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/cursor.maxTimeMS/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::value)
    create_many(
        v_noabi::client_session const& session,
        std::vector<v_noabi::index_model> const& indexes,
        v_noabi::options::index_view const& options = {});

    ///
    /// Drops a single index by name.
    ///
    /// @param name
    ///    The name of the index being dropped.
    /// @param options
    ///    Optional arguments for the overall operation, see mongocxx::v_noabi::options::index_view.
    ///
    /// @exception
    ///   Throws operation_exception for any errors encountered by the server or if max_time_ms
    ///   option is present and the operation exceeds the time limit.
    /// @exception
    ///   Throws logic_error if "*" is passed in for the index name.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/cursor.maxTimeMS/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    drop_one(bsoncxx::v_noabi::stdx::string_view name, v_noabi::options::index_view const& options = {});

    ///
    /// Drops a single index by name.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the drop.
    /// @param name
    ///    The name of the index being dropped.
    /// @param options
    ///    Optional arguments for the overall operation, see mongocxx::v_noabi::options::index_view.
    ///
    /// @exception
    ///   Throws operation_exception for any errors encountered by the server or if max_time_ms
    ///   option is present and the operation exceeds the time limit.
    /// @exception
    ///   Throws logic_error if "*" is passed in for the index name.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/cursor.maxTimeMS/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    drop_one(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::stdx::string_view name,
        v_noabi::options::index_view const& options = {});

    ///
    /// Attempts to drop a single index from the collection given the keys and options.
    ///
    /// @param keys
    ///    A document containing the index keys and their corresponding index types. If no name
    ///    option is present in the options, a name based on the keys will be used.
    /// @param index_options (optional)
    ///    A document containing set of options used to create the index. Only the name field will
    ///    be used from here, and if it is not included, a name based on they keys will be used.
    /// @param options
    ///    Optional arguments for the overall operation, see mongocxx::v_noabi::options::index_view.
    ///
    /// @exception
    ///   Throws bsoncxx::v_noabi::exception if "name" key is present in options but is not a
    ///   string.
    /// @exception
    ///   Throws operation_exception for any errors encountered by the server or if max_time_ms
    ///   option is present and the operation exceeds the time limit.
    /// @exception
    ///   Throws logic_error if "*" is passed in for the index name
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/cursor.maxTimeMS/
    ///
    void drop_one(
        bsoncxx::v_noabi::document::view_or_value const& keys,
        bsoncxx::v_noabi::document::view_or_value const& index_options = {},
        v_noabi::options::index_view const& options = {}) {
        return this->drop_one(v_noabi::index_model{std::move(keys), std::move(index_options)}, options);
    }

    ///
    /// Attempts to drop a single index from the collection given the keys and options.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the drop.
    /// @param keys
    ///    A document containing the index keys and their corresponding index types. If no name
    ///    option is present in the options, a name based on the keys will be used.
    /// @param index_options (optional)
    ///    A document containing set of options used to create the index. Only the name field will
    ///    be used from here, and if it is not included, a name based on they keys will be used.
    /// @param options
    ///    Optional arguments for the overall operation, see mongocxx::v_noabi::options::index_view.
    ///
    /// @exception
    ///   Throws bsoncxx::v_noabi::exception if "name" key is present in options but is not a
    ///   string.
    /// @exception
    ///   Throws operation_exception for any errors encountered by the server or if max_time_ms
    ///   option is present and the operation exceeds the time limit.
    /// @exception
    ///   Throws logic_error if "*" is passed in for the index name
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/cursor.maxTimeMS/
    ///
    void drop_one(
        v_noabi::client_session const& session,
        bsoncxx::v_noabi::document::view_or_value const& keys,
        bsoncxx::v_noabi::document::view_or_value const& index_options = {},
        v_noabi::options::index_view const& options = {}) {
        return this->drop_one(session, v_noabi::index_model{std::move(keys), std::move(index_options)}, options);
    }

    ///
    /// Attempts to drop a single index from the collection given an index model.
    ///
    /// @param index
    ///    An index model describing the index being dropped.
    /// @param options
    ///    Optional arguments for the overall operation, see mongocxx::v_noabi::options::index_view.
    ///
    /// @exception
    ///   Throws bsoncxx::v_noabi::exception if "name" key is present in options but is not a
    ///   string.
    /// @exception
    ///   Throws operation_exception for any errors encountered by the server or if max_time_ms
    ///   option is present and the operation exceeds the time limit.
    /// @exception
    ///   Throws logic_error if "*" is passed in for the index name
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/cursor.maxTimeMS/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    drop_one(v_noabi::index_model const& index, v_noabi::options::index_view const& options = {});

    ///
    /// Attempts to drop a single index from the collection given an index model.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the drop.
    /// @param index
    ///    An index model describing the index being dropped.
    /// @param options
    ///    Optional arguments for the overall operation, see mongocxx::v_noabi::options::index_view.
    ///
    /// @exception
    ///   Throws bsoncxx::v_noabi::exception if "name" key is present in options but is not a
    ///   string.
    /// @exception
    ///   Throws operation_exception for any errors encountered by the server or if max_time_ms
    ///   option is present and the operation exceeds the time limit.
    /// @exception
    ///   Throws logic_error if "*" is passed in for the index name
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/cursor.maxTimeMS/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    drop_one(client_session const& session, index_model const& index, v_noabi::options::index_view const& options = {});

    ///
    /// Drops all indexes in the collection.
    ///
    /// @param options
    ///    Optional arguments for the overall operation, see mongocxx::v_noabi::options::index_view.
    ///
    /// @exception
    ///   Throws operation_exception for any errors encountered by the server or if max_time_ms
    ///   option is present and the operation exceeds the time limit.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/cursor.maxTimeMS/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    drop_all(options::index_view const& options = {});

    ///
    /// Drops all indexes in the collection.
    ///
    /// @param session
    ///   The mongocxx::v_noabi::client_session with which to perform the drop.
    /// @param options
    ///    Optional arguments for the overall operation, see mongocxx::v_noabi::options::index_view.
    ///
    /// @exception
    ///   Throws operation_exception for any errors encountered by the server or if max_time_ms
    ///   option is present and the operation exceeds the time limit.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/cursor.maxTimeMS/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void)
    drop_all(v_noabi::client_session const& session, v_noabi::options::index_view const& options = {});
};

} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::index_view from_v1(v1::indexes v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::indexes to_v1(v_noabi::index_view v) {
    return v1::indexes{std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::index_view.
///
/// @par Includes
/// - @ref mongocxx/v1/indexes.hpp
///
