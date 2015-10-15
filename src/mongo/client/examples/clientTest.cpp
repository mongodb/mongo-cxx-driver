// clientTest.cpp

/*    Copyright 2009 10gen Inc.
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
 * a simple test for the c++ driver
 */

// It is the responsibility of the mongo client consumer to ensure that any necessary windows
// headers have already been included before including the driver facade headers.
#if defined(_WIN32)
#include <winsock2.h>
#include <windows.h>
#endif

// this header should be first to ensure that it includes cleanly in any context
#include "mongo/client/dbclient.h"

#include <iostream>
#include <set>

#ifndef verify
#define verify(x) MONGO_verify(x)
#endif

using namespace std;
using namespace mongo;

int main(int argc, const char** argv) {
    if (argc > 2) {
        std::cout << "usage: " << argv[0] << " [MONGODB_URI]" << std::endl;
        return EXIT_FAILURE;
    }

    mongo::client::GlobalInstance instance;
    if (!instance.initialized()) {
        std::cout << "failed to initialize the client driver: " << instance.status() << std::endl;
        return EXIT_FAILURE;
    }

    std::string uri = argc == 2 ? argv[1] : "mongodb://localhost:27017";
    std::string errmsg;

    ConnectionString cs = ConnectionString::parse(uri, errmsg);

    if (!cs.isValid()) {
        std::cout << "Error parsing connection string " << uri << ": " << errmsg << std::endl;
        return EXIT_FAILURE;
    }

    boost::scoped_ptr<DBClientBase> conn(cs.connect(errmsg));

    const char* ns = "test.test1";

    conn->dropCollection(ns);

    // clean up old data from any previous tests
    conn->remove(ns, BSONObj());
    verify(conn->findOne(ns, BSONObj()).isEmpty());

    // test insert
    conn->insert(ns,
                 BSON("name"
                      << "eliot"
                      << "num" << 1));
    verify(!conn->findOne(ns, BSONObj()).isEmpty());

    // test remove
    conn->remove(ns, BSONObj());
    verify(conn->findOne(ns, BSONObj()).isEmpty());


    // insert, findOne testing
    conn->insert(ns,
                 BSON("name"
                      << "eliot"
                      << "num" << 1));
    {
        BSONObj res = conn->findOne(ns, BSONObj());
        verify(strstr(res.getStringField("name"), "eliot"));
        verify(!strstr(res.getStringField("name2"), "eliot"));
        verify(1 == res.getIntField("num"));
    }


    // cursor
    conn->insert(ns,
                 BSON("name"
                      << "sara"
                      << "num" << 2));
    {
        auto_ptr<DBClientCursor> cursor = conn->query(ns, BSONObj());
        int count = 0;
        while (cursor->more()) {
            count++;
            BSONObj obj = cursor->next();
        }
        verify(count == 2);
    }

    {
        auto_ptr<DBClientCursor> cursor = conn->query(ns, BSON("num" << 1));
        int count = 0;
        while (cursor->more()) {
            count++;
            BSONObj obj = cursor->next();
        }
        verify(count == 1);
    }

    {
        auto_ptr<DBClientCursor> cursor = conn->query(ns, BSON("num" << 3));
        int count = 0;
        while (cursor->more()) {
            count++;
            BSONObj obj = cursor->next();
        }
        verify(count == 0);
    }

    // update
    {
        BSONObj res = conn->findOne(ns, BSONObjBuilder().append("name", "eliot").obj());
        verify(!strstr(res.getStringField("name2"), "eliot"));

        BSONObj after = BSONObjBuilder().appendElements(res).append("name2", "h").obj();

        conn->update(ns, BSONObjBuilder().append("name", "eliot2").obj(), after);
        res = conn->findOne(ns, BSONObjBuilder().append("name", "eliot").obj());
        verify(!strstr(res.getStringField("name2"), "eliot"));
        verify(conn->findOne(ns, BSONObjBuilder().append("name", "eliot2").obj()).isEmpty());

        conn->update(ns, BSONObjBuilder().append("name", "eliot").obj(), after);
        res = conn->findOne(ns, BSONObjBuilder().append("name", "eliot").obj());
        verify(strstr(res.getStringField("name"), "eliot"));
        verify(strstr(res.getStringField("name2"), "h"));
        verify(conn->findOne(ns, BSONObjBuilder().append("name", "eliot2").obj()).isEmpty());

        // upsert
        try {
            conn->update(
                ns, BSONObjBuilder().append("name", "eliot2").obj(), after, UpdateOption_Upsert);
        } catch (OperationException&) {
            // This upsert throws an OperationException because of a duplicate key error:
            // The upserted document has the same _id as one already in the collection
        }
        verify(!conn->findOne(ns, BSONObjBuilder().append("name", "eliot").obj()).isEmpty());
    }

    // Create an index on 'name'
    conn->createIndex(ns, BSON("name" << 1));

    /*
        {
            // 5 second TTL index
            const char * ttlns = "test.ttltest1";
            conn->dropCollection( ttlns );

            {
                mongo::BSONObjBuilder b;
                b.appendTimeT("ttltime", time(0));
                b.append("name", "foo");
                conn->insert(ttlns, b.obj());
            }
            conn->ensureIndex(ttlns, BSON("ttltime" << 1), false, "", true, false, -1, 5);
            verify(!conn->findOne(ttlns, BSONObjBuilder().append("name", "foo").obj()).isEmpty());
            // Sleep 66 seconds, 60 seconds for the TTL loop, 5 seconds for the TTL and 1 to ensure
            sleepsecs(66);
            verify(conn->findOne(ttlns, BSONObjBuilder().append("name", "foo").obj()).isEmpty());
        }
    */
    {
        // hint related tests
        verify(conn->findOne(ns, "{ name : 'eliot' }")["name"].str() == "eliot");
        verify(conn->getLastError() == "");

        // nonexistent index test
        bool asserted = false;
        try {
            conn->findOne(ns, Query("{name:\"eliot\"}").hint("foo_1}"));
        } catch (...) {
            asserted = true;
        }
        verify(asserted);

        // existing index
        verify(conn->findOne(ns, Query("{name:'eliot'}").hint("name_1")).hasElement("name"));

        // run validate
        verify(conn->validate(ns));
    }

    {
        // timestamp test

        const char* tsns = "test.tstest1";
        conn->dropCollection(tsns);

        {
            mongo::BSONObjBuilder b;
            b.appendTimestamp("ts", Timestamp_t());
            conn->insert(tsns, b.obj());
        }

        mongo::BSONObj out = conn->findOne(tsns, mongo::BSONObj());
        uint32_t oldTime = out["ts"].timestamp().seconds();
        uint32_t oldInc = out["ts"].timestamp().increment();

        {
            mongo::BSONObjBuilder b1;
            b1.append(out["_id"]);

            mongo::BSONObjBuilder b2;
            b2.append(out["_id"]);
            b2.appendTimestamp("ts", Timestamp_t());

            conn->update(tsns, b1.obj(), b2.obj());
        }

        BSONObj found = conn->findOne(tsns, mongo::BSONObj());
        cout << "old: " << out << "\nnew: " << found << endl;
        verify((oldTime < found["ts"].timestamp().seconds()) ||
               (oldTime == found["ts"].timestamp().seconds() &&
                oldInc < found["ts"].timestamp().increment()));
    }

    {
        // check that killcursors doesn't affect last error
        verify(conn->getLastError().empty());

        BufBuilder b;
        b.appendNum((int)0);   // reserved
        b.appendNum((int)-1);  // invalid # of cursors triggers exception
        b.appendNum((int)-1);  // bogus cursor id

        Message m;
        m.setData(dbKillCursors, b.buf(), b.len());

        // say() is protected in DBClientConnection, so get superclass
        static_cast<DBConnector*>(conn.get())->say(m);

        verify(conn->getLastError().empty());
    }

    {
        list<string> l = conn->getDatabaseNames();
        for (list<string>::iterator i = l.begin(); i != l.end(); i++) {
            cout << "db name : " << *i << endl;
        }

        l = conn->getCollectionNames("test");
        for (list<string>::iterator i = l.begin(); i != l.end(); i++) {
            cout << "coll name : " << *i << endl;
        }
    }

    {
        const string ns = "test.listMyIndexes";
        conn->dropCollection(ns);
        conn->insert(ns, BSON("a" << 1));
        conn->createIndex(ns, BSON("a" << 1));
        conn->createIndex(ns, BSON("b" << 1));
        conn->createIndex(ns, BSON("c" << 1));
        list<string> indexNames(conn->getIndexNames(ns));
        std::multiset<string> names(indexNames.begin(), indexNames.end());
        verify(indexNames.size() == 4);
        verify(names.count(string("_id_")) == 1);
        verify(names.count(string("a_1")) == 1);
        verify(names.count(string("b_1")) == 1);
        verify(names.count(string("c_1")) == 1);
    }

    {
        // Map Reduce (this mostly just tests that it compiles with all output types)
        const string ns = "test.mr";
        conn->insert(ns, BSON("a" << 1));
        conn->insert(ns, BSON("a" << 1));

        const char* map = "function() { emit(this.a, 1); }";
        const char* reduce = "function(key, values) { return Array.sum(values); }";

        const string outcoll = ns + ".out";

        BSONObj out;
        out = conn->mapreduce(ns, map, reduce, BSONObj());  // default to inline
        // MONGO_PRINT(out);
        out = conn->mapreduce(ns, map, reduce, BSONObj(), outcoll);
        // MONGO_PRINT(out);
        out = conn->mapreduce(ns, map, reduce, BSONObj(), outcoll.c_str());
        // MONGO_PRINT(out);
        out = conn->mapreduce(ns, map, reduce, BSONObj(), BSON("reduce" << outcoll));
        // MONGO_PRINT(out);
    }

    {
        // test timeouts

        boost::scoped_ptr<DBClientBase> conn(cs.connect(errmsg, 2));
        if (!conn) {
            cout << "couldn't connect : " << errmsg << endl;
            throw - 11;
        }
        conn->insert("test.totest", BSON("x" << 1));
        BSONObj res;

        bool gotError = false;
        verify(conn->evalDeprecated("test", "return db.totest.findOne().x", res));
        try {
            conn->evalDeprecated("test", "sleep(5000); return db.totest.findOne().x", res);
        } catch (std::exception& e) {
            gotError = true;
            std::cout << e.what() << endl;
        }
        verify(gotError);
        // sleep so the server isn't locked anymore
        sleepsecs(4);

        verify(conn->evalDeprecated("test", "return db.totest.findOne().x", res));
    }

    cout << "client test finished!" << endl;
    return EXIT_SUCCESS;
}
