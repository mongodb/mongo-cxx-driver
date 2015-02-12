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

#include <bsoncxx/document/view.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {
namespace stream {

struct BSONCXX_API open_document_type { constexpr open_document_type() {} };
constexpr open_document_type open_document;

struct BSONCXX_API close_document_type { constexpr close_document_type() {} };
constexpr close_document_type close_document;

struct BSONCXX_API open_array_type { constexpr open_array_type() {} };
constexpr open_array_type open_array;

struct BSONCXX_API close_array_type { constexpr close_array_type() {} };
constexpr close_array_type close_array;

struct BSONCXX_API finalize_type { constexpr finalize_type() {} };
constexpr finalize_type finalize;

struct BSONCXX_API concatenate {
    document::view view;

    operator document::view() const { return view; }
};

}  // namespace stream
}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
