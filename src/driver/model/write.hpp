/**
 * Copyright 2014 MongoDB Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <cstdint>
#include "driver/util/optional.hpp"

namespace mongo {
namespace driver {
namespace model {

class write_concern;

template <class derived>
class write {

   public:
    derived& write_concern(const write_concern* write_concern) {
        _write_concern = &write_concern;
        return *this;
    }

    optional<class write_concern*> write_concern() const { return _write_concern; }

   protected:
    optional<const class write_concern*> _write_concern;
};

}  // namespace model
}  // namespace driver
}  // namespace mongo
