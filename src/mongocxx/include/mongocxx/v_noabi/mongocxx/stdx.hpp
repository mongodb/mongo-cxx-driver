// Copyright 2015 MongoDB Inc.
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

#include <mongocxx/config/prelude.hpp>

// "Forward-declare" the `bsoncxx::stdx` namespace to permit the using-declaration below.
namespace bsoncxx {
inline namespace v_noabi {
namespace stdx {}
}  // namespace v_noabi
}  // namespace bsoncxx

namespace mongocxx {
inline namespace v_noabi {
namespace stdx {

// We adopt all the bsoncxx polyfills
using namespace ::bsoncxx::stdx;

}  // namespace stdx
}  // namespace v_noabi
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
