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

#include "driver/options/delete.hpp"

namespace mongo {
namespace driver {
namespace options {

void delete_options::write_concern(base::write_concern wc) { _write_concern = std::move(wc); }

const optional<base::write_concern>& delete_options::write_concern() const {
    return _write_concern;
}

}  // namespace options
}  // namespace driver
}  // namespace mongo
