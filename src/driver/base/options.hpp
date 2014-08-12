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

#include <string>

namespace mongo {
namespace driver {

/**
 * The options class represents a set of options for the MongoDB driver client.
 *
 * @param mongodb_uri The MongoDB uri to extract options from.
 */
class options {
    friend class client;

   public:
    options();
    options(std::string mongodb_uri);
    options(const char* mongodb_uri);

   private:
    std::string _mongodb_uri;
};

}  // namespace driver
}  // namespace mongo
