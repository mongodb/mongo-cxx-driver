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

#include "mongo/client/write_concern.h"
#include "mongo/unittest/unittest.h"

namespace mongo {

using std::string;

TEST(WriteConcern, Defaults) {
    WriteConcern wc;
    ASSERT_FALSE(wc.hasMode());
    ASSERT_EQUALS(wc.nodes(), 1);
    ASSERT_FALSE(wc.journal());
    ASSERT_FALSE(wc.fsync());
    ASSERT_EQUALS(wc.timeout(), 0);
}

TEST(WriteConcern, Chain) {
    WriteConcern wc = WriteConcern().nodes(2).journal(true);
    ASSERT_EQUALS(wc.nodes(), 2);
    ASSERT_FALSE(wc.hasMode());
    ASSERT_TRUE(wc.journal());

    // should be defaults
    ASSERT_FALSE(wc.fsync());
    ASSERT_EQUALS(wc.timeout(), 0);
}

TEST(WriteConcern, DefaultToBSON) {
    BSONObj result;

    WriteConcern wc;
    ASSERT_EQUALS(wc.nodes(), 1);
    result = wc.obj();
    ASSERT_FALSE(result.hasField("w"));
    ASSERT_FALSE(result.hasField("j"));
    ASSERT_FALSE(result.hasField("fsync"));
    ASSERT_FALSE(result.hasField("wtimeout"));
}

TEST(WriteConcern, NodesString) {
    BSONObj result;
    WriteConcern wc;

    wc.nodes(3);
    ASSERT_FALSE(wc.hasMode());
    ASSERT_EQUALS(wc.nodes(), 3);
    result = wc.obj();
    ASSERT_TRUE(result.hasField("w"));
    ASSERT_EQUALS(result["w"].Int(), 3);

    wc.mode(WriteConcern::kMajority);
    ASSERT_TRUE(wc.hasMode());
    ASSERT_EQUALS(wc.mode(), string(WriteConcern::kMajority));
    result = wc.obj();
    ASSERT_TRUE(result.hasField("w"));
    ASSERT_EQUALS(result["w"].String(), string(WriteConcern::kMajority));

    wc.nodes(5);
    ASSERT_FALSE(wc.hasMode());
    ASSERT_EQUALS(wc.nodes(), 5);
    result = wc.obj();
    ASSERT_TRUE(result.hasField("w"));
    ASSERT_EQUALS(result["w"].Int(), 5);
}

TEST(WriteConcern, NeedsGLE) {
    WriteConcern wc;
    ASSERT_TRUE(wc.requiresConfirmation());

    wc.nodes(0);
    ASSERT_FALSE(wc.requiresConfirmation());

    wc.nodes(3);
    ASSERT_TRUE(wc.requiresConfirmation());
}

TEST(WriteConcern, Unacknowledged) {
    ASSERT_FALSE(WriteConcern::unacknowledged.requiresConfirmation());
    ASSERT_EQUALS(WriteConcern::unacknowledged.nodes(), 0);

    ASSERT_FALSE(WriteConcern().nodes(0).requiresConfirmation());

    ASSERT_FALSE(WriteConcern().nodes(0).journal(false).requiresConfirmation());

    ASSERT_FALSE(WriteConcern().nodes(0).timeout(100).requiresConfirmation());
}

TEST(WriteConcern, Acknowledged) {
    ASSERT_TRUE(WriteConcern::acknowledged.requiresConfirmation());
    ASSERT_EQUALS(WriteConcern::acknowledged.nodes(), 1);
}

TEST(WriteConcern, Journaled) {
    ASSERT_TRUE(WriteConcern::journaled.requiresConfirmation());
    ASSERT_TRUE(WriteConcern::journaled.journal());
}

TEST(WriteConcern, Replicated) {
    ASSERT_TRUE(WriteConcern::replicated.requiresConfirmation());
    ASSERT_EQUALS(WriteConcern::replicated.nodes(), 2);
}

TEST(WriteConcern, Majority) {
    ASSERT_TRUE(WriteConcern::majority.requiresConfirmation());
    ASSERT_EQUALS(WriteConcern::majority.mode(), string(WriteConcern::kMajority));
    ASSERT_TRUE(WriteConcern::majority.hasMode());
}

}  // namespace
