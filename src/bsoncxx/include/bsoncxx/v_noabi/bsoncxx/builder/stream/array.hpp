// Copyright 2009-present MongoDB, Inc.
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

#include <bsoncxx/builder/stream/array-fwd.hpp>

#include <bsoncxx/array/value.hpp>
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/builder/core.hpp>
#include <bsoncxx/builder/stream/array_context.hpp>
#include <bsoncxx/builder/stream/key_context.hpp>
#include <bsoncxx/builder/stream/single_context.hpp>
#include <bsoncxx/builder/stream/value_context.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace builder {
namespace stream {

///
/// A streaming interface for constructing a BSON array.
///
/// @warning Use of the stream builder is discouraged. See
/// [Working with
/// BSON](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/working-with-bson/#basic-builder)
/// for more details.
///
class array : public array_context<> {
   public:
    ///
    /// Default constructor.
    ///
    array() : array_context<>(&_core), _core(true) {}

    ///
    /// @return A view of the BSON array.
    ///
    bsoncxx::v_noabi::array::view view() const {
        return _core.view_array();
    }

    ///
    /// @return A view of the BSON array.
    ///
    operator bsoncxx::v_noabi::array::view() const {
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
    bsoncxx::v_noabi::array::value extract() {
        return _core.extract_array();
    }

    ///
    /// Reset the underlying BSON to an empty array.
    ///
    void clear() {
        _core.clear();
    }

   private:
    core _core;
};

} // namespace stream
} // namespace builder
} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::builder::stream::array.
///
