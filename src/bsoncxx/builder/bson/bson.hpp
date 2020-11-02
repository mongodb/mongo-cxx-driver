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
        bool is_document = [&] {
            if (init.size() % 2 != 0)
                return false;
            for (size_t i = 0; i < init.size(); i += 2)
                if ((begin(init) + i)->_value.view().type() != type::k_utf8)
                    return false;
            return true;
        }();

        core _core{!is_document};
        if (is_document) {
            for (size_t i = 0; i < init.size(); i += 2) {
                _core.key_owned(std::string((begin(init) + i)->_value.view().get_string().value));
                _core.append((begin(init) + i + 1)->_value);
            }
            _value = bson_value::value(_core.extract_document());
        } else {
            for (auto&& ele : init)
                _core.append(ele._value);
            _value = bson_value::value(_core.extract_array());
        }
    }

    operator bson_value::value() {
        return value();
    }

    operator bson_value::view() {
        return view();
    }

    bson_value::view view() {
        return _value.view();
    }

    bson_value::value value() {
        return _value;
    }

   private:
    bson_value::value _value{nullptr};
};
}
BSONCXX_INLINE_NAMESPACE_END
}
