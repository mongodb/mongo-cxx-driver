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

#include <bsoncxx/builder/core.hpp>
#include <bsoncxx/config/prelude.hpp>
#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/types/bson_value/value.hpp>
#include <sstream>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {
using namespace bsoncxx::types;

class list {
    using initializer_list_t = std::initializer_list<list>;

   public:
    list() : list({}) {}

    template <typename T>
    list(T value) : val{value} {}

    list(initializer_list_t init) : list(init, true, true) {}

    operator bson_value::value() {
        return value();
    }

    operator bson_value::view() {
        return view();
    }

    bson_value::view view() {
        return val.view();
    }

    bson_value::value value() {
        return val;
    }

    class array {
       public:
        array() = default;
        array(initializer_list_t init) : lst(init) {}
        operator list() {
            return list(lst, false, true);
        }

       private:
        initializer_list_t lst;
    };

    class document {
       public:
        document() = default;
        document(initializer_list_t init) : lst(init) {}
        operator list() {
            return list(lst, false, false);
        }

       private:
        initializer_list_t lst;
    };

   private:
    bson_value::value val;

    list(initializer_list_t init, bool type_deduction, bool is_array) : val{nullptr} {
        std::stringstream err_msg{"cannot construct document"};
        bool valid_document = false;
        if (type_deduction || !is_array) {
            valid_document = [&] {
                if (init.size() % 2 != 0) {
                    err_msg << " : must be list of key-value pairs";
                    return false;
                }
                for (size_t i = 0; i < init.size(); i += 2) {
                    auto t = (begin(init) + i)->val.view().type();
                    if (t != type::k_utf8) {
                        err_msg << " : all keys must be string type. ";
                        err_msg << "Found type=" << to_string(t);
                        return false;
                    }
                }
                return true;
            }();
        }

        if (valid_document) {
            core _core{false};
            for (size_t i = 0; i < init.size(); i += 2) {
                _core.key_owned(std::string((begin(init) + i)->val.view().get_string().value));
                _core.append((begin(init) + i + 1)->val);
            }
            val = bson_value::value(_core.extract_document());
        } else if (type_deduction || is_array) {
            core _core{true};
            for (auto&& ele : init)
                _core.append(ele.val);
            val = bson_value::value(_core.extract_array());
        } else {
            throw bsoncxx::exception{error_code::k_unmatched_key_in_builder, err_msg.str()};
        }
    }
};
}
BSONCXX_INLINE_NAMESPACE_END
}
