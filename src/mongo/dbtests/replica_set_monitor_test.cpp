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

#include "mongo/platform/basic.h"

#include "mongo/client/dbclientinterface.h"
#include "mongo/client/dbclient_rs.h"
#include "mongo/client/replica_set_monitor.h"
#include "mongo/client/replica_set_monitor_internal.h"
#include "mongo/dbtests/mock/mock_conn_registry.h"
#include "mongo/dbtests/mock/mock_replica_set.h"
#include "mongo/unittest/unittest.h"

#include <set>
#include <vector>

using std::map;
using std::vector;
using std::set;
using std::string;
using boost::scoped_ptr;

using mongo::BSONObj;
using mongo::BSONObjBuilder;
using mongo::BSONArray;
using mongo::BSONArrayBuilder;
using mongo::BSONElement;
using mongo::ConnectionString;
using mongo::HostAndPort;
using mongo::MockReplicaSet;
using mongo::ReadPreference;
using mongo::ReadPreferenceSetting;
using mongo::ReplicaSetMonitor;
using mongo::ReplicaSetMonitorPtr;
using mongo::TagSet;

// Pull nested types to top-level scope
typedef ReplicaSetMonitor::IsMasterReply IsMasterReply;
typedef ReplicaSetMonitor::ScanState ScanState;
typedef ReplicaSetMonitor::ScanStatePtr ScanStatePtr;
typedef ReplicaSetMonitor::SetState SetState;
typedef ReplicaSetMonitor::SetStatePtr SetStatePtr;
typedef ReplicaSetMonitor::Refresher Refresher;
typedef Refresher::NextStep NextStep;
typedef ScanState::UnconfirmedReplies UnconfirmedReplies;
typedef SetState::Node Node;
typedef SetState::Nodes Nodes;

namespace mongo_test {

    bool isCompatible(const Node& node, ReadPreference pref, const TagSet& tagSet) {
        set<HostAndPort> seeds;
        seeds.insert(node.host);
        SetState set("name", seeds);
        set.nodes.push_back(node);

        ReadPreferenceSetting criteria(pref, tagSet);
        return !set.getMatchingHost(criteria).empty();
    }

    HostAndPort selectNode(const vector<Node>& nodes,
                           ReadPreference pref,
                           const TagSet& tagSet,
                           int latencyThresholdMillis,
                           bool* isPrimarySelected)
    {
        invariant(!nodes.empty());
        set<HostAndPort> seeds;
        seeds.insert(nodes.front().host);
        SetState set("name", seeds);
        set.nodes = nodes;
        set.latencyThresholdMicros = latencyThresholdMillis * 1000;

        ReadPreferenceSetting criteria(pref, tagSet);
        HostAndPort out = set.getMatchingHost(criteria);
        if (isPrimarySelected)
            *isPrimarySelected = !out.empty() && set.findNode(out)->isMaster;
        return out;
    }


    const BSONObj SampleIsMasterDoc = BSON("tags"
                                            << BSON("dc" << "NYC"
                                                    << "p" << "2"
                                                    << "region" << "NA"));
    const BSONObj SampleTags = SampleIsMasterDoc["tags"].Obj();
    const BSONObj NoTags = BSONObj();
    const BSONObj NoTagIsMasterDoc = BSON("isMaster" << true);

    TEST(ReplSetMonitorNode, SimpleGoodMatch) {
        Node node(((HostAndPort())));
        node.tags = BSON("dc" << "sf");
        ASSERT(node.matches(BSON("dc" << "sf")));
    }

    TEST(ReplSetMonitorNode, SimpleBadMatch) {
        Node node((HostAndPort()));
        node.tags = BSON("dc" << "nyc");
        ASSERT(!node.matches(BSON("dc" << "sf")));
    }

    TEST(ReplSetMonitorNode, ExactMatch) {
        Node node((HostAndPort()));
        node.tags = SampleTags;
        ASSERT(node.matches(SampleIsMasterDoc["tags"].Obj()));
    }

    TEST(ReplSetMonitorNode, EmptyTag) {
        Node node((HostAndPort()));
        node.tags = SampleTags;
        ASSERT(node.matches(BSONObj()));
    }

    TEST(ReplSetMonitorNode, MemberNoTagMatchesEmptyTag) {
        Node node((HostAndPort()));
        node.tags = NoTags;
        ASSERT(node.matches(BSONObj()));
    }

    TEST(ReplSetMonitorNode, MemberNoTagDoesNotMatch) {
        Node node((HostAndPort()));
        node.tags = NoTags;
        ASSERT(!node.matches(BSON("dc" << "NYC")));
    }

    TEST(ReplSetMonitorNode, IncompleteMatch) {
        Node node((HostAndPort()));
        node.tags = SampleTags;
        ASSERT(!node.matches(BSON("dc" << "NYC"
                                     << "p" << "2"
                                     << "hello" << "world")));
    }

    TEST(ReplSetMonitorNode, PartialMatch) {
        Node node((HostAndPort()));
        node.tags = SampleTags;
        ASSERT(node.matches(BSON("dc" << "NYC"
                                    << "p" << "2")));
    }

    TEST(ReplSetMonitorNode, SingleTagCrit) {
        Node node((HostAndPort()));
        node.tags = SampleTags;
        ASSERT(node.matches(BSON("p" << "2")));
    }

