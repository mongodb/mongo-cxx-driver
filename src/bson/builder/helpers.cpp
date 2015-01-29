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

#include "bson/builder.hpp"

namespace bson {
namespace builder {
namespace helpers {

open_doc_t open_doc;
close_doc_t close_doc;
open_array_t open_array;
close_array_t close_array;

}  // namespace helpers
}  // namespace builders
}  // namespace bson

#include "driver/config/postlude.hpp"
