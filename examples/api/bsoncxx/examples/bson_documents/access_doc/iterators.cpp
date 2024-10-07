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

#include <bsoncxx/document/element.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// {"a": 1, "b": 2}
void example(bsoncxx::document::view doc) {
    EXPECT(doc.begin() != doc.end());

    auto iter = doc.begin();
    EXPECT(iter == doc.begin());

    {
        bsoncxx::document::element e = *iter;

        EXPECT(e.key().compare("a") == 0);
        EXPECT(e.get_int32().value == 1);
    }

    ++iter;

    EXPECT(iter->key().compare("b") == 0);
    EXPECT(iter->get_int32().value == 2);

    {
        auto iter_copy = iter++;

        EXPECT(iter_copy != iter);
        EXPECT(iter_copy->key().compare("b") == 0);
        EXPECT(iter_copy->get_int32() == 2);
    }

    EXPECT(iter == doc.end());
}
// [Example]

}  // namespace

RUNNER_REGISTER_COMPONENT() {
    example(bsoncxx::from_json(R"({"a": 1, "b": 2})"));
}