    TEST(ReplSetMonitorNode, BadSingleTagCrit) {
        Node node((HostAndPort()));
        node.tags = SampleTags;
        ASSERT(!node.matches(BSON("dc" << "SF")));
    }

    TEST(ReplSetMonitorNode, NonExistingFieldTag) {
        Node node((HostAndPort()));
        node.tags = SampleTags;
        ASSERT(!node.matches(BSON("noSQL" << "Mongo")));
    }

    TEST(ReplSetMonitorNode, UnorederedMatching) {
            Node node((HostAndPort()));
            node.tags = SampleTags;
            ASSERT(node.matches(BSON("p" << "2" << "dc" << "NYC")));
    }

    TEST(ReplSetMonitorNode, SameValueDiffKey) {
        Node node((HostAndPort()));
        node.tags = SampleTags;
        ASSERT(!node.matches(BSON("datacenter" << "NYC")));
    }

    TEST(ReplSetMonitorNode, PriNodeCompatibleTag) {
        Node node(HostAndPort("dummy", 3));
        node.tags = SampleTags;

        node.isUp = true;
        node.isMaster = true;

        BSONArrayBuilder builder;
        builder.append(BSON("dc" << "NYC"));

        TagSet tags(BSONArray(builder.done()));

        ASSERT(isCompatible(node, mongo::ReadPreference_PrimaryOnly, tags));
        ASSERT(isCompatible(node, mongo::ReadPreference_PrimaryPreferred, tags));
        ASSERT(isCompatible(node, mongo::ReadPreference_SecondaryPreferred, tags));
        ASSERT(!isCompatible(node, mongo::ReadPreference_SecondaryOnly, tags));
        ASSERT(isCompatible(node, mongo::ReadPreference_Nearest, tags));
    }

    TEST(ReplSetMonitorNode, SecNodeCompatibleTag) {
        Node node(HostAndPort("dummy", 3));
        node.tags = SampleTags;

        node.isUp = true;
        node.isMaster = false;

        BSONArrayBuilder builder;
        builder.append(BSON("dc" << "NYC"));

        TagSet tags(BSONArray(builder.done()));

        ASSERT(!isCompatible(node, mongo::ReadPreference_PrimaryOnly, tags));
        ASSERT(isCompatible(node, mongo::ReadPreference_PrimaryPreferred, tags));
        ASSERT(isCompatible(node, mongo::ReadPreference_SecondaryPreferred, tags));
        ASSERT(isCompatible(node, mongo::ReadPreference_SecondaryOnly, tags));
        ASSERT(isCompatible(node, mongo::ReadPreference_Nearest, tags));
    }

    TEST(ReplSetMonitorNode, PriNodeNotCompatibleTag) {
        Node node(HostAndPort("dummy", 3));
        node.tags = SampleTags;

        node.isUp = true;
        node.isMaster = true;

        BSONArrayBuilder builder;
        builder.append(BSON("dc" << "SF"));

        TagSet tags(BSONArray(builder.done()));

        ASSERT(isCompatible(node, mongo::ReadPreference_PrimaryOnly, tags));
        ASSERT(isCompatible(node, mongo::ReadPreference_PrimaryPreferred, tags));
        ASSERT(isCompatible(node, mongo::ReadPreference_SecondaryPreferred, tags));
        ASSERT(!isCompatible(node, mongo::ReadPreference_SecondaryOnly, tags));
        ASSERT(!isCompatible(node, mongo::ReadPreference_Nearest, tags));
    }

    TEST(ReplSetMonitorNode, SecNodeNotCompatibleTag) {
        Node node(HostAndPort("dummy", 3));
        node.tags = SampleTags;

        node.isUp = true;
        node.isMaster = false;

        BSONArrayBuilder builder;
        builder.append(BSON("dc" << "SF"));

        TagSet tags(BSONArray(builder.done()));

        ASSERT(!isCompatible(node, mongo::ReadPreference_PrimaryOnly, tags));
        ASSERT(!isCompatible(node, mongo::ReadPreference_PrimaryPreferred, tags));
        ASSERT(!isCompatible(node, mongo::ReadPreference_SecondaryPreferred, tags));
        ASSERT(!isCompatible(node, mongo::ReadPreference_SecondaryOnly, tags));
        ASSERT(!isCompatible(node, mongo::ReadPreference_Nearest, tags));
    }

    TEST(ReplSetMonitorNode, PriNodeCompatiblMultiTag) {
        Node node(HostAndPort("dummy", 3));
        node.tags = SampleTags;

        node.isUp = true;
        node.isMaster = true;

        BSONArrayBuilder builder;
        builder.append(BSON("dc" << "RP"));
        builder.append(BSON("dc" << "NYC" << "p" << "2"));

        TagSet tags(BSONArray(builder.done()));

        ASSERT(isCompatible(node, mongo::ReadPreference_PrimaryOnly, tags));
        ASSERT(isCompatible(node, mongo::ReadPreference_PrimaryPreferred, tags));
        ASSERT(isCompatible(node, mongo::ReadPreference_SecondaryPreferred, tags));
        ASSERT(!isCompatible(node, mongo::ReadPreference_SecondaryOnly, tags));
        ASSERT(isCompatible(node, mongo::ReadPreference_Nearest, tags));
    }

