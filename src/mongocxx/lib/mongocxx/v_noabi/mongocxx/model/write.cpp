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

#include <mongocxx/model/write.hpp>

//

#include <mongocxx/v1/detail/macros.hpp>

#include <utility>

#include <mongocxx/model/delete_many.hpp>
#include <mongocxx/model/delete_one.hpp>
#include <mongocxx/model/insert_one.hpp>
#include <mongocxx/model/replace_one.hpp>
#include <mongocxx/model/update_many.hpp>
#include <mongocxx/model/update_one.hpp>
#include <mongocxx/write_type.hpp>

#include <bsoncxx/private/type_traits.hh>

namespace mongocxx {
namespace v_noabi {
namespace model {

static_assert(
    bsoncxx::is_nothrow_moveable<v_noabi::model::insert_one>::value,
    "mongocxx::v_noabi::model::insert_one must be nothrow moveable");

static_assert(
    bsoncxx::is_nothrow_moveable<v_noabi::model::delete_one>::value,
    "mongocxx::v_noabi::model::delete_one must be nothrow moveable");

static_assert(
    bsoncxx::is_nothrow_moveable<v_noabi::model::delete_many>::value,
    "mongocxx::v_noabi::model::delete_many must be nothrow moveable");

static_assert(
    bsoncxx::is_nothrow_moveable<v_noabi::model::update_one>::value,
    "mongocxx::v_noabi::model::update_one must be nothrow moveable");

static_assert(
    bsoncxx::is_nothrow_moveable<v_noabi::model::update_many>::value,
    "mongocxx::v_noabi::model::update_many must be nothrow moveable");

static_assert(
    bsoncxx::is_nothrow_moveable<replace_one>::value,
    "mongocxx::v_noabi::model::v_noabi::model::replace_one must be nothrow moveable");

write::write(v1::bulk_write::single op) : _type{static_cast<v_noabi::write_type>(op.type())} {
    switch (_type) {
        case v_noabi::write_type::k_insert_one:
            new (&_insert_one) v_noabi::model::insert_one{std::move(op).get_insert_one()};
            break;
        case v_noabi::write_type::k_delete_one:
            new (&_delete_one) v_noabi::model::delete_one{std::move(op).get_delete_one()};
            break;
        case v_noabi::write_type::k_delete_many:
            new (&_delete_many) v_noabi::model::delete_many{std::move(op).get_delete_many()};
            break;
        case v_noabi::write_type::k_update_one:
            new (&_update_one) v_noabi::model::update_one{std::move(op).get_update_one()};
            break;
        case v_noabi::write_type::k_update_many:
            new (&_update_many) v_noabi::model::update_many{std::move(op).get_update_many()};
            break;
        case v_noabi::write_type::k_replace_one:
            new (&_replace_one) v_noabi::model::replace_one{std::move(op).get_replace_one()};
            break;
        default:
            MONGOCXX_PRIVATE_UNREACHABLE;
    }
}

write::operator v1::bulk_write::single() && {
    switch (_type) {
        case v_noabi::write_type::k_insert_one:
            return to_v1(_insert_one);
        case v_noabi::write_type::k_delete_one:
            return to_v1(_delete_one);
        case v_noabi::write_type::k_delete_many:
            return to_v1(_delete_many);
        case v_noabi::write_type::k_update_one:
            return to_v1(_update_one);
        case v_noabi::write_type::k_update_many:
            return to_v1(_update_many);
        case v_noabi::write_type::k_replace_one:
            return to_v1(_replace_one);
        default:
            MONGOCXX_PRIVATE_UNREACHABLE;
    }
}

write::operator v1::bulk_write::single() const& {
    switch (_type) {
        case v_noabi::write_type::k_insert_one:
            return to_v1(_insert_one);
        case v_noabi::write_type::k_delete_one:
            return to_v1(_delete_one);
        case v_noabi::write_type::k_delete_many:
            return to_v1(_delete_many);
        case v_noabi::write_type::k_update_one:
            return to_v1(_update_one);
        case v_noabi::write_type::k_update_many:
            return to_v1(_update_many);
        case v_noabi::write_type::k_replace_one:
            return to_v1(_replace_one);
        default:
            MONGOCXX_PRIVATE_UNREACHABLE;
    }
}

} // namespace model
} // namespace v_noabi
} // namespace mongocxx
