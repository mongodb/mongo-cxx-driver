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

#include <bsoncxx/array/value.hpp>
#include <bsoncxx/builder/core.hpp>
#include <bsoncxx/builder/stream/closed_context.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/util/functor.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {
namespace stream {

 template <class T>
 class key_context;

 class single_context;

template <class base = closed_context>
class array_context {
   public:
    array_context(core* core) : _core(core) {}

    template <class T>
    typename std::enable_if<!(util::is_functor<T, void(array_context<>)>::value || util::is_functor<T, void(single_context)>::value || std::is_same<T, const close_document_type>::value || std::is_same<typename std::remove_reference<T>::type, const finalize_type>::value), array_context>::type& operator<<(
        T&& t) {
        _core->append(std::forward<T>(t));
        return *this;
    }

    template <typename Func>
    typename std::enable_if<(util::is_functor<Func, void(array_context<>)>::value || util::is_functor<Func, void(single_context)>::value), array_context>::type& operator<<(
        Func func) {
        func(*this);
        return *this;
    }

    template <typename T>
    typename std::enable_if<std::is_same<base, closed_context>::value &&
                            std::is_same<typename std::remove_reference<T>::type, const finalize_type>::value,
                            array::value>::type
    operator<<(T&&) {
        return _core->extract_array();
    }

    key_context<array_context> operator<<(const open_document_type) {
        _core->open_document();
        return wrap_document();
    }

    array_context operator<<(concatenate concatenate) {
        _core->concatenate(concatenate);
        return *this;
    }

    array_context<array_context> operator<<(const open_array_type) {
        _core->open_array();
        return wrap_array();
    }

    base operator<<(const close_array_type) {
        _core->close_array();
        return unwrap();
    }

    operator array_context<>() { return array_context<>(_core); }

    operator single_context();

   private:
    base unwrap() { return base(_core); }
    array_context<array_context> wrap_array() { return array_context<array_context>(_core); }
    key_context<array_context> wrap_document() { return key_context<array_context>(_core); }

    core* _core;
};

}  // namespace stream
}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