    TEST(ReplSetMonitorNode, SecNodeCompatibleMultiTag) {
        Node node(HostAndPort("dummy", 3));
        node.tags = SampleTags;

        node.isUp = true;
        node.isMaster = false;

        BSONArrayBuilder builder;
        builder.append(BSON("dc" << "RP"));
        builder.append(BSON("dc" << "NYC" << "p" << "2"));

        TagSet tags(BSONArray(builder.done()));

        ASSERT(!isCompatible(node, mongo::ReadPreference_PrimaryOnly, tags));
        ASSERT(isCompatible(node, mongo::ReadPreference_PrimaryPreferred, tags));
        ASSERT(isCompatible(node, mongo::ReadPreference_SecondaryPreferred, tags));
        ASSERT(isCompatible(node, mongo::ReadPreference_SecondaryOnly, tags));
        ASSERT(isCompatible(node, mongo::ReadPreference_Nearest, tags));
    }

    TEST(ReplSetMonitorNode, PriNodeNotCompatibleMultiTag) {
        Node node(HostAndPort("dummy", 3));
        node.tags = SampleTags;

        node.isUp = true;
        node.isMaster = true;

        BSONArrayBuilder builder;
        builder.append(BSON("dc" << "sf"));
        builder.append(BSON("dc" << "NYC" << "P" << "4"));

        TagSet tags(BSONArray(builder.done()));

        ASSERT(isCompatible(node, mongo::ReadPreference_PrimaryOnly, tags));
        ASSERT(isCompatible(node, mongo::ReadPreference_PrimaryPreferred, tags));
        ASSERT(isCompatible(node, mongo::ReadPreference_SecondaryPreferred, tags));
        ASSERT(!isCompatible(node, mongo::ReadPreference_SecondaryOnly, tags));
        ASSERT(!isCompatible(node, mongo::ReadPreference_Nearest, tags));
    }

    TEST(ReplSetMonitorNode, SecNodeNotCompatibleMultiTag) {
        Node node(HostAndPort("dummy", 3));
        node.tags = SampleTags;

        node.isUp = true;
        node.isMaster = false;

        BSONArrayBuilder builder;
        builder.append(BSON("dc" << "sf"));
        builder.append(BSON("dc" << "NYC" << "P" << "4"));

        TagSet tags(BSONArray(builder.done()));

        ASSERT(!isCompatible(node, mongo::ReadPreference_PrimaryOnly, tags));
        ASSERT(!isCompatible(node, mongo::ReadPreference_PrimaryPreferred, tags));
        ASSERT(!isCompatible(node, mongo::ReadPreference_SecondaryPreferred, tags));
        ASSERT(!isCompatible(node, mongo::ReadPreference_SecondaryOnly, tags));
        ASSERT(!isCompatible(node, mongo::ReadPreference_Nearest, tags));
    }


    class NodeSetFixtures {
    public:
        static vector<Node> getThreeMemberWithTags();
    };

    vector<Node> NodeSetFixtures::getThreeMemberWithTags() {
        vector<Node> nodes;

        nodes.push_back(Node(HostAndPort("a")));
        nodes.push_back(Node(HostAndPort("b")));
        nodes.push_back(Node(HostAndPort("c")));

        nodes[0].isUp = true;
        nodes[1].isUp = true;
        nodes[2].isUp = true;

        nodes[0].isMaster = false;
        nodes[1].isMaster = true;
        nodes[2].isMaster = false;

        nodes[0].tags = BSON("dc" << "nyc" << "p" << "1");
        nodes[1].tags = BSON("dc" << "sf");
        nodes[2].tags = BSON("dc" << "nyc" << "p" << "2");

        return nodes;
    }

    class TagSetFixtures {
    public:
        static BSONArray getDefaultSet();
        static BSONArray getP2Tag();
        static BSONArray getSingleNoMatchTag();
        static BSONArray getMultiNoMatchTag();
    };

    BSONArray TagSetFixtures::getDefaultSet() {
        BSONArrayBuilder arrayBuilder;
        arrayBuilder.append(BSONObj());
        return arrayBuilder.arr();
    }

    BSONArray TagSetFixtures::getP2Tag() {
        BSONArrayBuilder arrayBuilder;
        arrayBuilder.append(BSON("p" << "2"));
        return arrayBuilder.arr();
    }

    BSONArray TagSetFixtures::getSingleNoMatchTag() {
        BSONArrayBuilder arrayBuilder;
        arrayBuilder.append(BSON("k" << "x"));
        return arrayBuilder.arr();
    }

    BSONArray TagSetFixtures::getMultiNoMatchTag() {
        BSONArrayBuilder arrayBuilder;
        arrayBuilder.append(BSON("mongo" << "db"));
        arrayBuilder.append(BSON("by" << "10gen"));
        return arrayBuilder.arr();
    }

