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

#include "mongo/orchestration/mongo_resource.h"
#include "mongo/util/mongoutils/str.h"

#include <stdexcept>

namespace mongo {
namespace orchestration {

    MongoResource::MongoResource(const std::string& url)
        : Resource(url)
    {}

    void MongoResource::destroy() {
        del();
    }

    RestClient::response MongoResource::status() const {
        return get();
    }

    RestClient::response MongoResource::action(const std::string& action) {
        Json::Value doc;
        Json::FastWriter writer;
        doc["action"] = action;
        return post("", writer.write(doc));
    }

    std::string MongoResource::uri() const {
        // mongodb_uri has the format: mongodb://<hostport>[/stuff not in standalones]
        std::string uri = handleResponse(status())["mongodb_uri"].asString();
        const std::string prefix("mongodb://");
        if (uri.substr(0, prefix.size()) != prefix) {
            throw std::runtime_error(
                      str::stream() << "mongodb_uri does not begin with prefix 'mongodb://'"
                                    << ", got: " << uri
                  );
        }
        uri = uri.substr(prefix.size());
        const size_t suffix = uri.find('/');
        return uri.substr(0, suffix);
    }

    std::string MongoResource::mongodbUri() const {
        return handleResponse(status())["mongodb_uri"].asString();
    }

} // namespace orchestration
} // namespace mongo
