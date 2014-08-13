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

#include "mongo/unittest/unittest.h"

#include "mongo/client/dbclientinterface.h"

namespace mongo {

    using namespace std;

    /* Query Class */
    TEST(QueryTest, Explain) {
        Query q;
        q.explain();
        ASSERT_TRUE(q.isComplex());
        ASSERT_TRUE(q.isExplain());
    }

    TEST(QueryTest, Snapshot) {
        Query q;
        q.snapshot();
        ASSERT_TRUE(q.isComplex());
        ASSERT_TRUE(q.obj.hasField("$snapshot"));
        ASSERT_TRUE(q.obj.getBoolField("$snapshot"));
    }

    TEST(QueryTest, Sort) {
        Query q;
        q.sort(BSON("a" << 1));
        ASSERT_TRUE(q.isComplex());
        BSONObj sort = q.getSort();
        ASSERT_TRUE(sort.hasField("a"));
        ASSERT_EQUALS(sort.getIntField("a"), 1);
    }

    TEST(QueryTest, Hint) {
        Query q;
        q.hint(BSON("a" << 1));
        BSONElement hint = q.getHint();
        ASSERT_TRUE(hint.Obj().hasField("a"));
        ASSERT_EQUALS(hint.Obj().getIntField("a"), 1);
    }

    TEST(QueryTest, MinKey) {
        Query q;
        BSONObj minobj;
        q.minKey(minobj);
        ASSERT_TRUE(q.isComplex());
        ASSERT_TRUE(q.obj.hasField("$min"));
        ASSERT_EQUALS(q.obj["$min"].Obj(), minobj);
    }

    TEST(QueryTest, MaxKey) {
        Query q;
        BSONObj maxobj;
        q.maxKey(maxobj);
        ASSERT_TRUE(q.isComplex());
        ASSERT_TRUE(q.obj.hasField("$max"));
        ASSERT_EQUALS(q.obj["$max"].Obj(), maxobj);
    }

    TEST(QueryTest, ReadPreferencePrimary) {
        Query q("{}");
        q.readPref(mongo::ReadPreference_PrimaryOnly, BSONArray());
        ASSERT_TRUE(q.obj.hasField(Query::ReadPrefField.name()));
        BSONElement read_pref_elem = q.obj[Query::ReadPrefField.name()];
        ASSERT_TRUE(read_pref_elem.isABSONObj());
        BSONObj read_pref_obj = read_pref_elem.Obj();
        ASSERT_EQUALS(read_pref_obj[Query::ReadPrefModeField.name()].String(), "primary");
        ASSERT_FALSE(read_pref_obj.hasField(Query::ReadPrefTagsField.name()));
    }

    TEST(QueryTest, ReadPreferencePrimaryPreferred) {
        Query q("{}");
        q.readPref(mongo::ReadPreference_PrimaryPreferred, BSONArray());
        ASSERT_TRUE(q.obj.hasField(Query::ReadPrefField.name()));
        BSONElement read_pref_elem = q.obj[Query::ReadPrefField.name()];
        ASSERT_TRUE(read_pref_elem.isABSONObj());
        BSONObj read_pref_obj = read_pref_elem.Obj();
        ASSERT_EQUALS(read_pref_obj[Query::ReadPrefModeField.name()].String(), "primaryPreferred");
        ASSERT_FALSE(read_pref_obj.hasField(Query::ReadPrefTagsField.name()));
    }

    TEST(QueryTest, ReadPreferenceSecondary) {
        Query q("{}");
        q.readPref(mongo::ReadPreference_SecondaryOnly, BSONArray());
        ASSERT_TRUE(q.obj.hasField(Query::ReadPrefField.name()));
        BSONElement read_pref_elem = q.obj[Query::ReadPrefField.name()];
        ASSERT_TRUE(read_pref_elem.isABSONObj());
        BSONObj read_pref_obj = read_pref_elem.Obj();
        ASSERT_EQUALS(read_pref_obj[Query::ReadPrefModeField.name()].String(), "secondary");
        ASSERT_FALSE(read_pref_obj.hasField(Query::ReadPrefTagsField.name()));
    }

    TEST(QueryTest, ReadPreferenceSecondaryPreferred) {
        Query q("{}");
        q.readPref(mongo::ReadPreference_SecondaryPreferred, BSONArray());
        ASSERT_TRUE(q.obj.hasField(Query::ReadPrefField.name()));
        BSONElement read_pref_elem = q.obj[Query::ReadPrefField.name()];
        ASSERT_TRUE(read_pref_elem.isABSONObj());
        BSONObj read_pref_obj = read_pref_elem.Obj();
        ASSERT_EQUALS(read_pref_obj[Query::ReadPrefModeField.name()].String(), "secondaryPreferred");
        ASSERT_FALSE(read_pref_obj.hasField(Query::ReadPrefTagsField.name()));
    }

    TEST(QueryTest, ReadPreferenceNearest) {
        Query q("{}");
        q.readPref(mongo::ReadPreference_Nearest, BSONArray());
        ASSERT_TRUE(q.obj.hasField(Query::ReadPrefField.name()));
        BSONElement read_pref_elem = q.obj[Query::ReadPrefField.name()];
        ASSERT_TRUE(read_pref_elem.isABSONObj());
        BSONObj read_pref_obj = read_pref_elem.Obj();
        ASSERT_EQUALS(read_pref_obj[Query::ReadPrefModeField.name()].String(), "nearest");
        ASSERT_FALSE(read_pref_obj.hasField(Query::ReadPrefTagsField.name()));
    }

    TEST(QueryTest, ReadPreferenceTagSets) {
        Query q("{}");
        BSONObj tag_set1 = BSON("datacenter" << "nyc");
        BSONObj tag_set2 = BSON("awesome" << "yeah");
        BSONObjBuilder bob;
        BSONArrayBuilder bab;
        bab.append(tag_set1);
        bab.append(tag_set2);
        q.readPref(mongo::ReadPreference_SecondaryOnly, bab.arr());
        ASSERT_TRUE(q.obj.hasField(Query::ReadPrefField.name()));

        BSONElement read_pref_elem = q.obj[Query::ReadPrefField.name()];
        ASSERT_TRUE(read_pref_elem.isABSONObj());
        BSONObj read_pref_obj = read_pref_elem.Obj();
        ASSERT_EQUALS(read_pref_obj[Query::ReadPrefModeField.name()].String(), "secondary");
        ASSERT_TRUE(read_pref_obj.hasField(Query::ReadPrefTagsField.name()));
        vector<BSONElement> tag_sets = read_pref_obj[Query::ReadPrefTagsField.name()].Array();
        ASSERT_EQUALS(tag_sets[0].Obj(), tag_set1);
        ASSERT_EQUALS(tag_sets[1].Obj(), tag_set2);
    }

} // namespace mongo