    TEST(ReplSetMonitorReadPref, PrimaryOnly) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getDefaultSet());

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryOnly, tags, 3,
            &isPrimarySelected);

        ASSERT(isPrimarySelected);
        ASSERT_EQUALS("b", host.host());
    }

    TEST(ReplSetMonitorReadPref, PrimaryOnlyPriNotOk) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getDefaultSet());

        nodes[1].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryOnly, tags, 3, 
            &isPrimarySelected);

        ASSERT(host.empty());
    }

    TEST(ReplSetMonitorReadPref, PrimaryMissing) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getDefaultSet());

        nodes[1].isMaster = false;

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryOnly, tags, 3,
            &isPrimarySelected);

        ASSERT(host.empty());
    }

    TEST(ReplSetMonitorReadPref, PriPrefWithPriOk) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();

        TagSet tags(TagSetFixtures::getDefaultSet());

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryPreferred, tags, 1,
            &isPrimarySelected);

        ASSERT(isPrimarySelected);
        ASSERT_EQUALS("b", host.host());
    }

    TEST(ReplSetMonitorReadPref, PriPrefWithPriNotOk) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getDefaultSet());

        nodes[1].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryPreferred, tags, 1,
            &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT(host.host() == "a" || host.host() == "c");
    }

    TEST(ReplSetMonitorReadPref, SecOnly) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getDefaultSet());

        nodes[2].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryOnly, tags, 1,
            &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("a", host.host());
    }

    TEST(ReplSetMonitorReadPref, SecOnlyOnlyPriOk) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getDefaultSet());

        nodes[0].markFailed();
        nodes[2].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryOnly, tags, 1,
            &isPrimarySelected);

        ASSERT(host.empty());
    }

    TEST(ReplSetMonitorReadPref, SecPref) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getDefaultSet());

        nodes[2].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryPreferred, tags, 1,
            &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("a", host.host());
    }

    TEST(ReplSetMonitorReadPref, SecPrefWithNoSecOk) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getDefaultSet());

        nodes[0].markFailed();
        nodes[2].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryPreferred, tags, 1,
            &isPrimarySelected);

        ASSERT(isPrimarySelected);
        ASSERT_EQUALS("b", host.host());
    }

    TEST(ReplSetMonitorReadPref, SecPrefWithNoNodeOk) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getDefaultSet());

        nodes[0].markFailed();
        nodes[1].markFailed();
        nodes[2].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryPreferred, tags, 1,
            &isPrimarySelected);

        ASSERT(host.empty());
    }

    TEST(ReplSetMonitorReadPref, NearestAllLocal) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getDefaultSet());

        nodes[0].latencyMicros = 1*1000;
        nodes[1].latencyMicros = 2*1000;
        nodes[2].latencyMicros = 3*1000;

        bool isPrimarySelected = 0;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_Nearest, tags, 3,
            &isPrimarySelected);

        // Any host is ok
        ASSERT(!host.empty());
        ASSERT_EQUALS(isPrimarySelected, host.host() == "b");
    }

    TEST(ReplSetMonitorReadPref, NearestOneLocal) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getDefaultSet());

        nodes[0].latencyMicros = 10*1000;
        nodes[1].latencyMicros = 20*1000;
        nodes[2].latencyMicros = 30*1000;

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_Nearest, tags, 3,
            &isPrimarySelected);

        ASSERT_EQUALS("a", host.host());
        ASSERT(!isPrimarySelected);
    }

    TEST(ReplSetMonitorReadPref, PriOnlyWithTagsNoMatch) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getP2Tag());

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryOnly, tags, 3,
            &isPrimarySelected);

        ASSERT(isPrimarySelected);
        // Note: PrimaryOnly ignores tag
        ASSERT_EQUALS("b", host.host());
    }

    TEST(ReplSetMonitorReadPref, PriPrefPriNotOkWithTags) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getP2Tag());

        nodes[1].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryPreferred, tags, 3,
            &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("c", host.host());
    }

    TEST(ReplSetMonitorReadPref, PriPrefPriOkWithTagsNoMatch) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getSingleNoMatchTag());

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryPreferred, tags, 3,
            &isPrimarySelected);

        ASSERT(isPrimarySelected);
        ASSERT_EQUALS("b", host.host());
    }

    TEST(ReplSetMonitorReadPref, PriPrefPriNotOkWithTagsNoMatch) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getSingleNoMatchTag());

        nodes[1].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryPreferred, tags, 3,
            &isPrimarySelected);

        ASSERT(host.empty());
    }

    TEST(ReplSetMonitorReadPref, SecOnlyWithTags) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getP2Tag());

        bool isPrimarySelected;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryOnly, tags, 3,
            &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("c", host.host());
    }

    TEST(ReplSetMonitorReadPref, SecOnlyWithTagsMatchOnlyPri) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();

        BSONArrayBuilder arrayBuilder;
        arrayBuilder.append(BSON("dc" << "sf"));
        TagSet tags(arrayBuilder.arr());

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
             mongo::ReadPreference_SecondaryOnly, tags, 3,
             &isPrimarySelected);

        ASSERT(host.empty());
    }

    TEST(ReplSetMonitorReadPref, SecPrefWithTags) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getP2Tag());

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryPreferred, tags, 3,
            &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("c", host.host());
    }

    TEST(ReplSetMonitorReadPref, SecPrefSecNotOkWithTags) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();

        BSONArrayBuilder arrayBuilder;
        arrayBuilder.append(BSON("dc" << "nyc"));
        TagSet tags(arrayBuilder.arr());

        nodes[2].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryPreferred, tags, 3,
            &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("a", host.host());
    }

    TEST(ReplSetMonitorReadPref, SecPrefPriOkWithTagsNoMatch) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getSingleNoMatchTag());

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryPreferred, tags, 3,
            &isPrimarySelected);

        ASSERT(isPrimarySelected);
        ASSERT_EQUALS("b", host.host());
    }

    TEST(ReplSetMonitorReadPref, SecPrefPriNotOkWithTagsNoMatch) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getSingleNoMatchTag());

        nodes[1].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryPreferred, tags, 3,
            &isPrimarySelected);

        ASSERT(host.empty());
    }

    TEST(ReplSetMonitorReadPref, SecPrefPriOkWithSecNotMatchTag) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getSingleNoMatchTag());

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryPreferred, tags, 3,
            &isPrimarySelected);

        ASSERT(isPrimarySelected);
        ASSERT_EQUALS("b", host.host());
    }

    TEST(ReplSetMonitorReadPref, NearestWithTags) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();

        BSONArrayBuilder arrayBuilder;
        arrayBuilder.append(BSON("p" << "1"));
        TagSet tags(arrayBuilder.arr());

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_Nearest, tags, 3,
            &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("a", host.host());
    }

    TEST(ReplSetMonitorReadPref, NearestWithTagsNoMatch) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getSingleNoMatchTag());

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_Nearest, tags, 3,
            &isPrimarySelected);

        ASSERT(host.empty());
    }

    TEST(ReplSetMonitorReadPref, MultiPriOnlyTag) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getMultiNoMatchTag());

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryOnly, tags, 3,
            &isPrimarySelected);

        ASSERT(isPrimarySelected);
        ASSERT_EQUALS("b", host.host());
    }

    TEST(ReplSetMonitorReadPref, MultiPriOnlyPriNotOkTag) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getMultiNoMatchTag());

        nodes[1].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryOnly, tags, 3,
            &isPrimarySelected);

        ASSERT(host.empty());
    }

    TEST(ReplSetMonitorReadPref, PriPrefPriOk) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();

        BSONArrayBuilder arrayBuilder;
        arrayBuilder.append(BSON("p" << "1"));
        arrayBuilder.append(BSON("p" << "2"));

        TagSet tags(arrayBuilder.arr());

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryPreferred, tags, 3,
            &isPrimarySelected);

        ASSERT(isPrimarySelected);
        ASSERT_EQUALS("b", host.host());
    }

    class MultiTags: public mongo::unittest::Test {
    public:
        vector<Node> getNodes() const {
            return NodeSetFixtures::getThreeMemberWithTags();
        }

        const TagSet& getMatchesFirstTagSet() {
            if (matchFirstTags.get() != NULL) {
                return *matchFirstTags;
            }

            BSONArrayBuilder arrayBuilder;
            arrayBuilder.append(BSON("p" << "1"));
            arrayBuilder.append(BSON("p" << "2"));
            matchFirstTags.reset(new TagSet(arrayBuilder.arr()));

            return *matchFirstTags;
        }

        const TagSet& getMatchesSecondTagSet() {
            if (matchSecondTags.get() != NULL) {
                return *matchSecondTags;
            }

            BSONArrayBuilder arrayBuilder;
            arrayBuilder.append(BSON("p" << "3"));
            arrayBuilder.append(BSON("p" << "2"));
            arrayBuilder.append(BSON("p" << "1"));
            matchSecondTags.reset(new TagSet(arrayBuilder.arr()));

            return *matchSecondTags;
        }

        const TagSet& getMatchesLastTagSet() {
            if (matchLastTags.get() != NULL) {
                return *matchLastTags;
            }

            BSONArrayBuilder arrayBuilder;
            arrayBuilder.append(BSON("p" << "12"));
            arrayBuilder.append(BSON("p" << "23"));
            arrayBuilder.append(BSON("p" << "19"));
            arrayBuilder.append(BSON("p" << "34"));
            arrayBuilder.append(BSON("p" << "1"));
            matchLastTags.reset(new TagSet(arrayBuilder.arr()));

            return *matchLastTags;
        }

        const TagSet& getMatchesPriTagSet() {
            if (matchPriTags.get() != NULL) {
                return *matchPriTags;
            }

            BSONArrayBuilder arrayBuilder;
            arrayBuilder.append(BSON("dc" << "sf"));
            arrayBuilder.append(BSON("p" << "1"));
            matchPriTags.reset(new TagSet(arrayBuilder.arr()));

            return *matchPriTags;
        }

    private:
        scoped_ptr<TagSet> matchFirstTags;
        scoped_ptr<TagSet> matchSecondTags;
        scoped_ptr<TagSet> matchLastTags;
        scoped_ptr<TagSet> matchPriTags;
    };

    TEST_F(MultiTags, MultiTagsMatchesFirst) {
        vector<Node> nodes = getNodes();

        nodes[1].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryPreferred, getMatchesFirstTagSet(),
            3, &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("a", host.host());
    }

    TEST_F(MultiTags, PriPrefPriNotOkMatchesFirstNotOk) {
        vector<Node> nodes = getNodes();

        nodes[0].markFailed();
        nodes[1].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryPreferred, getMatchesFirstTagSet(),
            3, &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("c", host.host());
    }

    TEST_F(MultiTags, PriPrefPriNotOkMatchesSecondTest) {
        vector<Node> nodes = getNodes();

        nodes[1].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryPreferred, getMatchesSecondTagSet(),
            3, &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("c", host.host());
    }

    TEST_F(MultiTags, PriPrefPriNotOkMatchesSecondNotOkTest) {
        vector<Node> nodes = getNodes();

        nodes[1].markFailed();
        nodes[2].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryPreferred, getMatchesSecondTagSet(),
            3, &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("a", host.host());
    }

    TEST_F(MultiTags, PriPrefPriNotOkMatchesLastTest) {
        vector<Node> nodes = getNodes();

        nodes[1].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryPreferred, getMatchesLastTagSet(),
            3, &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("a", host.host());
    }

    TEST_F(MultiTags, PriPrefPriNotOkMatchesLastNotOkTest) {
        vector<Node> nodes = getNodes();

        nodes[0].markFailed();
        nodes[1].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryPreferred, getMatchesLastTagSet(),
            3, &isPrimarySelected);

        ASSERT(host.empty());
    }

    TEST(MultiTagsXXX, PriPrefPriOkNoMatch) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();

        TagSet tags(TagSetFixtures::getMultiNoMatchTag());

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryPreferred, tags, 3,
            &isPrimarySelected);

        ASSERT(isPrimarySelected);
        ASSERT_EQUALS("b", host.host());
    }

    TEST(MultiTagsXXX, PriPrefPriNotOkNoMatch) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getMultiNoMatchTag());

        nodes[1].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_PrimaryPreferred, tags, 3,
            &isPrimarySelected);

        ASSERT(host.empty());
    }

    TEST_F(MultiTags, SecOnlyMatchesFirstTest) {
        vector<Node> nodes = getNodes();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryOnly, getMatchesFirstTagSet(),
            3, &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("a", host.host());
    }

    TEST_F(MultiTags, SecOnlyMatchesFirstNotOk) {
        vector<Node> nodes = getNodes();

        nodes[0].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryOnly, getMatchesFirstTagSet(),
            3, &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("c", host.host());
    }

    TEST_F(MultiTags, SecOnlyMatchesSecond) {
        vector<Node> nodes = getNodes();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryOnly, getMatchesSecondTagSet(),
            3, &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("c", host.host());
    }

    TEST_F(MultiTags, SecOnlyMatchesSecondNotOk) {
        vector<Node> nodes = getNodes();

        nodes[2].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryOnly, getMatchesSecondTagSet(),
            3, &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("a", host.host());
    }

    TEST_F(MultiTags, SecOnlyMatchesLast) {
        vector<Node> nodes = getNodes();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryOnly, getMatchesLastTagSet(),
            3, &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("a", host.host());
    }

    TEST_F(MultiTags, SecOnlyMatchesLastNotOk) {
        vector<Node> nodes = getNodes();

        nodes[0].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryOnly, getMatchesLastTagSet(),
            3, &isPrimarySelected);

        ASSERT(host.empty());
    }

    TEST_F(MultiTags, SecOnlyMultiTagsWithPriMatch) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
             mongo::ReadPreference_SecondaryOnly, getMatchesPriTagSet(),
             3, &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("a", host.host());
    }

    TEST_F(MultiTags, SecOnlyMultiTagsNoMatch) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getMultiNoMatchTag());

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
             mongo::ReadPreference_SecondaryOnly, tags, 3,
             &isPrimarySelected);

        ASSERT(host.empty());
    }

    TEST_F(MultiTags, SecPrefMatchesFirst) {
        vector<Node> nodes = getNodes();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryPreferred, getMatchesFirstTagSet(),
            3, &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("a", host.host());
    }

    TEST_F(MultiTags, SecPrefMatchesFirstNotOk) {
        vector<Node> nodes = getNodes();

        nodes[0].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryPreferred, getMatchesFirstTagSet(),
            3, &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("c", host.host());
    }

    TEST_F(MultiTags, SecPrefMatchesSecond) {
        vector<Node> nodes = getNodes();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryPreferred, getMatchesSecondTagSet(),
            3, &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("c", host.host());
    }

    TEST_F(MultiTags, SecPrefMatchesSecondNotOk) {
        vector<Node> nodes = getNodes();

        nodes[2].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryPreferred, getMatchesSecondTagSet(),
            3, &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("a", host.host());
    }

    TEST_F(MultiTags, SecPrefMatchesLast) {
        vector<Node> nodes = getNodes();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryPreferred, getMatchesLastTagSet(),
            3, &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("a", host.host());
    }

    TEST_F(MultiTags, SecPrefMatchesLastNotOk) {
        vector<Node> nodes = getNodes();

        nodes[0].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_SecondaryPreferred, getMatchesLastTagSet(),
            3, &isPrimarySelected);

        ASSERT(isPrimarySelected);
        ASSERT_EQUALS("b", host.host());
    }

    TEST_F(MultiTags, SecPrefMultiTagsWithPriMatch) {
        vector<Node> nodes = getNodes();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
             mongo::ReadPreference_SecondaryPreferred, getMatchesPriTagSet(),
             3, &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("a", host.host());
    }

    TEST(MultiTagsXXX, SecPrefMultiTagsNoMatch) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getMultiNoMatchTag());

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
             mongo::ReadPreference_SecondaryPreferred, tags, 3,
             &isPrimarySelected);

        ASSERT(isPrimarySelected);
        ASSERT_EQUALS("b", host.host());
    }

    TEST(MultiTagsXXX, SecPrefMultiTagsNoMatchPriNotOk) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getMultiNoMatchTag());

        nodes[1].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
             mongo::ReadPreference_SecondaryPreferred, tags, 3,
             &isPrimarySelected);

        ASSERT(host.empty());
    }

    TEST_F(MultiTags, NearestMatchesFirst) {
        vector<Node> nodes = getNodes();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_Nearest, getMatchesFirstTagSet(),
            3, &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("a", host.host());
    }

    TEST(MultiTagsXXX, NearestMatchesFirstNotOk) {
        vector<Node> nodes = NodeSetFixtures::getThreeMemberWithTags();

        BSONArrayBuilder arrayBuilder;
        arrayBuilder.append(BSON("p" << "1"));
        arrayBuilder.append(BSON("dc" << "sf"));

        TagSet tags(arrayBuilder.arr());

        nodes[0].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_Nearest, tags, 3,
            &isPrimarySelected);

        ASSERT(isPrimarySelected);
        ASSERT_EQUALS("b", host.host());
    }

    TEST_F(MultiTags, NearestMatchesSecond) {
        vector<Node> nodes = getNodes();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_Nearest, getMatchesSecondTagSet(), 3,
            &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("c", host.host());
    }

    TEST_F(MultiTags, NearestMatchesSecondNotOk) {
        vector<Node> nodes = NodeSetFixtures::getThreeMemberWithTags();

        BSONArrayBuilder arrayBuilder;
        arrayBuilder.append(BSON("z" << "2"));
        arrayBuilder.append(BSON("p" << "2"));
        arrayBuilder.append(BSON("dc" << "sf"));

        TagSet tags(arrayBuilder.arr());

        nodes[2].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_Nearest, tags, 3,
            &isPrimarySelected);

        ASSERT(isPrimarySelected);
        ASSERT_EQUALS("b", host.host());
    }

    TEST_F(MultiTags, NearestMatchesLast) {
        vector<Node> nodes = getNodes();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_Nearest, getMatchesLastTagSet(), 3,
            &isPrimarySelected);

        ASSERT(!isPrimarySelected);
        ASSERT_EQUALS("a", host.host());
    }

    TEST_F(MultiTags, NeatestMatchesLastNotOk) {
        vector<Node> nodes = getNodes();

        nodes[0].markFailed();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
            mongo::ReadPreference_Nearest, getMatchesLastTagSet(), 3,
            &isPrimarySelected);

        ASSERT(host.empty());
    }

    TEST_F(MultiTags, NearestMultiTagsWithPriMatch) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
             mongo::ReadPreference_Nearest, getMatchesPriTagSet(), 3,
             &isPrimarySelected);

        ASSERT(isPrimarySelected);
        ASSERT_EQUALS("b", host.host());
    }

    TEST(MultiTagsXXX, NearestMultiTagsNoMatch) {
        vector<Node> nodes =
                NodeSetFixtures::getThreeMemberWithTags();
        TagSet tags(TagSetFixtures::getMultiNoMatchTag());

        bool isPrimarySelected = false;
        HostAndPort host = selectNode(nodes,
             mongo::ReadPreference_Nearest, tags, 3,
             &isPrimarySelected);

        ASSERT(host.empty());
    }

    TEST(TagSet, DefaultConstructorMatchesAll) {
        TagSet tags;
        ASSERT_EQUALS(tags.getTagBSON(), BSON_ARRAY(BSONObj()));
    }


    // TODO: Port these existing tests here: replmonitor_bad_seed.js, repl_monitor_refresh.js

    /**
     * Warning: Tests running this fixture cannot be run in parallel with other tests
     * that uses ConnectionString::setConnectionHook
     */
    class ReplicaSetMonitorTest: public mongo::unittest::Test {
    protected:
        void setUp() {
            _replSet.reset(new MockReplicaSet("test", 3));
            _originalConnectionHook = ConnectionString::getConnectionHook();
            ConnectionString::setConnectionHook(
                    mongo::MockConnRegistry::get()->getConnStrHook());
            ReplicaSetMonitor::initialize();
        }

        void tearDown() {
            ConnectionString::setConnectionHook(_originalConnectionHook);
            ReplicaSetMonitor::shutdown();
            _replSet.reset();
        }

        MockReplicaSet* getReplSet() {
            return _replSet.get();
        }

    private:
        ConnectionString::ConnectionHook* _originalConnectionHook;
        boost::scoped_ptr<MockReplicaSet> _replSet;
    };

    TEST_F(ReplicaSetMonitorTest, SeedWithPriOnlySecDown) {
        // Test to make sure that the monitor doesn't crash when
        // ConnectionString::connect returns NULL
        MockReplicaSet* replSet = getReplSet();
        replSet->kill(replSet->getSecondaries());

        // Create a monitor with primary as the only seed list and the two secondaries
        // down so a NULL connection object will be stored for these secondaries in
        // the _nodes vector.
        const string replSetName(replSet->getSetName());
        set<HostAndPort> seedList;
        seedList.insert(HostAndPort(replSet->getPrimary()));
        ReplicaSetMonitor::createIfNeeded(replSetName, seedList);

        replSet->kill(replSet->getPrimary());

        ReplicaSetMonitorPtr monitor = ReplicaSetMonitor::get(replSet->getSetName());
        // Trigger calls to Node::getConnWithRefresh
        monitor->startOrContinueRefresh().refreshAll();
    }

    // Stress test case for a node that is previously a primary being removed from the set.
    // This test goes through configurations with different positions for the primary node
    // in the host list returned from the isMaster command. The test here is to make sure
    // that the ReplicaSetMonitor will not crash under these situations.
    TEST(ReplicaSetMonitorTestXXX, PrimaryRemovedFromSetStress) {
        const size_t NODE_COUNT = 5;
        MockReplicaSet replSet("test", NODE_COUNT);
        ConnectionString::ConnectionHook* originalConnHook =
                ConnectionString::getConnectionHook();
        ConnectionString::setConnectionHook(mongo::MockConnRegistry::get()->getConnStrHook());

        const string replSetName(replSet.getSetName());
        set<HostAndPort> seedList;
        seedList.insert(HostAndPort(replSet.getPrimary()));
        ReplicaSetMonitor::initialize();
        ReplicaSetMonitor::createIfNeeded(replSetName, seedList);

        const MockReplicaSet::ReplConfigMap origConfig = replSet.getReplConfig();
        mongo::ReplicaSetMonitorPtr replMonitor = ReplicaSetMonitor::get(replSetName);

        for (size_t idxToRemove = 0; idxToRemove < NODE_COUNT; idxToRemove++) {
            MockReplicaSet::ReplConfigMap newConfig = origConfig;

            replSet.setConfig(origConfig);
            // Make sure the monitor sees the change
            replMonitor->startOrContinueRefresh().refreshAll();

            string hostToRemove;
            {
                BSONObjBuilder monitorStateBuilder;
                replMonitor->appendInfo(monitorStateBuilder);
                BSONObj monitorState = monitorStateBuilder.done();

                BSONElement hostsElem = monitorState["hosts"];
                BSONElement addrElem = hostsElem[mongo::str::stream() << idxToRemove]["addr"];
                hostToRemove = addrElem.String();
            }

            replSet.setPrimary(hostToRemove);
            // Make sure the monitor sees the new primary
            replMonitor->startOrContinueRefresh().refreshAll();

            newConfig.erase(hostToRemove);
            replSet.setConfig(newConfig);
            replSet.setPrimary(newConfig.begin()->first);
            // Force refresh -> should not crash
            replMonitor->startOrContinueRefresh().refreshAll();
        }

        ReplicaSetMonitor::shutdown();
        ConnectionString::setConnectionHook(originalConnHook);
    }

    /**
     * Warning: Tests running this fixture cannot be run in parallel with other tests
     * that use ConnectionString::setConnectionHook.
     */
    class TwoNodeWithTags: public mongo::unittest::Test {
    protected:
        void setUp() {
            _replSet.reset(new MockReplicaSet("test", 2));
            _originalConnectionHook = ConnectionString::getConnectionHook();
            ConnectionString::setConnectionHook(
                    mongo::MockConnRegistry::get()->getConnStrHook());

            mongo::MockReplicaSet::ReplConfigMap config = _replSet->getReplConfig();

            {
                const string host(_replSet->getPrimary());
                map<string, string>& tag = config[host].tags;
                tag.clear();
                tag["dc"] = "ny";
                tag["num"] = "1";
            }

            {
                const string host(_replSet->getSecondaries().front());
                map<string, string>&  tag = config[host].tags;
                tag.clear();
                tag["dc"] = "ny";
                tag["num"] = "2";
            }

            _replSet->setConfig(config);
            ReplicaSetMonitor::initialize();

        }

        void tearDown() {
            ConnectionString::setConnectionHook(_originalConnectionHook);
            ReplicaSetMonitor::shutdown();
            _replSet.reset();
        }

        MockReplicaSet* getReplSet() {
            return _replSet.get();
        }

    private:
        ConnectionString::ConnectionHook* _originalConnectionHook;
        boost::scoped_ptr<MockReplicaSet> _replSet;
    };

    // Tests the case where the connection to secondary went bad and the replica set
    // monitor needs to perform a refresh of it's local view then retry the node selection
    // again after the refresh.
    TEST_F(TwoNodeWithTags, SecDownRetryNoTag) {
        MockReplicaSet* replSet = getReplSet();

        set<HostAndPort> seedList;
        seedList.insert(HostAndPort(replSet->getPrimary()));
        ReplicaSetMonitor::createIfNeeded(replSet->getSetName(), seedList);

        const string secHost(replSet->getSecondaries().front());
        replSet->kill(secHost);

        ReplicaSetMonitorPtr monitor = ReplicaSetMonitor::get(replSet->getSetName());
        // Make sure monitor sees the dead secondary
        monitor->startOrContinueRefresh().refreshAll();

        replSet->restore(secHost);

        HostAndPort node = monitor->getHostOrRefresh(
            ReadPreferenceSetting(mongo::ReadPreference_SecondaryOnly, TagSet()));

        ASSERT_FALSE(monitor->isPrimary(node));
        ASSERT_EQUALS(secHost, node.toString());
    }

    // Tests the case where the connection to secondary went bad and the replica set
    // monitor needs to perform a refresh of it's local view then retry the node selection
    // with tags again after the refresh.
    TEST_F(TwoNodeWithTags, SecDownRetryWithTag) {
        MockReplicaSet* replSet = getReplSet();

        set<HostAndPort> seedList;
        seedList.insert(HostAndPort(replSet->getPrimary()));
        ReplicaSetMonitor::createIfNeeded(replSet->getSetName(), seedList);

        const string secHost(replSet->getSecondaries().front());
        replSet->kill(secHost);

        ReplicaSetMonitorPtr monitor = ReplicaSetMonitor::get(replSet->getSetName());
        // Make sure monitor sees the dead secondary
        monitor->startOrContinueRefresh().refreshAll();

        replSet->restore(secHost);

        TagSet tags(BSON_ARRAY(BSON("dc" << "ny")));
        HostAndPort node = monitor->getHostOrRefresh(
            ReadPreferenceSetting(mongo::ReadPreference_SecondaryOnly, tags));

        ASSERT_FALSE(monitor->isPrimary(node));
        ASSERT_EQUALS(secHost, node.toString());
    }
}
