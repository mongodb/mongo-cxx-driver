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
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/builder/basic/impl.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/sub_array.hpp>
#include <bsoncxx/builder/core.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {
namespace basic {

///
/// A traditional builder-style interface for constructing
/// a BSON array.
///
class BSONCXX_API array : public sub_array {
   public:
    array() : sub_array(&_core), _core(true) {
    }

    ///
    /// @return A view of the BSON array.
    ///
    BSONCXX_INLINE bsoncxx::array::view view() const {
        return _core.view_array();
    }

    BSONCXX_INLINE operator bsoncxx::array::view() const {
        return view();
    }

    ///
    /// Transfer ownership of the underlying array to the caller.
    ///
    /// @return An array::value with ownership of the array.
    ///
    /// @warning
    ///  After calling extract() it is illegal to call any methods
    ///  on this class, unless it is subsequenly moved into.
    ///
    BSONCXX_INLINE bsoncxx::array::value extract() {
        return _core.extract_array();
    }

    ///
    /// Reset the underlying BSON to an empty array.
    ///
    BSONCXX_INLINE void clear() {
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
