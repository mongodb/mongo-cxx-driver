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

#include "driver/model/delete_one.hpp"

namespace mongo {
namespace driver {
namespace model {

delete_one::delete_one(bson::document::view filter) : _filter(std::move(filter)) {}

const bson::document::view& delete_one::filter() const { return _filter; }

}  // namespace model
}  // namespace driver
}  // namespace mongo
