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

#include <mongocxx/model/write-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/bulk_write.hpp> // IWYU pragma: export

#include <cstdint> // IWYU pragma: keep: backward compatibility, to be removed.
#include <utility>

#include <bsoncxx/stdx/optional.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

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
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() write(v1::bulk_write::single op);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    /// @warning Invalidates all associated references.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator v1::bulk_write::single() &&;

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator v1::bulk_write::single() const&;

    ///
    /// Constructs a write from a model::insert_one.
    ///
    write(insert_one value) : _type{v_noabi::write_type::k_insert_one}, _insert_one{std::move(value)} {}

    ///
    /// Constructs a write from a model::update_one.
    ///
    write(update_one value) : _type{v_noabi::write_type::k_update_one}, _update_one{std::move(value)} {}

    ///
    /// Constructs a write from a model::update_many.
    ///
    write(update_many value) : _type{v_noabi::write_type::k_update_many}, _update_many{std::move(value)} {}

    ///
    /// Constructs a write from a model::delete_one.
    ///
    write(delete_one value) : _type{v_noabi::write_type::k_delete_one}, _delete_one{std::move(value)} {}

    ///
    /// Constructs a write from a model::delete_many.
    ///
    write(delete_many value) : _type{v_noabi::write_type::k_delete_many}, _delete_many{std::move(value)} {}

    ///
    /// Constructs a write from a model::replace_one.
    ///
    write(replace_one value) : _type{v_noabi::write_type::k_replace_one}, _replace_one{std::move(value)} {}

    ///
    /// Move constructs a write.
    ///
    write(write&& rhs) noexcept : _type{rhs._type} {
        switch (rhs._type) {
            case write_type::k_insert_one:
                new (&_insert_one) insert_one(std::move(rhs._insert_one));
                break;
            case write_type::k_update_one:
                new (&_update_one) update_one(std::move(rhs._update_one));
                break;
            case write_type::k_update_many:
                new (&_update_many) update_many(std::move(rhs._update_many));
                break;
            case write_type::k_delete_one:
                new (&_delete_one) delete_one(std::move(rhs._delete_one));
                break;
            case write_type::k_delete_many:
                new (&_delete_many) delete_many(std::move(rhs._delete_many));
                break;
            case write_type::k_replace_one:
                new (&_replace_one) replace_one(std::move(rhs._replace_one));
                break;
        }
    }

    ///
    /// Move assigns a write.
    ///
    write& operator=(write&& rhs) noexcept {
        if (this != &rhs) {
            this->destroy_member();

            switch (rhs._type) {
                case write_type::k_insert_one:
                    new (&_insert_one) insert_one(std::move(rhs._insert_one));
                    break;
                case write_type::k_update_one:
                    new (&_update_one) update_one(std::move(rhs._update_one));
                    break;
                case write_type::k_update_many:
                    new (&_update_many) update_many(std::move(rhs._update_many));
                    break;
                case write_type::k_delete_one:
                    new (&_delete_one) delete_one(std::move(rhs._delete_one));
                    break;
                case write_type::k_delete_many:
                    new (&_delete_many) delete_many(std::move(rhs._delete_many));
                    break;
                case write_type::k_replace_one:
                    new (&_replace_one) replace_one(std::move(rhs._replace_one));
                    break;
            }

            _type = rhs._type;
        }

        return *this;
    }

    write(write const& rhs) = delete;
    write& operator=(write const& rhs) = delete;

    ///
    /// Destroys a write.
    ///
    ~write() {
        this->destroy_member();
    }

    ///
    /// Returns the current type of this write.
    ///
    write_type type() const {
        return _type;
    }

    ///
    /// Accesses the write as a model::insert_one. It is illegal to call
    /// this method if the return of type() does not indicate
    /// that this object currently contains the applicable type.
    ///
    insert_one const& get_insert_one() const {
        return _insert_one;
    }

    ///
    /// Accesses the write as an model::update_one. It is illegal to call
    /// this method if the return of type() does not indicate
    /// that this object currently contains the applicable type.
    ///
    update_one const& get_update_one() const {
        return _update_one;
    }

    ///
    /// Accesses the write as an model::update_many. It is illegal to call
    /// this method if the return of type() does not indicate
    /// that this object currently contains the applicable type.
    ///
    update_many const& get_update_many() const {
        return _update_many;
    }

    ///
    /// Accesses the write as a model::delete_one. It is illegal to call
    /// this method if the return of type() does not indicate
    /// that this object currently contains the applicable type.
    ///
    delete_one const& get_delete_one() const {
        return _delete_one;
    }

    ///
    /// Accesses the write as a model::delete_many. It is illegal to call
    /// this method if the return of type() does not indicate
    /// that this object currently contains the applicable type.
    ///
    delete_many const& get_delete_many() const {
        return _delete_many;
    }

    ///
    /// Accesses the write as a model::replace_one. It is illegal to call
    /// this method if the return of type() does not indicate
    /// that this object currently contains the applicable type.
    ///
    replace_one const& get_replace_one() const {
        return _replace_one;
    }

   private:
    void destroy_member() noexcept {
        switch (_type) {
            case write_type::k_insert_one:
                _insert_one.~insert_one();
                break;
            case write_type::k_update_one:
                _update_one.~update_one();
                break;
            case write_type::k_update_many:
                _update_many.~update_many();
                break;
            case write_type::k_delete_one:
                _delete_one.~delete_one();
                break;
            case write_type::k_delete_many:
                _delete_many.~delete_many();
                break;
            case write_type::k_replace_one:
                _replace_one.~replace_one();
                break;
        }
    }

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

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::model::write from_v1(v1::bulk_write::single v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::bulk_write::single to_v1(v_noabi::model::write const& v) {
    return v1::bulk_write::single{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::model::write.
///
/// @par Includes
/// - @ref mongocxx/v1/bulk_write.hpp
///
