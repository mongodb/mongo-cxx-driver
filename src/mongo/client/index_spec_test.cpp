/*    Copyright 2014 10gen Inc.
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

#include "mongo/client/index_spec.h"

#include "mongo/unittest/unittest.h"

#define ASSERT_UASSERTS(STATEMENT) ASSERT_THROWS(STATEMENT, UserException)

namespace mongo {

    TEST(Options, RepeatedOptionsFail) {
        ASSERT_UASSERTS(IndexSpec().background().background());
        ASSERT_UASSERTS(IndexSpec().unique().unique());
        ASSERT_UASSERTS(IndexSpec().dropDuplicatesDeprecated().dropDuplicatesDeprecated());
        ASSERT_UASSERTS(IndexSpec().sparse().sparse());
        ASSERT_UASSERTS(IndexSpec().expireAfterSeconds(1).expireAfterSeconds(1));
        ASSERT_UASSERTS(IndexSpec().version(0).version(0));
        ASSERT_UASSERTS(IndexSpec().textWeights(BSONObj()).textWeights(BSONObj()));
        ASSERT_UASSERTS(IndexSpec().textDefaultLanguage("foo").textDefaultLanguage("foo"));
        ASSERT_UASSERTS(IndexSpec().textLanguageOverride("foo").textLanguageOverride("foo"));
        ASSERT_UASSERTS(IndexSpec().textIndexVersion(0).textIndexVersion(0));
        ASSERT_UASSERTS(IndexSpec().geo2DSphereIndexVersion(0).geo2DSphereIndexVersion(0));
        ASSERT_UASSERTS(IndexSpec().geo2DBits(0).geo2DBits(0));
        ASSERT_UASSERTS(IndexSpec().geo2DMin(2.00).geo2DMin(2.00));
        ASSERT_UASSERTS(IndexSpec().geo2DMax(2.00).geo2DMax(2.00));
        ASSERT_UASSERTS(IndexSpec().geoHaystackBucketSize(2.0).geoHaystackBucketSize(2.0));
        ASSERT_UASSERTS(IndexSpec().addOptions(BSON("foo" << 1 << "foo" << 1)));
        ASSERT_UASSERTS(IndexSpec().sparse(0).addOptions(BSON("sparse" << 1)));
    }

    TEST(Options, RepeatedKeysFail) {

        IndexSpec spec;
        spec.addKey("aField");

        ASSERT_UASSERTS(spec.addKey("aField"));

        const BSONObj fields = BSON("someField" << 1 << "aField" << 1 << "anotherField" << 1);
        ASSERT_UASSERTS(spec.addKey(fields.getField("aField")));
        ASSERT_UASSERTS(spec.addKeys(fields));
    }

    TEST(Options, NameIsHonored) {
        IndexSpec spec;
        spec.addKey("aField");

        // Should get an auto generated name
        ASSERT_FALSE(spec.name().empty());

        // That is not the name we are about to set.
        ASSERT_NE("someName", spec.name());

        spec.name("someName");

        // Should get the name we specified.
        ASSERT_EQ("someName", spec.name());

        // Name can be changed as many times as we want
        spec.name("yetAnotherName");
        ASSERT_EQ("yetAnotherName", spec.name());
    }

} // namespace mongo
