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

#include <bsoncxx/builder/stream/document-fwd.hpp>

#include <bsoncxx/builder/core.hpp>
#include <bsoncxx/builder/stream/key_context.hpp>
#include <bsoncxx/builder/stream/single_context.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace builder {
namespace stream {

///
/// A streaming interface for constructing a BSON document.
///
/// @warning Use of the stream builder is discouraged. See
/// [Working with
/// BSON](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/working-with-bson/#basic-builder)
/// for more details.
///
class document : public key_context<> {
   public:
    ///
    /// Default constructor.
    ///
    document() : key_context<>(&_core), _core(false) {}

    ///
    /// @return A view of the BSON document.
    ///
    bsoncxx::v_noabi::document::view view() const {
        return _core.view_document();
    }

    ///
    /// @return A view of the BSON document.
    ///
    operator bsoncxx::v_noabi::document::view() const {
        return view();
    }

    ///
    /// Transfer ownership of the underlying document to the caller.
    ///
    /// @return A document::value with ownership of the document.
    ///
    /// @warning
    ///  After calling extract() it is illegal to call any methods
    ///  on this class, unless it is subsequenly moved into.
    ///
    bsoncxx::v_noabi::document::value extract() {
        return _core.extract_document();
    }

    ///
    /// Reset the underlying BSON to an empty document.
    ///
    void clear() {
        _core.clear();
    }

   private:
    core _core;
};

}  // namespace stream
}  // namespace builder
}  // namespace v_noabi
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::builder::stream::document.
///
