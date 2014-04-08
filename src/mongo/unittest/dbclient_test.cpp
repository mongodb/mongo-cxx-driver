#include <boost/scoped_ptr.hpp>
#include <string>

#include "mongo/unittest/integration_test.h"

#include "mongo/bson/bson.h"
#include "mongo/client/dbclient.h"

using std::string;
using std::list;
using std::auto_ptr;
using namespace mongo;
using namespace mongo::unittest;
using namespace bson;

namespace {
    const string TEST_NS = "test.foo";

    class DBClientTest : public ::testing::Test {
    public:
        DBClientTest() {
            c.connect(string("localhost:") + integrationTestParams.port);
            c.dropCollection("test.foo");
        }
        ~DBClientTest() {
        }
        DBClientConnection c;
    };
}

/*
 * From simple_client_demo.cpp
 */
TEST_F(DBClientTest, Count) {
    unsigned long long count = c.count("test.foo");
    ASSERT_EQUALS(count, 0);

    bo o = BSON( "hello" << "world" );
    c.insert(TEST_NS, o);
    string e = c.getLastError();

    ASSERT_TRUE(e.empty());

}

TEST_F(DBClientTest, CreateIndex) {
    // make an index with a unique key constraint
    c.ensureIndex(TEST_NS, BSON("hello"<<1), /*unique*/true);

    bo o = BSON( "hello" << "world" );
    c.insert(TEST_NS, o);
    c.insert(TEST_NS, o); // will cause a dup key error on "hello" field
    ASSERT_FALSE(c.getLastError().empty());
}

/*
 * From insert_demo.cpp
 */
TEST_F(DBClientTest, LargeInsert) {
    bo o = BSON("hello" << "world");

    for(unsigned i = 0; i < 100000; i++) {
        c.insert(TEST_NS, o);
    }

    // wait until all operations applied
    ASSERT_TRUE(c.getLastError().empty());
}

/*
 * From clientTest.cpp
 */
TEST_F(DBClientTest, Insert) {
    c.insert(TEST_NS, BSON("name" << "Tyler"));
    ASSERT_FALSE(c.findOne(TEST_NS, BSONObj()).isEmpty());
}

TEST_F(DBClientTest, Remove) {
    c.insert(TEST_NS, BSON("name" << "Tyler"));
    c.remove(TEST_NS, BSONObj());
    ASSERT_TRUE(c.findOne(TEST_NS, BSONObj()).isEmpty());
}

TEST_F(DBClientTest, FindOne) {
    c.insert(TEST_NS, BSON("name" << "Tyler" << "num" << 1));
    BSONObj doc = c.findOne(TEST_NS , BSONObj());
    ASSERT_EQUALS(doc.getStringField("name"), string("Tyler"));
    ASSERT_NOT_EQUALS(doc.getStringField("name2"), string("Tyler"));
    ASSERT_EQUALS(doc.getIntField( "num" ), 1);
}

TEST_F(DBClientTest, CursorCountEmptyQuery) {
    c.insert(TEST_NS, BSON("name" << "Tyler" << "num" << 1));
    c.insert(TEST_NS, BSON("name" << "Jason" << "num" << 2));
    auto_ptr<DBClientCursor> cursor = c.query(TEST_NS, BSONObj());
    int count = 0;
    while ( cursor->more() ) {
        count++;
        BSONObj obj = cursor->next();
    }
    ASSERT_EQUALS(count, 2);
}

TEST_F(DBClientTest, CursorCountMatching) {
    c.insert(TEST_NS, BSON("name" << "Tyler" << "num" << 1));
    c.insert(TEST_NS, BSON("name" << "Jason" << "num" << 2));
    auto_ptr<DBClientCursor> cursor = c.query(TEST_NS, BSON("num" << 1));
    int count = 0;
    while ( cursor->more() ) {
        count++;
        BSONObj obj = cursor->next();
    }
    ASSERT_EQUALS(count, 1);
}

