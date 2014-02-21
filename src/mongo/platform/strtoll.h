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

#pragma once

#include <cstdlib>

#ifdef _WIN32
static inline long long strtoll(const char* nptr, char** endptr, int base) {
    return _strtoi64(nptr, endptr, base);
}

static inline unsigned long long strtoull(const char* nptr, char** endptr, int base) {
    return _strtoui64(nptr, endptr, base);
}
#endif  // defined(_WIN32)
