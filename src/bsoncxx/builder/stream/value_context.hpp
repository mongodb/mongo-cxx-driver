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

#include <bsoncxx/builder/core.hpp>
#include <bsoncxx/builder/stream/array_context.hpp>
#include <bsoncxx/builder/stream/closed_context.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/util/functor.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {
namespace stream {

///
/// @todo document this class
///
template <class base>
class value_context {
   public:

    ///
    /// @todo document this method
    ///
    BSONCXX_INLINE value_context(core* core) : _core(core) {
    }

    ///
    /// @todo document this method
    ///
    template <class T>
    BSONCXX_INLINE
        typename std::enable_if<!util::is_functor<T, void(single_context)>::value, base>::type
        operator<<(T&& t) {
        _core->append(std::forward<T>(t));
        return unwrap();
    }

    ///
    /// @todo document this method
    ///
    template <typename T>
    BSONCXX_INLINE
        typename std::enable_if<util::is_functor<T, void(single_context)>::value, base>::type
        operator<<(T&& func) {
        func(*this);
        return unwrap();
    }

    ///
    /// @todo document this method
    ///
    BSONCXX_INLINE key_context<base> operator<<(const open_document_type) {
        _core->open_document();
        return wrap_document();
    }

    ///
    /// @todo document this method
    ///
    BSONCXX_INLINE array_context<base> operator<<(const open_array_type) {
        _core->open_array();
        return wrap_array();
    }

    ///
    /// @todo document this method
    ///
    operator single_context();

#if !defined(_MSC_VER)
    // TODO(MSVC): Causes an ICE under VS2015U1
    static_assert(
        std::is_same<value_context, decltype(std::declval<value_context>() << 1 << "str")>::value,
        "value_context must be templatized on a key_context");
#endif

   private:
    BSONCXX_INLINE base unwrap() {
        return base(_core);
    }

    BSONCXX_INLINE array_context<base> wrap_array() {
        return array_context<base>(_core);
    }

    BSONCXX_INLINE key_context<base> wrap_document() {
        return key_context<base>(_core);
    }

    core* _core;
};

}  // namespace stream
}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
