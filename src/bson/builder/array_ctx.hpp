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
class array_ctx {
   public:
    array_ctx(concrete* concrete) : _concrete(concrete) {}

    template <class T>
    typename std::enable_if<!(util::is_functor<T, void(array_ctx<>)>::value || util::is_functor<T, void(single_ctx)>::value || std::is_same<T, builder::helpers::close_doc_t>::value), array_ctx>::type& operator<<(
        T&& t) {
        _concrete->value_append(std::forward<T>(t));
        return *this;
    }

    template <typename Func>
    typename std::enable_if<(util::is_functor<Func, void(array_ctx<>)>::value || util::is_functor<Func, void(single_ctx)>::value), array_ctx>::type& operator<<(
        Func func) {
        func(*this);
        return *this;
    }

    key_ctx<array_ctx> operator<<(builder::helpers::open_doc_t) {
        _concrete->open_doc_append();
        return wrap_document();
    }

    array_ctx operator<<(builder::helpers::concat concat) {
        _concrete->concat_append(concat);
        return *this;
    }

    array_ctx<array_ctx> operator<<(builder::helpers::open_array_t) {
        _concrete->open_array_append();
        return wrap_array();
    }

    Base operator<<(builder::helpers::close_array_t) {
        _concrete->close_array_append();
        return unwrap();
    }

    operator array_ctx<>() { return array_ctx<>(_concrete); }

    operator single_ctx();

   private:
    Base unwrap() { return Base(_concrete); }
    array_ctx<array_ctx> wrap_array() { return array_ctx<array_ctx>(_concrete); }
    key_ctx<array_ctx> wrap_document() { return key_ctx<array_ctx>(_concrete); }

    concrete* _concrete;
};

}  // namespace builder
}  // namespace bson

#include "driver/config/postlude.hpp"
