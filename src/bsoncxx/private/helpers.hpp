// Copyright 2015 MongoDB Inc.
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

#include <bson.h>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>

#include <bsoncxx/config/private/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

namespace helpers {

inline document::view view_from_bson_t(const bson_t* bson) {
    return {::bson_get_data(bson), bson->len};
}

inline document::value value_from_bson_t(const bson_t* bson) {
    return document::value{view_from_bson_t(bson)};
}

}  // namespace helpers
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/private/postlude.hpp>
