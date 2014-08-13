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

#include "mongo/orchestration/resource.h"

namespace mongo {
namespace orchestration {

    class MongoResource : public Resource {
    public:
        void destroy();
        std::string uri() const;
        std::string mongodbUri() const;
        RestClient::response action(const std::string& action);

    protected:
        MongoResource(const std::string& url);
        RestClient::response status() const;
    };

} // namespace orchestration
} // namespace mongo
