// Copyright 2020 MongoDB Inc.
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
#include <bsoncxx/types/bson_value/value.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {
using namespace bsoncxx::types;

class bson {
   public:
    template <typename T>
    bson(T value) : _value{value} {}

    bson(std::initializer_list<bson> init) {
        size_t counter = 0;
        bool is_document =
            std::all_of(begin(init),
                        end(init),
                        [&](const bson ele) {
                            return counter++ % 2 != 0 || ele._value.view().type() == type::k_utf8;
                        }) &&
            counter % 2 == 0;

        core _core{!is_document};
        if (is_document) {
            bool is_key = true;
            for (auto ele = begin(init); ele != end(init); ele++, is_key = !is_key) {
                if (is_key) {
                    auto key = std::string(ele->_value.view().get_string().value);
                    _core.key_owned(key);
                } else {
                    _core.append(ele->_value);
                }
            };
            _value = bson_value::value(_core.extract_document());
        } else {
            for (auto&& ele : init)
                _core.append(ele._value);
            _value = bson_value::value(_core.extract_array());
        }
    }

    operator bson_value::value() {
        return _value;
    }

   private:
    bson_value::value _value{nullptr};
};
}
BSONCXX_INLINE_NAMESPACE_END
}
