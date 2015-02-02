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

#include "bson/builder/concrete.hpp"
#include "bson/builder/closed_ctx.hpp"
#include "bson/builder/helpers.hpp"
#include "bson/builder/forward_decls.hpp"
#include "bson/util/functor.hpp"

namespace bson {
namespace builder {

template <class Base = closed_ctx>
class value_ctx {
   public:
    value_ctx(concrete* concrete) : _concrete(concrete) {}

    template <class T>
    typename std::enable_if<!util::is_functor<T, void(single_ctx)>::value, Base>::type operator<<(
        T&& t) {
        _concrete->value_append(std::forward<T>(t));
        return unwrap();
    }

    template <typename Func>
    typename std::enable_if<util::is_functor<Func, void(single_ctx)>::value, Base>::type operator<<(
        Func func) {
        func(*this);
        return unwrap();
    }

    key_ctx<Base> operator<<(builder::helpers::open_doc_t) {
        _concrete->open_doc_append();
        return wrap_document();
    }

    array_ctx<Base> operator<<(builder::helpers::open_array_t) {
        _concrete->open_array_append();
        return wrap_array();
    }

    operator single_ctx();

   private:
    Base unwrap() { return Base(_concrete); }
    array_ctx<Base> wrap_array() { return array_ctx<Base>(_concrete); }
    key_ctx<Base> wrap_document() { return key_ctx<Base>(_concrete); }

    concrete* _concrete;
};

}  // namespace builder
}  // namespace bson

#include "driver/config/postlude.hpp"
