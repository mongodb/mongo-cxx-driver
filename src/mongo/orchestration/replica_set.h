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

#include "mongo/orchestration/mongo_resource.h"
#include "mongo/orchestration/server.h"

namespace mongo {
namespace orchestration {

    class ReplicaSet : public MongoResource {
        friend class Resource;
        friend class Service;
        friend class Cluster;

    public:
        Server primary() const;
        std::vector<Server> secondaries() const;
        std::vector<Server> arbiters() const;
        std::vector<Server> hidden() const;
        std::vector<Server> members() const;

        static std::string resourceName();

    private:
        ReplicaSet(const std::string& url);
    };

} // namespace orchestration
} // namespace mongo
