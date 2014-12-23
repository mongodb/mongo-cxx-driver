// Copyright 2014 MongoDB Inc.
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

#include "driver/config/prelude.hpp"

#include "bson/document/view.hpp"
#include "bson/document/value.hpp"

namespace bson {
namespace document {

class LIBMONGOCXX_EXPORT view_or_value {

   public:
    view_or_value(bson::document::view view);
    view_or_value(bson::document::value value);

    view_or_value(view_or_value&& rhs);
    view_or_value& operator=(view_or_value&& rhs);

    ~view_or_value();

    document::view view() const;
    operator document::view() const;

   private:
    view_or_value(const bson::document::view_or_value& view) = delete;
    view_or_value& operator=(const bson::document::view_or_value& view) = delete;

    bool _is_view;
    union {
        bson::document::view _view;
        bson::document::value _value;
    };

};

}  // namespace document
}  // namespace bson

#include "driver/config/postlude.hpp"
