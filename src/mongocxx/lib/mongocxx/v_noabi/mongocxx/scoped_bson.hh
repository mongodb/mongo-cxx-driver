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

#include <mongocxx/private/scoped_bson.hh> // IWYU pragma: export

//

#include <bsoncxx/array/view_or_value.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>

namespace mongocxx {

inline scoped_bson_view to_scoped_bson_view(bsoncxx::v_noabi::builder::basic::document const& v) {
    return {bsoncxx::v_noabi::to_v1(v.view())};
}

inline scoped_bson_view to_scoped_bson_view(bsoncxx::v_noabi::document::view const& v) {
    return {bsoncxx::v_noabi::to_v1(v)};
}

inline scoped_bson_view to_scoped_bson_view(bsoncxx::v_noabi::document::value const& v) {
    return {bsoncxx::v_noabi::to_v1(v.view())};
}

inline scoped_bson_view to_scoped_bson_view(bsoncxx::v_noabi::document::view_or_value const& v) {
    return {bsoncxx::v_noabi::to_v1(v.view())};
}

inline scoped_bson_view to_scoped_bson_view(bsoncxx::v_noabi::array::view const& v) {
    return {bsoncxx::v1::document::view{bsoncxx::v_noabi::to_v1(v)}};
}

inline scoped_bson_view to_scoped_bson_view(bsoncxx::v_noabi::array::value const& v) {
    return {bsoncxx::v1::document::view{bsoncxx::v_noabi::to_v1(v.view())}};
}

inline scoped_bson_view to_scoped_bson_view(bsoncxx::v_noabi::array::view_or_value const& v) {
    return {bsoncxx::v1::document::view{bsoncxx::v_noabi::to_v1(v.view())}};
}

inline scoped_bson to_scoped_bson(bsoncxx::v_noabi::document::value v) {
    return scoped_bson{bsoncxx::v_noabi::to_v1(std::move(v))};
}

inline bsoncxx::v_noabi::document::value from_v1(scoped_bson&& v) {
    return bsoncxx::v_noabi::from_v1(std::move(v).value());
}

} // namespace mongocxx
