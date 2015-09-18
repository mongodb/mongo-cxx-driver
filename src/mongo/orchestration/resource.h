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
#include <vector>

#include "third_party/jsoncpp/json.h"
#include "third_party/restclient/restclient.h"

// Notes:
//  Posts to resources must not end with a slash or you will get a 404
//  Posts containing a JSON encoded body must conform to strict JSON

namespace mongo {
namespace orchestration {

typedef Json::Value Document;

class Resource {
public:
    Resource(const std::string& url);
    virtual ~Resource();

protected:
    RestClient::response get(const std::string& relative_path = "") const;
    RestClient::response put(const std::string& relative_path = "",
                             const std::string& payload = "{}");
    RestClient::response post(const std::string& relative_path = "",
                              const std::string& payload = "{}");
    RestClient::response del(const std::string& relative_path = "");

    std::string url() const;
    std::string relativeUrl(const std::string& relative_path) const;
    std::string baseRelativeUrl(const std::string& relative_path) const;

    Document handleResponse(const RestClient::response& response) const;

    static std::string resourceName();

    template <typename T>
    std::vector<T> pluralResource(const std::string& resource_name) const {
        std::vector<T> resources;
        Document doc = handleResponse(get(resource_name));

        for (unsigned i = 0; i < doc.size(); i++) {
            std::string server_id = doc[resource_name][i]["server_id"].asString();
            std::string url = T::resourceName().append("/").append(server_id);
            T resource(baseRelativeUrl(url));
            resources.push_back(resource);
        }

        return resources;
    }

private:
    std::string _url;
};

}  // namespace orchestration
}  // namesace mongo
