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

#include <cstdint>

#include <mongocxx/model/write-fwd.hpp>

#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/model/delete_many.hpp>
#include <mongocxx/model/delete_one.hpp>
#include <mongocxx/model/insert_one.hpp>
#include <mongocxx/model/replace_one.hpp>
#include <mongocxx/model/update_many.hpp>
#include <mongocxx/model/update_one.hpp>
#include <mongocxx/write_type.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace model {

///
/// A single write operation for use with @ref mongocxx::v_noabi::bulk_write.
///
class write {
   public:
    ///
    /// Constructs a write from a model::insert_one.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() write(insert_one value);

    ///
    /// Constructs a write from a model::update_one.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() write(update_one value);

    ///
    /// Constructs a write from a model::update_many.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() write(update_many value);

    ///
    /// Constructs a write from a model::delete_one.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() write(delete_one value);

    ///
    /// Constructs a write from a model::delete_many.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() write(delete_many value);

    ///
    /// Constructs a write from a model::replace_one.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() write(replace_one value);

    ///
    /// Move constructs a write.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() write(write&& rhs) noexcept;

    ///
    /// Move assigns a write.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(write&) operator=(write&& rhs) noexcept;

    write(write const& rhs) = delete;
    write& operator=(write const& rhs) = delete;

    ///
    /// Destroys a write.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~write();

    ///
    /// Returns the current type of this write.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(write_type) type() const;

    ///
    /// Accesses the write as a model::insert_one. It is illegal to call
    /// this method if the return of type() does not indicate
    /// that this object currently contains the applicable type.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(insert_one const&) get_insert_one() const;

    ///
    /// Accesses the write as an model::update_one. It is illegal to call
    /// this method if the return of type() does not indicate
    /// that this object currently contains the applicable type.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_one const&) get_update_one() const;

    ///
    /// Accesses the write as an model::update_many. It is illegal to call
    /// this method if the return of type() does not indicate
    /// that this object currently contains the applicable type.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(update_many const&) get_update_many() const;

    ///
    /// Accesses the write as a model::delete_one. It is illegal to call
    /// this method if the return of type() does not indicate
    /// that this object currently contains the applicable type.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_one const&) get_delete_one() const;

    ///
    /// Accesses the write as a model::delete_many. It is illegal to call
    /// this method if the return of type() does not indicate
    /// that this object currently contains the applicable type.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(delete_many const&) get_delete_many() const;

    ///
    /// Accesses the write as a model::replace_one. It is illegal to call
    /// this method if the return of type() does not indicate
    /// that this object currently contains the applicable type.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(replace_one const&) get_replace_one() const;

   private:
    void destroy_member() noexcept;

    write_type _type;

    union {
        insert_one _insert_one;
        update_one _update_one;
        update_many _update_many;
        delete_one _delete_one;
        delete_many _delete_many;
        replace_one _replace_one;
    };
};

} // namespace model
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::model::write.
///
