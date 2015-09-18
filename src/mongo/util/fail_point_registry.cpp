/*    Copyright 2014 MongoDB Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "mongo/util/fail_point_registry.h"

#include "mongo/util/map_util.h"
#include "mongo/util/mongoutils/str.h"

using mongoutils::str::stream;

namespace mongo {

using std::string;

FailPointRegistry::FailPointRegistry() : _frozen(false) {}

Status FailPointRegistry::addFailPoint(const string& name, FailPoint* failPoint) {
    if (_frozen) {
        return Status(ErrorCodes::CannotMutateObject, "Registry is already frozen");
    }

    if (_fpMap.count(name) > 0) {
        return Status(ErrorCodes::DuplicateKey,
                      stream() << "Fail point already registered: " << name);
    }

    _fpMap.insert(make_pair(name, failPoint));
    return Status::OK();
}

FailPoint* FailPointRegistry::getFailPoint(const string& name) const {
    return mapFindWithDefault(_fpMap, name, static_cast<FailPoint*>(NULL));
}

void FailPointRegistry::freeze() {
    _frozen = true;
}
}
