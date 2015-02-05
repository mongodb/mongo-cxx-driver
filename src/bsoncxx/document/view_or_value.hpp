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

#include <bsoncxx/config/prelude.hpp>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/value.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace document {

class LIBBSONCXX_API view_or_value {

   public:
    view_or_value(bsoncxx::document::view view);
    view_or_value(bsoncxx::document::value value);

    view_or_value(view_or_value&& rhs);
    view_or_value& operator=(view_or_value&& rhs);

    ~view_or_value();

    document::view view() const;
    operator document::view() const;

   private:
    view_or_value(const bsoncxx::document::view_or_value& view) = delete;
    view_or_value& operator=(const bsoncxx::document::view_or_value& view) = delete;

    bool _is_view;
    union {
        bsoncxx::document::view _view;
        bsoncxx::document::value _value;
    };

};

}  // namespace document
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
