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

#include "mongo/orchestration/resource.h"

#include "mongo/orchestration/replica_set.h"
#include "mongo/orchestration/server.h"
#include "mongo/orchestration/sharded_cluster.h"

namespace mongo {
namespace orchestration {

    class Service : public Resource {
    public:
        explicit Service(const std::string& url);

        std::vector<Server> servers() const;
        std::vector<ReplicaSet> replica_sets() const;
        std::vector<ShardedCluster> clusters() const;

        std::string createMongod(const Document& params = Document());
        std::string createMongos(const Document& params = Document());
        std::string createReplicaSet(const Document& params = Document());
        std::string createShardedCluster(const Document& params = Document());

        Server server(const std::string& id) const;
        ReplicaSet replicaSet(const std::string& id) const;
        ShardedCluster shardedCluster(const std::string& id) const;

    private:
        std::string _createResource(const char resource[], const Document& params);
    };

} // namespace orchestration
} // namespace mongo
