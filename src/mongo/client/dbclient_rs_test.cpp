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

/**
 * This file contains tests for DBClientReplicaSet. The tests mocks the servers
 * the DBClientReplicaSet talks to, so the tests only covers the client side logic.
 */

#include "mongo/platform/basic.h"

#include "mongo/bson/bson_field.h"
#include "mongo/client/dbclientcursor.h"
#include "mongo/client/dbclientinterface.h"
#include "mongo/client/dbclient_rs.h"
#include "mongo/client/replica_set_monitor.h"
#include "mongo/dbtests/mock/mock_conn_registry.h"
#include "mongo/dbtests/mock/mock_replica_set.h"
#include "mongo/unittest/unittest.h"
#include "mongo/util/assert_util.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace {
    using boost::scoped_ptr;
    using std::auto_ptr;
    using std::map;
    using std::make_pair;
    using std::pair;
    using std::string;
    using std::vector;

    using mongo::AssertionException;
    using mongo::BSONArray;
    using mongo::BSONElement;
    using mongo::BSONField;
    using mongo::BSONObj;
    using mongo::ConnectionString;
    using mongo::DBClientCursor;
    using mongo::DBClientReplicaSet;
    using mongo::HostAndPort;
    using mongo::HostField;
    using mongo::IdentityNS;
    using mongo::MockReplicaSet;
    using mongo::Query;
    using mongo::ReadPreference;
    using mongo::ReplicaSetMonitor;
    using mongo::TagSet;

    /**
     * Basic fixture with one primary and one secondary.
     */
    class BasicRS: public mongo::unittest::Test {
    protected:
        void setUp() {
            ReplicaSetMonitor::initialize();
            _replSet.reset(new MockReplicaSet("test", 2));
            ConnectionString::setConnectionHook(
                    mongo::MockConnRegistry::get()->getConnStrHook());
        }

        void tearDown() {
            ReplicaSetMonitor::shutdown();
            _replSet.reset();
        }

        MockReplicaSet* getReplSet() {
            return _replSet.get();
        }

    private:
        boost::scoped_ptr<MockReplicaSet> _replSet;
    };

    TEST_F(BasicRS, ReadFromPrimary) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        Query query;
        query.readPref(mongo::ReadPreference_PrimaryOnly, BSONArray());

        // Note: IdentityNS contains the name of the server.
        auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
        BSONObj doc = cursor->next();
        ASSERT_EQUALS(replSet->getPrimary(), doc[HostField.name()].str());
    }

    TEST_F(BasicRS, SecondaryOnly) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        Query query;
        query.readPref(mongo::ReadPreference_SecondaryOnly, BSONArray());

        // Note: IdentityNS contains the name of the server.
        auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
        BSONObj doc = cursor->next();
        ASSERT_EQUALS(replSet->getSecondaries().front(), doc[HostField.name()].str());
    }

    TEST_F(BasicRS, PrimaryPreferred) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        // Need up-to-date view, since either host is valid if view is stale.
        ReplicaSetMonitor::get(replSet->getSetName())->startOrContinueRefresh().refreshAll();

        Query query;
        query.readPref(mongo::ReadPreference_PrimaryPreferred, BSONArray());

        // Note: IdentityNS contains the name of the server.
        auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
        BSONObj doc = cursor->next();
        ASSERT_EQUALS(replSet->getPrimary(), doc[HostField.name()].str());
    }

    TEST_F(BasicRS, SecondaryPreferred) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        // Need up-to-date view, since either host is valid if view is stale.
        ReplicaSetMonitor::get(replSet->getSetName())->startOrContinueRefresh().refreshAll();

        Query query;
        query.readPref(mongo::ReadPreference_SecondaryPreferred, BSONArray());

        // Note: IdentityNS contains the name of the server.
        auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
        BSONObj doc = cursor->next();
        ASSERT_EQUALS(replSet->getSecondaries().front(), doc[HostField.name()].str());
    }

    /**
     * Setup for 2 member replica set will all of the nodes down.
     */
    class AllNodesDown: public mongo::unittest::Test {
    protected:
        void setUp() {
            ReplicaSetMonitor::initialize();
            _replSet.reset(new MockReplicaSet("test", 2));
            ConnectionString::setConnectionHook(
                    mongo::MockConnRegistry::get()->getConnStrHook());

            vector<HostAndPort> hostList(_replSet->getHosts());
            for (vector<HostAndPort>::const_iterator iter = hostList.begin();
                    iter != hostList.end(); ++iter) {
                _replSet->kill(iter->toString());
            }
        }

        void tearDown() {
            ReplicaSetMonitor::shutdown();
            _replSet.reset();
        }

        MockReplicaSet* getReplSet() {
            return _replSet.get();
        }

    private:
        boost::scoped_ptr<MockReplicaSet> _replSet;
    };

    TEST_F(AllNodesDown, ReadFromPrimary) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        Query query;
        query.readPref(mongo::ReadPreference_PrimaryOnly, BSONArray());
        ASSERT_THROWS(replConn.query(IdentityNS, query), AssertionException);
    }

    TEST_F(AllNodesDown, SecondaryOnly) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        Query query;
        query.readPref(mongo::ReadPreference_SecondaryOnly, BSONArray());
        ASSERT_THROWS(replConn.query(IdentityNS, query), AssertionException);
    }

    TEST_F(AllNodesDown, PrimaryPreferred) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        Query query;
        query.readPref(mongo::ReadPreference_PrimaryPreferred, BSONArray());
        ASSERT_THROWS(replConn.query(IdentityNS, query), AssertionException);
    }

    TEST_F(AllNodesDown, SecondaryPreferred) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        Query query;
        query.readPref(mongo::ReadPreference_SecondaryPreferred, BSONArray());
        ASSERT_THROWS(replConn.query(IdentityNS, query), AssertionException);
    }

    TEST_F(AllNodesDown, Nearest) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        Query query;
        query.readPref(mongo::ReadPreference_Nearest, BSONArray());
        ASSERT_THROWS(replConn.query(IdentityNS, query), AssertionException);
    }

    /**
     * Setup for 2 member replica set with the primary down.
     */
    class PrimaryDown: public mongo::unittest::Test {
    protected:
        void setUp() {
            ReplicaSetMonitor::initialize();
            _replSet.reset(new MockReplicaSet("test", 2));
            ConnectionString::setConnectionHook(
                    mongo::MockConnRegistry::get()->getConnStrHook());
            _replSet->kill(_replSet->getPrimary());
        }

        void tearDown() {
            ReplicaSetMonitor::shutdown();
            _replSet.reset();
        }

        MockReplicaSet* getReplSet() {
            return _replSet.get();
        }

    private:
        boost::scoped_ptr<MockReplicaSet> _replSet;
    };

    TEST_F(PrimaryDown, ReadFromPrimary) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        Query query;
        query.readPref(mongo::ReadPreference_PrimaryOnly, BSONArray());
        ASSERT_THROWS(replConn.query(IdentityNS, query), AssertionException);
    }

    TEST_F(PrimaryDown, SecondaryOnly) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        Query query;
        query.readPref(mongo::ReadPreference_SecondaryOnly, BSONArray());

        // Note: IdentityNS contains the name of the server.
        auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
        BSONObj doc = cursor->next();
        ASSERT_EQUALS(replSet->getSecondaries().front(), doc[HostField.name()].str());
    }

    TEST_F(PrimaryDown, PrimaryPreferred) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        Query query;
        query.readPref(mongo::ReadPreference_PrimaryPreferred, BSONArray());

        // Note: IdentityNS contains the name of the server.
        auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
        BSONObj doc = cursor->next();
        ASSERT_EQUALS(replSet->getSecondaries().front(), doc[HostField.name()].str());
    }

    TEST_F(PrimaryDown, SecondaryPreferred) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        Query query;
        query.readPref(mongo::ReadPreference_SecondaryPreferred, BSONArray());

        // Note: IdentityNS contains the name of the server.
        auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
        BSONObj doc = cursor->next();
        ASSERT_EQUALS(replSet->getSecondaries().front(), doc[HostField.name()].str());
    }

    TEST_F(PrimaryDown, Nearest) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        Query query;
        query.readPref(mongo::ReadPreference_Nearest, BSONArray());
        auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
        BSONObj doc = cursor->next();
        ASSERT_EQUALS(replSet->getSecondaries().front(), doc[HostField.name()].str());
    }

    /**
     * Setup for 2 member replica set with the secondary down.
     */
    class SecondaryDown: public mongo::unittest::Test {
    protected:
        void setUp() {
            ReplicaSetMonitor::initialize();
            _replSet.reset(new MockReplicaSet("test", 2));
            ConnectionString::setConnectionHook(
                    mongo::MockConnRegistry::get()->getConnStrHook());

            _replSet->kill(_replSet->getSecondaries().front());
        }

        void tearDown() {
            ReplicaSetMonitor::shutdown();
            _replSet.reset();

        }

        MockReplicaSet* getReplSet() {
            return _replSet.get();
        }

    private:
        boost::scoped_ptr<MockReplicaSet> _replSet;
    };

    TEST_F(SecondaryDown, ReadFromPrimary) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        Query query;
        query.readPref(mongo::ReadPreference_PrimaryOnly, BSONArray());

        // Note: IdentityNS contains the name of the server.
        auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
        BSONObj doc = cursor->next();
        ASSERT_EQUALS(replSet->getPrimary(), doc[HostField.name()].str());
    }

    TEST_F(SecondaryDown, SecondaryOnly) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        Query query;
        query.readPref(mongo::ReadPreference_SecondaryOnly, BSONArray());
        ASSERT_THROWS(replConn.query(IdentityNS, query), AssertionException);
    }

    TEST_F(SecondaryDown, PrimaryPreferred) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        Query query;
        query.readPref(mongo::ReadPreference_PrimaryPreferred, BSONArray());

        // Note: IdentityNS contains the name of the server.
        auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
        BSONObj doc = cursor->next();
        ASSERT_EQUALS(replSet->getPrimary(), doc[HostField.name()].str());
    }

    TEST_F(SecondaryDown, SecondaryPreferred) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        Query query;
        query.readPref(mongo::ReadPreference_SecondaryPreferred, BSONArray());

        // Note: IdentityNS contains the name of the server.
        auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
        BSONObj doc = cursor->next();
        ASSERT_EQUALS(replSet->getPrimary(), doc[HostField.name()].str());
    }

    TEST_F(SecondaryDown, Nearest) {
        MockReplicaSet* replSet = getReplSet();
        DBClientReplicaSet replConn(replSet->getSetName(), replSet->getHosts());

        Query query;
        query.readPref(mongo::ReadPreference_Nearest, BSONArray());

        // Note: IdentityNS contains the name of the server.
        auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
        BSONObj doc = cursor->next();
        ASSERT_EQUALS(replSet->getPrimary(), doc[HostField.name()].str());
    }

    /**
     * Warning: Tests running this fixture cannot be run in parallel with other tests
     * that uses ConnectionString::setConnectionHook
     */
    class TaggedFiveMemberRS: public mongo::unittest::Test {
    protected:
        void setUp() {
            // Tests for pinning behavior require this.
            ReplicaSetMonitor::useDeterministicHostSelection = true;

            // This shuts down the background RSMWatcher thread and prevents it from running. These
            // tests depend on controlling when the RSMs are updated.
            ReplicaSetMonitor::initialize();

            _replSet.reset(new MockReplicaSet("test", 5));
            _originalConnectionHook = ConnectionString::getConnectionHook();
            ConnectionString::setConnectionHook(
                    mongo::MockConnRegistry::get()->getConnStrHook());

            {
                mongo::MockReplicaSet::ReplConfigMap config = _replSet->getReplConfig();

                {
                    const string host(_replSet->getPrimary());
                    map<string, string>& tag = config[host].tags;
                    tag.clear();
                    tag["dc"] = "ny";
                    tag["p"] = "1";
                    _replSet->getNode(host)->insert(IdentityNS, BSON(HostField(host)));
                }

                vector<string> secNodes = _replSet->getSecondaries();
                vector<string>::const_iterator secIter = secNodes.begin();

                {
                    const string host(*secIter);
                    map<string, string>&  tag = config[host].tags;
                    tag.clear();
                    tag["dc"] = "sf";
                    tag["s"] = "1";
                    tag["group"] = "1";
                    _replSet->getNode(host)->insert(IdentityNS, BSON(HostField(host)));
                }

                {
                    ++secIter;
                    const string host(*secIter);
                    map<string, string>&  tag = config[host].tags;
                    tag.clear();
                    tag["dc"] = "ma";
                    tag["s"] = "2";
                    tag["group"] = "1";
                    _replSet->getNode(host)->insert(IdentityNS, BSON(HostField(host)));
                }

                {
                    ++secIter;
                    const string host(*secIter);
                    map<string, string>&  tag = config[host].tags;
                    tag.clear();
                    tag["dc"] = "eu";
                    tag["s"] = "3";
                    _replSet->getNode(host)->insert(IdentityNS, BSON(HostField(host)));
                }

                {
                    ++secIter;
                    const string host(*secIter);
                    map<string, string>&  tag = config[host].tags;
                    tag.clear();
                    tag["dc"] = "jp";
                    tag["s"] = "4";
                    _replSet->getNode(host)->insert(IdentityNS, BSON(HostField(host)));
                }

                _replSet->setConfig(config);
            }
        }

        void tearDown() {
            ReplicaSetMonitor::useDeterministicHostSelection = false;

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

    TEST_F(TaggedFiveMemberRS, ConnShouldPinIfSameSettings) {
        MockReplicaSet* replSet = getReplSet();
        vector<HostAndPort> seedList;
        seedList.push_back(HostAndPort(replSet->getPrimary()));

        DBClientReplicaSet replConn(replSet->getSetName(), seedList);

        string dest;
        {
            Query query;
            query.readPref(mongo::ReadPreference_PrimaryPreferred, BSONArray());

            // Note: IdentityNS contains the name of the server.
            auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
            BSONObj doc = cursor->next();
            dest = doc[HostField.name()].str();
        }

        {
            Query query;
            query.readPref(mongo::ReadPreference_PrimaryPreferred, BSONArray());
            auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
            BSONObj doc = cursor->next();
            const string newDest = doc[HostField.name()].str();
            ASSERT_EQUALS(dest, newDest);
        }
    }

    TEST_F(TaggedFiveMemberRS, ConnShouldNotPinIfHostMarkedAsFailed) {
        MockReplicaSet* replSet = getReplSet();
        vector<HostAndPort> seedList;
        seedList.push_back(HostAndPort(replSet->getPrimary()));

        DBClientReplicaSet replConn(replSet->getSetName(), seedList);

        string dest;
        {
            Query query;
            query.readPref(mongo::ReadPreference_PrimaryPreferred, BSONArray());

            // Note: IdentityNS contains the name of the server.
            auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
            BSONObj doc = cursor->next();
            dest = doc[HostField.name()].str();
        }

        // This is the only difference from ConnShouldPinIfSameSettings which tests that we *do* pin
        // in if the host is still marked as up. Note that this only notifies the RSM, and does not
        // directly effect the DBClientRS.
        ReplicaSetMonitor::get(replSet->getSetName())->failedHost(HostAndPort(dest));

        {
            Query query;
            query.readPref(mongo::ReadPreference_PrimaryPreferred, BSONArray());
            auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
            BSONObj doc = cursor->next();
            const string newDest = doc[HostField.name()].str();
            ASSERT_NOT_EQUALS(dest, newDest);
        }
    }

    TEST_F(TaggedFiveMemberRS, ConnShouldNotPinIfDiffMode) {
        MockReplicaSet* replSet = getReplSet();
        vector<HostAndPort> seedList;
        seedList.push_back(HostAndPort(replSet->getPrimary()));

        DBClientReplicaSet replConn(replSet->getSetName(), seedList);

        // Need up-to-date view to ensure there are multiple valid choices.
        ReplicaSetMonitor::get(replSet->getSetName())->startOrContinueRefresh().refreshAll();

        string dest;
        {
            Query query;
            query.readPref(mongo::ReadPreference_SecondaryPreferred, BSONArray());

            // Note: IdentityNS contains the name of the server.
            auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
            BSONObj doc = cursor->next();
            dest = doc[HostField.name()].str();
            ASSERT_NOT_EQUALS(dest, replSet->getPrimary());
        }

        {
            Query query;
            query.readPref(mongo::ReadPreference_SecondaryOnly, BSONArray());
            auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
            BSONObj doc = cursor->next();
            const string newDest = doc[HostField.name()].str();
            ASSERT_NOT_EQUALS(dest, newDest);
        }
    }

    TEST_F(TaggedFiveMemberRS, ConnShouldNotPinIfDiffTag) {
        MockReplicaSet* replSet = getReplSet();
        vector<HostAndPort> seedList;
        seedList.push_back(HostAndPort(replSet->getPrimary()));

        DBClientReplicaSet replConn(replSet->getSetName(), seedList);

        // Need up-to-date view to ensure there are multiple valid choices.
        ReplicaSetMonitor::get(replSet->getSetName())->startOrContinueRefresh().refreshAll();

        string dest;
        {
            Query query;
            query.readPref(mongo::ReadPreference_SecondaryPreferred,
                    BSON_ARRAY(BSON("dc" << "sf")));

            // Note: IdentityNS contains the name of the server.
            auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
            BSONObj doc = cursor->next();
            dest = doc[HostField.name()].str();
            ASSERT_NOT_EQUALS(dest, replSet->getPrimary());
        }

        {
            Query query;
            vector<pair<string, string> > tagSet;
            query.readPref(mongo::ReadPreference_SecondaryPreferred,
                    BSON_ARRAY(BSON("group" << 1)));
            auto_ptr<DBClientCursor> cursor = replConn.query(IdentityNS, query);
            BSONObj doc = cursor->next();
            const string newDest = doc[HostField.name()].str();
            ASSERT_NOT_EQUALS(dest, newDest);
        }
    }

    // Note: slaveConn is dangerous and should be deprecated! Also see SERVER-7801.
    TEST_F(TaggedFiveMemberRS, SlaveConnReturnsSecConn) {
        MockReplicaSet* replSet = getReplSet();
        vector<HostAndPort> seedList;
        seedList.push_back(HostAndPort(replSet->getPrimary()));

        DBClientReplicaSet replConn(replSet->getSetName(), seedList);

        // Need up-to-date view since slaveConn() uses SecondaryPreferred, and this test assumes it
        // knows about at least one secondary.
        ReplicaSetMonitor::get(replSet->getSetName())->startOrContinueRefresh().refreshAll();

        string dest;
        mongo::DBClientConnection& secConn = replConn.slaveConn();

        // Note: IdentityNS contains the name of the server.
        auto_ptr<DBClientCursor> cursor = secConn.query(IdentityNS, Query());
        BSONObj doc = cursor->next();
        dest = doc[HostField.name()].str();
        ASSERT_NOT_EQUALS(dest, replSet->getPrimary());
    }
}
