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

#ifndef UTIL_VERSION_HEADER
#define UTIL_VERSION_HEADER

#include <string>

#include "mongo/bson/bsonobj.h"

namespace mongo {
    struct BSONArray;

    // mongo version
    extern const char versionString[];
    extern const BSONArray versionArray;
    std::string mongodVersion();

    // Convert a version string into a numeric array
    BSONArray toVersionArray(const char* version);

    // Checks whether another version is the same major version as us
    bool isSameMajorVersion(const char* version);

    const char * gitVersion();
    const char * compiledJSEngine();
    const char * allocator();
    const char * loaderFlags();
    const char * compilerFlags();
    std::string sysInfo();

}  // namespace mongo

#endif  // UTIL_VERSION_HEADER
