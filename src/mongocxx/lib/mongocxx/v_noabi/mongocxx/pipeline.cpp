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

#include <mongocxx/pipeline.hh>

//

#include <mongocxx/v1/pipeline.hh>

#include <mongocxx/private/scoped_bson.hh>

namespace mongocxx {
namespace v_noabi {

scoped_bson const& pipeline::internal::doc(pipeline const& self) {
    return v1::pipeline::internal::doc(self._pipeline);
}

} // namespace v_noabi
} // namespace mongocxx
