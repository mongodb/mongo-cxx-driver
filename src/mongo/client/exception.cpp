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

#include "mongo/client/exception.h"

namespace mongo {

    const char OperationException::kName[] = "OperationException";

    OperationException::OperationException(BSONObj gle) throw() : _gle(gle) {
    }

    OperationException::~OperationException() throw () {
    }

    const char* OperationException::what() const throw() {
        return kName;
    }

    BSONObj OperationException::obj() const throw() {
        return _gle;
    }

    int OperationException::code() const throw() {
        if (_gle.hasField("code")) {
            return _gle.getIntField("code");
        } else {
            return -1;
        }
    }

} // namespace mongo