TEST_F(DBClientTest, CursorCountNotMatching) {
    c.insert(TEST_NS, BSON("name" << "Tyler" << "num" << 1));
    c.insert(TEST_NS, BSON("name" << "Jason" << "num" << 2));
    auto_ptr<DBClientCursor> cursor = c.query(TEST_NS , BSON("num" << 3));
    int count = 0;
    while ( cursor->more() ) {
        count++;
        BSONObj obj = cursor->next();
    }
    ASSERT_EQUALS(count, 0);
}

TEST_F(DBClientTest, Update) {
    c.insert(TEST_NS, BSON("name" << "Tyler" << "num" << 1));

    BSONObj doc = c.findOne(TEST_NS, BSONObjBuilder().append("name", "Tyler").obj());
    ASSERT_NOT_EQUALS(doc.getStringField("name2"), "Tyler");

    BSONObj after = BSONObjBuilder().appendElements(doc).append("name2", "h").obj();
    c.update(TEST_NS, BSONObjBuilder().append("name", "Tyler2").obj(), after);

    doc = c.findOne(TEST_NS, BSONObjBuilder().append("name", "Tyler").obj());
    ASSERT_NOT_EQUALS(doc.getStringField("name2"), "Tyler");
    ASSERT_TRUE(c.findOne(TEST_NS, BSONObjBuilder().append("name", "Tyler2").obj()).isEmpty());

    c.update(TEST_NS, BSONObjBuilder().append("name", "Tyler").obj(), after);
    doc = c.findOne(TEST_NS, BSONObjBuilder().append("name", "Tyler").obj());
    ASSERT_EQUALS(doc.getStringField("name"), string("Tyler"));
    ASSERT_EQUALS(doc.getStringField("name2"), string("h"));
    ASSERT_TRUE(c.findOne(TEST_NS, BSONObjBuilder().append("name", "Tyler2").obj()).isEmpty());
}

TEST_F(DBClientTest, Upsert) {
    BSONObj doc = c.findOne(TEST_NS, BSONObjBuilder().append("name", "Tyler").obj());
    BSONObj after = BSONObjBuilder().appendElements(doc).append("name2", "h").obj();
    c.update(TEST_NS, BSONObjBuilder().append("name", "Tyler2").obj(), after, 1);
    ASSERT_TRUE(c.findOne(TEST_NS, BSONObjBuilder().append("name", "Tyler").obj()).isEmpty());
}

TEST_F(DBClientTest, EnsureIndex) {
    ASSERT_TRUE(c.ensureIndex(TEST_NS, BSON("name" << 1)));
    ASSERT_FALSE(c.ensureIndex(TEST_NS, BSON("name" << 1)));
}

TEST_F(DBClientTest, DISABLED_TTLIndex) {
    BSONObjBuilder b;
    b.appendTimeT("ttltime", time(0));
    b.append("name", "foo");
    c.insert(TEST_NS, b.obj());

    c.ensureIndex(TEST_NS, BSON("ttltime" << 1), false, "", true, false, -1, 5);
    ASSERT_FALSE(c.findOne(TEST_NS, BSONObjBuilder().append("name", "foo").obj()).isEmpty());
    // Sleep 66 seconds, 60 seconds for the TTL loop, 5 seconds for the TTL and 1 to ensure
    sleepsecs(66);
    ASSERT_TRUE(c.findOne(TEST_NS, BSONObjBuilder().append("name", "foo").obj()).isEmpty());
}

TEST_F(DBClientTest, Hint) {
    c.insert(TEST_NS, BSON("name" << "Tyler" << "num" << 1));
    c.insert(TEST_NS, BSON("name" << "Jason" << "num" << 2));
    c.ensureIndex(TEST_NS, BSON("name" << 1));

    ASSERT_EQUALS(c.findOne(TEST_NS, "{}")["name"].str(), "Tyler");
    ASSERT_EQUALS(c.findOne(TEST_NS, "{name: 'Jason'}")["name"].str(), "Jason");
    ASSERT_THROWS(
        c.findOne(TEST_NS, Query("{name: 'Jason'}").hint("{foo: 1}"));
    , DBException);
}

