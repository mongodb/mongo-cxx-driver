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
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {
namespace basic {

    class document {
    public:
        document() : _core(false) {}

        template <typename Arg, typename ...Args>
        void append(Arg&& a, Args&& ...args) {
            append(std::forward<Arg>(a));
            append(std::forward<Args>(args)...);
        }

        template <typename T>
        void append(T&& t) {
            _core.key_owning(std::get<0>(t));
            _core.append(std::get<1>(t));
        }

        template <std::size_t n, typename T>
        void append(std::tuple<const char(&)[n], T>&& t) {
            _core.key_literal(std::get<0>(t), n - 1);
            _core.append(std::get<1>(t));
        }

        bsoncxx::document::view view() const {
            return _core.view_document();
        }

        operator bsoncxx::document::view() const {
            return view();
        }

        bsoncxx::document::value extract() {
            return _core.extract_document();
        }

        void clear() {
            _core.clear();
        }

    private:
        core _core;
    };

}  // namespace basic
}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
