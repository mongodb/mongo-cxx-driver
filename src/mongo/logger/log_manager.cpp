/*    Copyright 2013 10gen Inc.
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

#include "mongo/platform/basic.h"

#include "mongo/logger/log_manager.h"

namespace mongo {
namespace logger {

    LogManager::LogManager() {
    }

    LogManager::~LogManager() {
        for (DomainsByNameMap::iterator iter = _domains.begin(); iter != _domains.end(); ++iter) {
            delete iter->second;
        }
    }

    MessageLogDomain* LogManager::getNamedDomain(const std::string& name) {
        MessageLogDomain*& domain = _domains[name];
        if (!domain) {
            domain = new MessageLogDomain;
        }
        return domain;
    }

}  // logger
}  // mongo
