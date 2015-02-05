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

#include <bsoncxx/builder/concrete.hpp>
#include <bsoncxx/builder/key_ctx.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>

namespace mongo {
namespace bson {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {

    class document : public key_ctx<> {
    public:
        document() : key_ctx<>(&_concrete), _concrete(false) {}

        bson::document::view view() const {
            return _concrete.view();
        }

        operator bson::document::view() const {
            return _concrete.view();
        }

        bson::document::value extract() {
            return _concrete.extract();
        }

        void clear() {
            _concrete.clear();
        }

    private:
        concrete _concrete;
    };

}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bson
}  // namespace mongo

#include <bsoncxx/config/postlude.hpp>
