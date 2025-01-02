// Copyright 2009-present MongoDB, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <bsoncxx/json.hpp>

#include <mongocxx/change_stream.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/options/change_stream.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(mongocxx::client client) {
    // Basic usage.
    {
        mongocxx::change_stream stream = client.watch();

        EXPECT(stream.get_resume_token());
    }

    // With options.
    {
        mongocxx::options::change_stream opts;

        opts.batch_size(1);
        // ... other change stream options.

        mongocxx::change_stream stream = client.watch(opts);

        EXPECT(stream.get_resume_token());
    }

    // With a pipeline.
    {
        mongocxx::pipeline pipeline;

        pipeline.match(bsoncxx::from_json(R"({"operationType": "insert"})"));
        // ... other pipeline options.

        mongocxx::change_stream stream = client.watch(pipeline);

        EXPECT(stream.get_resume_token());
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_FOR_REPLICA() {
    example(mongocxx::client{mongocxx::uri{}});
}
