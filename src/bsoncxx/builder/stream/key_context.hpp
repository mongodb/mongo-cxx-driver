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

#include <bsoncxx/builder/core.hpp>
#include <bsoncxx/builder/stream/closed_context.hpp>
#include <bsoncxx/builder/stream/value_context.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/util/functor.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {
namespace stream {

///
/// An internal class of builder::stream. Users should not use this directly.
///
template <class base = closed_context>
class key_context {
   public:
    key_context(core* core) : _core(core) {
    }

    template <std::size_t n>
    BSONCXX_INLINE value_context<key_context> operator<<(const char(&v)[n]) {
        _core->key_view(stdx::string_view{v, n - 1});
        return value_context<key_context>(_core);
    }

    BSONCXX_INLINE value_context<key_context> operator<<(std::string str) {
        _core->key_owned(std::move(str));
        return value_context<key_context>(_core);
    }

    BSONCXX_INLINE value_context<key_context> operator<<(stdx::string_view str) {
        _core->key_view(std::move(str));
        return value_context<key_context>(_core);
    }

    template <typename T>
    BSONCXX_INLINE
        typename std::enable_if<util::is_functor<T, void(key_context<>)>::value, key_context>::type&
        operator<<(T&& func) {
        func(*this);
        return *this;
    }

    template <typename T>
    BSONCXX_INLINE typename std::enable_if<
        std::is_same<base, closed_context>::value &&
            std::is_same<typename std::remove_reference<T>::type, const finalize_type>::value,
        document::value>::type
    operator<<(T&&) {
        return _core->extract_document();
    }

    BSONCXX_INLINE key_context operator<<(concatenate_doc doc) {
        _core->concatenate(doc);
        return *this;
    }

    BSONCXX_INLINE base operator<<(const close_document_type) {
        _core->close_document();
        return unwrap();
    }

    BSONCXX_INLINE operator key_context<>() {
        return key_context<>(_core);
    }

   private:
    BSONCXX_INLINE base unwrap() {
        return base(_core);
    }

    core* _core;
};

}  // namespace stream
}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
