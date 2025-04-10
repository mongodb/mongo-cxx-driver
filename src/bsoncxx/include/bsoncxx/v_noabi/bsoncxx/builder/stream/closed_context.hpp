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

#include <bsoncxx/builder/core-fwd.hpp>
#include <bsoncxx/builder/stream/closed_context-fwd.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace builder {
namespace stream {

///
/// Indicates the document being constructed cannot be closed further.
///
/// Used as a template parameter for array_context, value_context or key_context. This could
/// indicate that the document is the root, or that the type stack has been intentionally erased, as
/// is the case when using callbacks in the stream api.
///
struct closed_context {
    closed_context(core*) {}
};

} // namespace stream
} // namespace builder
} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::builder::stream::closed_context.
///
