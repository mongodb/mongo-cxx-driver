// Copyright 2016 MongoDB Inc.
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

#include "helpers.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/create_view.hpp>

namespace {
using namespace bsoncxx;
using namespace mongocxx;

using builder::basic::kvp;
using builder::basic::make_document;

TEST_CASE("create_view accessors/mutators", "[create_view]") {
    instance::current();

    options::create_view cv;

    auto collation = make_document(kvp("locale", "en_US"));

    CHECK_OPTIONAL_ARGUMENT(cv, collation, collation.view());

    // We verify the accessors/mutators of 'pipeline' manually here, since the pipeline class
    // doesn't support equality (and therefore can't be used with CHECK_OPTIONAL_ARGUMENT()).
    SECTION("has pipeline disengaged") {
        REQUIRE(!cv.pipeline());
    }
    SECTION("has a method to set the pipeline") {
        cv.pipeline(std::move(pipeline{}.limit(1)));
        REQUIRE(cv.pipeline()->view_array() == pipeline{}.limit(1).view_array());
    }
}

TEST_CASE("create_view can be exported to a document", "[create_view]") {
    instance::current();

    options::create_view cv;

    auto collation_en_US = make_document(kvp("locale", "en_US"));

    cv.collation(collation_en_US.view());
    cv.pipeline(std::move(pipeline{}.limit(1)));

    auto doc = cv.to_document_deprecated();
    document::view doc_view{doc.view()};

    // "collation" field is set correctly.
    document::element collation{doc_view["collation"]};
    REQUIRE(collation);
    REQUIRE(collation.type() == type::k_document);
    REQUIRE(collation.get_document().value == collation_en_US);

    // "pipeline" field is set correctly.
    document::element pipeline_ele{doc_view["pipeline"]};
    REQUIRE(pipeline_ele);
    REQUIRE(pipeline_ele.type() == type::k_array);
    REQUIRE(pipeline_ele.get_array().value == pipeline{}.limit(1).view_array());
}
}  // namespace
