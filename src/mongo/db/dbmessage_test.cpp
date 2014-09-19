/**
 *    Copyright (C) 2014 MongoDB Inc.
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

#include <string>

#include "mongo/bson/util/builder.h"
#include "mongo/db/dbmessage.h"
#include "mongo/unittest/unittest.h"

namespace mongo {
    using std::string;

    // Test if the reserved field is short of 4 bytes
    TEST(DBMessage1, ShortFlags) {
        BufBuilder b;
        string ns("test");

        b.appendChar( 1 );

        Message toSend;
        toSend.setData( dbDelete , b.buf() , b.len() );

        ASSERT_THROWS(DbMessage d1(toSend), UserException);
    }

    // Test a short NS missing a trailing null
    TEST(DBMessage1, BadNS) {
        BufBuilder b;

        b.appendNum( static_cast<int>(1) );
        b.appendChar( 'b' );
        b.appendChar( 'a' );
        b.appendChar( 'd' );
        // Forget to append \0

        Message toSend;
        toSend.setData( dbDelete , b.buf() , b.len() );

        ASSERT_THROWS(DbMessage d1(toSend), UserException);
    }

    // Test a valid kill message and try an extra pull
    TEST(DBMessage1, GoodKill) {
        BufBuilder b;

        b.appendNum( static_cast<int>(1) );
        b.appendNum( static_cast<int>(3) );

        Message toSend;
        toSend.setData( dbKillCursors , b.buf() , b.len() );

        DbMessage d1(toSend);
        ASSERT_EQUALS(3, d1.pullInt());

        ASSERT_THROWS(d1.pullInt(), UserException);
    }

    // Try a bad read of a type too large
    TEST(DBMessage1, GoodKill2) {
        BufBuilder b;

        b.appendNum( static_cast<int>(1) );
        b.appendNum( static_cast<int>(3) );

        Message toSend;
        toSend.setData( dbKillCursors , b.buf() , b.len() );

        DbMessage d1(toSend);
        ASSERT_THROWS(d1.pullInt64(), UserException);
    }

    // Test a basic good insert, and an extra read
    TEST(DBMessage1, GoodInsert) {
        BufBuilder b;
        string ns("test");

        b.appendNum( static_cast<int>(1) );
        b.appendStr(ns);
        b.appendNum( static_cast<int>(3) );
        b.appendNum( static_cast<int>(39) );

        Message toSend;
        toSend.setData( dbInsert , b.buf() , b.len() );

        DbMessage d1(toSend);
        ASSERT_EQUALS(3, d1.pullInt());
        ASSERT_EQUALS(39, d1.pullInt());
        ASSERT_THROWS(d1.pullInt(), UserException);
    }

    // Test a basic good insert, and an extra read
    TEST(DBMessage1, GoodInsert2) {
        BufBuilder b;
        string ns("test");

        b.appendNum( static_cast<int>(1) );
        b.appendStr(ns);
        b.appendNum( static_cast<int>(3) );
        b.appendNum( static_cast<int>(39) );

        BSONObj bo = BSON( "ts" << 0 );
        bo.appendSelfToBufBuilder( b );

        Message toSend;
        toSend.setData( dbInsert , b.buf() , b.len() );

        DbMessage d1(toSend);
        ASSERT_EQUALS(3, d1.pullInt());


        ASSERT_EQUALS(39, d1.pullInt());
        BSONObj bo2 = d1.nextJsObj();
        ASSERT_THROWS(d1.nextJsObj(), MsgAssertionException);
    }



} // mongo namespace
