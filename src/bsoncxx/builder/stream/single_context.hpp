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
#include <bsoncxx/builder/stream/array_context.hpp>
#include <bsoncxx/builder/stream/value_context.hpp>
#include <bsoncxx/builder/stream/key_context.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {
namespace stream {

class single_context {
   public:
    single_context(core* core) : _core(core) {}

    array_context<single_context> wrap_array() { return array_context<single_context>(_core); }
    key_context<single_context> wrap_document() { return key_context<single_context>(_core); }

    key_context<single_context> operator<<(open_document_type) {
        _core->open_document();

        return wrap_document();
    }

    array_context<single_context> operator<<(open_array_type) {
        _core->open_array();

        return wrap_array();
    }

    template <class T>
    void operator<<(T&& t) {
        _core->append(std::forward<T>(t));
    }

   private:
    core* _core;
};

template <class T>
array_context<T>::operator single_context() {
    return single_context(_core);
}

template <class T>
value_context<T>::operator single_context() {
    return single_context(_core);
}

}  // namespace stream
}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