TEST_F(DBClientTest, Timestamp) {
    mongo::BSONObjBuilder b;
    b.appendTimestamp("ts");
    c.insert(TEST_NS, b.obj());

    BSONObj out = c.findOne(TEST_NS, mongo::BSONObj());
    Date_t oldTime = out["ts"].timestampTime();
    unsigned int oldInc = out["ts"].timestampInc();

    mongo::BSONObjBuilder b1;
    b1.append(out["_id"]);

    mongo::BSONObjBuilder b2;
    b2.append(out["_id"]);
    b2.appendTimestamp("ts");

    c.update(TEST_NS, b1.obj(), b2.obj());
    BSONObj found = c.findOne(TEST_NS, BSONObj());
    ASSERT_TRUE(
        (oldTime < found["ts"].timestampTime()) ||
        (oldTime == found["ts"].timestampTime() && oldInc < found["ts"].timestampInc())
    );
}

TEST_F(DBClientTest, KillCursors) {
    ASSERT_TRUE(c.getLastError().empty());

    BufBuilder b;
    b.appendNum((int)0); // reserved
    b.appendNum((int)-1); // invalid # of cursors triggers exception
    b.appendNum((int)-1); // bogus cursor id

    Message m;
    m.setData(dbKillCursors, b.buf(), b.len());

    // say() is protected in DBClientConnection, so get superclass
    static_cast<DBConnector*>(&c)->say(m);

    ASSERT_TRUE(c.getLastError().empty());
}

TEST_F(DBClientTest, GetDatabaseNames) {
    list<string> dbs = c.getDatabaseNames();

    for (list<string>::iterator i = dbs.begin(); i != dbs.end(); i++) {
        // TODO: Create a database and check if it's in this list
    }
}

TEST_F(DBClientTest, GetCollectionNames) {
    list<string> cols = c.getCollectionNames("test");

    for (list<string>::iterator i = cols.begin(); i != cols.end(); i++) {
        // TODO: Create a collection and check if it's in this list
    }
}

TEST_F(DBClientTest, MapReduce) {
    c.insert(TEST_NS, BSON("a" << 1));
    c.insert(TEST_NS, BSON("a" << 1));

    const char* map = "function() { emit(this.a, 1); }";
    const char* reduce = "function(key, values) { return Array.sum(values); }";

    const string outcoll = TEST_NS + ".out";

    BSONObj out;
    // TODO: Do something with the output
    out = c.mapreduce(TEST_NS, map, reduce, BSONObj()); // default to inline
    out = c.mapreduce(TEST_NS, map, reduce, BSONObj(), outcoll);
    out = c.mapreduce(TEST_NS, map, reduce, BSONObj(), outcoll.c_str());
    out = c.mapreduce(TEST_NS, map, reduce, BSONObj(), BSON("reduce" << outcoll));
}

TEST_F(DBClientTest, Timeout) {
    DBClientConnection conn(true, 0, 1);
    conn.connect(string("127.0.0.1:") + integrationTestParams.port);

    conn.insert(TEST_NS, BSON("x" << 1));
    conn.getLastError();

    BSONObj res;
    string eval_str = "return db.foo.findOne().x";

    ASSERT_TRUE(c.eval("test", eval_str, res));
    ASSERT_THROWS(
        conn.eval( "test" , "sleep(2000); " + eval_str, res)
    , DBException);
    ASSERT_TRUE(c.eval("test", eval_str, res));
}

/*
 * From whereExample.cpp
 */

TEST_F(DBClientTest, Where) {
    c.insert(TEST_NS, BSON("name" << "Tyler" << "num" << 17));
    c.insert(TEST_NS, BSON("name" << "Jason" << "num" << 24));

    Query q = Query("{}").where("this.name == name", BSON("name" << "Jason"));

    std::auto_ptr<DBClientCursor> cursor;
    cursor = c.query(TEST_NS, q);
    ASSERT_TRUE(cursor.get());

    int num = 0;
    while(cursor->more()) {
        BSONObj obj = cursor->next();
        num++;
    }
    ASSERT_EQUALS(num, 1);
}

