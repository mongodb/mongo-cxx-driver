// jsontests.cpp - Tests for json.{h,cpp} code and BSONObj::jsonString()
//

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

#define MONGO_LOG_DEFAULT_COMPONENT ::mongo::logger::LogComponent::kDefault

#include <limits>

#include "mongo/db/jsobj.h"
#include "mongo/db/json.h"
#include "mongo/unittest/unittest.h"
#include "mongo/util/log.h"

namespace JsonTests {

using namespace std;

namespace JsonStringTests {

class Empty {
public:
    void run() {
        ASSERT_EQUALS("{}", BSONObj().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(Empty);

class SingleStringMember {
public:
    void run() {
        ASSERT_EQUALS("{ \"a\" : \"b\" }",
                      BSON("a"
                           << "b").jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(SingleStringMember);

class EscapedCharacters {
public:
    void run() {
        BSONObjBuilder b;
        b.append("a", "\" \\ / \b \f \n \r \t");
        ASSERT_EQUALS("{ \"a\" : \"\\\" \\\\ / \\b \\f \\n \\r \\t\" }",
                      b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(EscapedCharacters);

// per http://www.ietf.org/rfc/rfc4627.txt, control characters are
// (U+0000 through U+001F).  U+007F is not mentioned as a control character.
class AdditionalControlCharacters {
public:
    void run() {
        BSONObjBuilder b;
        b.append("a", "\x1 \x1f");
        ASSERT_EQUALS("{ \"a\" : \"\\u0001 \\u001f\" }", b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(AdditionalControlCharacters);

class ExtendedAscii {
public:
    void run() {
        BSONObjBuilder b;
        b.append("a", "\x80");
        ASSERT_EQUALS("{ \"a\" : \"\x80\" }", b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(ExtendedAscii);

class EscapeFieldName {
public:
    void run() {
        BSONObjBuilder b;
        b.append("\t", "b");
        ASSERT_EQUALS("{ \"\\t\" : \"b\" }", b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(EscapeFieldName);

class SingleIntMember {
public:
    void run() {
        BSONObjBuilder b;
        b.append("a", 1);
        ASSERT_EQUALS("{ \"a\" : 1 }", b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(SingleIntMember);

class SingleNumberMember {
public:
    void run() {
        BSONObjBuilder b;
        b.append("a", 1.5);
        ASSERT_EQUALS("{ \"a\" : 1.5 }", b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(SingleNumberMember);

class InvalidNumbers {
public:
    void run() {
        BSONObjBuilder c;
        c.append("a", numeric_limits<double>::quiet_NaN());
        string s = c.done().jsonString(Strict);
        // Note there is no NaN in the JSON RFC but what would be the alternative?
        ASSERT(str::contains(s, "NaN"));

        // commented out assertion as it doesn't throw anymore:
        // ASSERT_THROWS( c.done().jsonString( Strict ), AssertionException );

        BSONObjBuilder d;
        d.append("a", numeric_limits<double>::signaling_NaN());
        // ASSERT_THROWS( d.done().jsonString( Strict ), AssertionException );
        s = d.done().jsonString(Strict);
        ASSERT(str::contains(s, "NaN"));
    }
};
DBTEST_SHIM_TEST(InvalidNumbers);

class NumberPrecision {
public:
    void run() {
        BSONObjBuilder b;
        b.append("a", 123456789);
        ASSERT_EQUALS("{ \"a\" : 123456789 }", b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(NumberPrecision);

class NegativeNumber {
public:
    void run() {
        BSONObjBuilder b;
        b.append("a", -1);
        ASSERT_EQUALS("{ \"a\" : -1 }", b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(NegativeNumber);

class NumberLongStrict {
public:
    void run() {
        BSONObjBuilder b;
        b.append("a", 20000LL);
        ASSERT_EQUALS("{ \"a\" : { \"$numberLong\" : \"20000\" } }", b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(NumberLongStrict);

// Test a NumberLong that is too big to fit into a 32 bit integer
class NumberLongStrictLarge {
public:
    void run() {
        BSONObjBuilder b;
        b.append("a", 9223372036854775807LL);
        ASSERT_EQUALS("{ \"a\" : { \"$numberLong\" : \"9223372036854775807\" } }",
                      b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(NumberLongStrictLarge);

class NumberLongStrictNegative {
public:
    void run() {
        BSONObjBuilder b;
        b.append("a", -20000LL);
        ASSERT_EQUALS("{ \"a\" : { \"$numberLong\" : \"-20000\" } }", b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(NumberLongStrictNegative);

class NumberDoubleNaN {
public:
    void run() {
        BSONObjBuilder b;
        b.append("a", std::numeric_limits<double>::quiet_NaN());
        ASSERT_EQUALS("{ \"a\" : NaN }", b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(NumberDoubleNaN);

class NumberDoubleInfinity {
public:
    void run() {
        BSONObjBuilder b;
        b.append("a", std::numeric_limits<double>::infinity());
        ASSERT_EQUALS("{ \"a\" : Infinity }", b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(NumberDoubleInfinity);

class NumberDoubleNegativeInfinity {
public:
    void run() {
        BSONObjBuilder b;
        b.append("a", -std::numeric_limits<double>::infinity());
        ASSERT_EQUALS("{ \"a\" : -Infinity }", b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(NumberDoubleNegativeInfinity);

class SingleBoolMember {
public:
    void run() {
        BSONObjBuilder b;
        b.appendBool("a", true);
        ASSERT_EQUALS("{ \"a\" : true }", b.done().jsonString(Strict));

        BSONObjBuilder c;
        c.appendBool("a", false);
        ASSERT_EQUALS("{ \"a\" : false }", c.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(SingleBoolMember);

class SingleNullMember {
public:
    void run() {
        BSONObjBuilder b;
        b.appendNull("a");
        ASSERT_EQUALS("{ \"a\" : null }", b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(SingleNullMember);

class SingleUndefinedMember {
public:
    void run() {
        BSONObjBuilder b;
        b.appendUndefined("a");
        ASSERT_EQUALS("{ \"a\" : { \"$undefined\" : true } }", b.done().jsonString(Strict));
        ASSERT_EQUALS("{ \"a\" : undefined }", b.done().jsonString(JS));
        ASSERT_EQUALS("{ \"a\" : undefined }", b.done().jsonString(TenGen));
    }
};
DBTEST_SHIM_TEST(SingleUndefinedMember);

class SingleObjectMember {
public:
    void run() {
        BSONObjBuilder b, c;
        b.append("a", c.done());
        ASSERT_EQUALS("{ \"a\" : {} }", b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(SingleObjectMember);

class TwoMembers {
public:
    void run() {
        BSONObjBuilder b;
        b.append("a", 1);
        b.append("b", 2);
        ASSERT_EQUALS("{ \"a\" : 1, \"b\" : 2 }", b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(TwoMembers);

class EmptyArray {
public:
    void run() {
        vector<int> arr;
        BSONObjBuilder b;
        b.append("a", arr);
        ASSERT_EQUALS("{ \"a\" : [] }", b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(EmptyArray);

class Array {
public:
    void run() {
        vector<int> arr;
        arr.push_back(1);
        arr.push_back(2);
        BSONObjBuilder b;
        b.append("a", arr);
        ASSERT_EQUALS("{ \"a\" : [ 1, 2 ] }", b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(Array);

class DBRef {
public:
    void run() {
        char OIDbytes[OID::kOIDSize];
        memset(&OIDbytes, 0xff, OID::kOIDSize);
        OID oid = OID::from(OIDbytes);
        BSONObjBuilder b;
        b.appendDBRef("a", "namespace", oid);
        BSONObj built = b.done();
        ASSERT_EQUALS(
            "{ \"a\" : { \"$ref\" : \"namespace\", \"$id\" : \"ffffffffffffffffffffffff\" } }",
            built.jsonString(Strict));
        ASSERT_EQUALS(
            "{ \"a\" : { \"$ref\" : \"namespace\", \"$id\" : \"ffffffffffffffffffffffff\" } }",
            built.jsonString(JS));
        ASSERT_EQUALS("{ \"a\" : Dbref( \"namespace\", \"ffffffffffffffffffffffff\" ) }",
                      built.jsonString(TenGen));
    }
};
DBTEST_SHIM_TEST(DBRef);

class DBRefZero {
public:
    void run() {
        char OIDbytes[OID::kOIDSize];
        memset(&OIDbytes, 0, OID::kOIDSize);
        OID oid = OID::from(OIDbytes);
        BSONObjBuilder b;
        b.appendDBRef("a", "namespace", oid);
        ASSERT_EQUALS(
            "{ \"a\" : { \"$ref\" : \"namespace\", \"$id\" : \"000000000000000000000000\" } }",
            b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(DBRefZero);

class ObjectId {
public:
    void run() {
        char OIDbytes[OID::kOIDSize];
        memset(&OIDbytes, 0xff, OID::kOIDSize);
        OID oid = OID::from(OIDbytes);
        BSONObjBuilder b;
        b.appendOID("a", &oid);
        BSONObj built = b.done();
        ASSERT_EQUALS("{ \"a\" : { \"$oid\" : \"ffffffffffffffffffffffff\" } }",
                      built.jsonString(Strict));
        ASSERT_EQUALS("{ \"a\" : ObjectId( \"ffffffffffffffffffffffff\" ) }",
                      built.jsonString(TenGen));
    }
};
DBTEST_SHIM_TEST(ObjectId);

class BinData {
public:
    void run() {
        char z[3];
        z[0] = 'a';
        z[1] = 'b';
        z[2] = 'c';
        BSONObjBuilder b;
        b.appendBinData("a", 3, BinDataGeneral, z);

        string o = b.done().jsonString(Strict);

        ASSERT_EQUALS("{ \"a\" : { \"$binary\" : \"YWJj\", \"$type\" : \"00\" } }", o);

        BSONObjBuilder c;
        c.appendBinData("a", 2, BinDataGeneral, z);
        ASSERT_EQUALS("{ \"a\" : { \"$binary\" : \"YWI=\", \"$type\" : \"00\" } }",
                      c.done().jsonString(Strict));

        BSONObjBuilder d;
        d.appendBinData("a", 1, BinDataGeneral, z);
        ASSERT_EQUALS("{ \"a\" : { \"$binary\" : \"YQ==\", \"$type\" : \"00\" } }",
                      d.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(BinData);

class Symbol {
public:
    void run() {
        BSONObjBuilder b;
        b.appendSymbol("a", "b");
        ASSERT_EQUALS("{ \"a\" : \"b\" }", b.done().jsonString(Strict));
    }
};
DBTEST_SHIM_TEST(Symbol);

#ifdef _WIN32
char tzEnvString[] = "TZ=EST+5EDT";
#else
char tzEnvString[] = "TZ=America/New_York";
#endif

class Date {
public:
    Date() {
        char* _oldTimezonePtr = getenv("TZ");
        _oldTimezone = std::string(_oldTimezonePtr ? _oldTimezonePtr : "");
        if (-1 == putenv(tzEnvString)) {
            // FAIL() << errnoWithDescription();
        }
        tzset();
    }
    ~Date() {
        if (!_oldTimezone.empty()) {
#ifdef _WIN32
            errno_t ret = _putenv_s("TZ", _oldTimezone.c_str());
            if (0 != ret) {
                StringBuilder sb;
                sb << "Error setting TZ environment variable to:  " << _oldTimezone
                   << ".  Error code:  " << ret;
                // FAIL() << sb.str();
            }
#else
            if (-1 == setenv("TZ", _oldTimezone.c_str(), 1)) {
                // FAIL() << errnoWithDescription();
            }
#endif
        } else {
#ifdef _WIN32
            errno_t ret = _putenv_s("TZ", "");
            if (0 != ret) {
                StringBuilder sb;
                sb << "Error unsetting TZ environment variable.  Error code:  " << ret;
                // FAIL() << sb.str();
            }
#else
            if (-1 == unsetenv("TZ")) {
                // FAIL() << errnoWithDescription();
            }
#endif
        }
        tzset();
    }

    void run() {
        BSONObjBuilder b;
        b.appendDate("a", 0);
        BSONObj built = b.done();
        ASSERT_EQUALS("{ \"a\" : { \"$date\" : \"1969-12-31T19:00:00.000-0500\" } }",
                      built.jsonString(Strict));
        ASSERT_EQUALS("{ \"a\" : Date( 0 ) }", built.jsonString(TenGen));
        ASSERT_EQUALS("{ \"a\" : Date( 0 ) }", built.jsonString(JS));

        // Test dates above our maximum formattable date.  See SERVER-13760.
        BSONObjBuilder b2;
        b2.appendDate("a", 32535262800000ULL);
        BSONObj built2 = b2.done();
        ASSERT_EQUALS("{ \"a\" : { \"$date\" : { \"$numberLong\" : \"32535262800000\" } } }",
                      built2.jsonString(Strict));
    }

private:
    std::string _oldTimezone;
};
DBTEST_SHIM_TEST(Date);

class DateNegative {
public:
    void run() {
        BSONObjBuilder b;
        b.appendDate("a", -1);
        BSONObj built = b.done();
        ASSERT_EQUALS("{ \"a\" : { \"$date\" : { \"$numberLong\" : \"-1\" } } }",
                      built.jsonString(Strict));
        ASSERT_EQUALS("{ \"a\" : Date( -1 ) }", built.jsonString(TenGen));
        ASSERT_EQUALS("{ \"a\" : Date( -1 ) }", built.jsonString(JS));
    }
};
DBTEST_SHIM_TEST(DateNegative);

class Regex {
public:
    void run() {
        BSONObjBuilder b;
        b.appendRegex("a", "abc", "i");
        BSONObj built = b.done();
        ASSERT_EQUALS("{ \"a\" : { \"$regex\" : \"abc\", \"$options\" : \"i\" } }",
                      built.jsonString(Strict));
        ASSERT_EQUALS("{ \"a\" : /abc/i }", built.jsonString(TenGen));
        ASSERT_EQUALS("{ \"a\" : /abc/i }", built.jsonString(JS));
    }
};
DBTEST_SHIM_TEST(Regex);

class RegexEscape {
public:
    void run() {
        BSONObjBuilder b;
        b.appendRegex("a", "/\"", "i");
        BSONObj built = b.done();
        ASSERT_EQUALS("{ \"a\" : { \"$regex\" : \"/\\\"\", \"$options\" : \"i\" } }",
                      built.jsonString(Strict));
        ASSERT_EQUALS("{ \"a\" : /\\/\\\"/i }", built.jsonString(TenGen));
        ASSERT_EQUALS("{ \"a\" : /\\/\\\"/i }", built.jsonString(JS));
    }
};
DBTEST_SHIM_TEST(RegexEscape);

class RegexManyOptions {
public:
    void run() {
        BSONObjBuilder b;
        b.appendRegex("a", "z", "abcgimx");
        BSONObj built = b.done();
        ASSERT_EQUALS("{ \"a\" : { \"$regex\" : \"z\", \"$options\" : \"abcgimx\" } }",
                      built.jsonString(Strict));
        ASSERT_EQUALS("{ \"a\" : /z/gim }", built.jsonString(TenGen));
        ASSERT_EQUALS("{ \"a\" : /z/gim }", built.jsonString(JS));
    }
};
DBTEST_SHIM_TEST(RegexManyOptions);

class CodeTests {
public:
    void run() {
        BSONObjBuilder b;
        b.appendCode("x", "function(arg){ var string = \"\\n\"; return 1; }");
        BSONObj o = b.obj();
        ASSERT_EQUALS(
            "{ \"x\" : \"function(arg){ var string = \\\"\\\\n\\\"; "
            "return 1; }\" }",
            o.jsonString());
    }
};
DBTEST_SHIM_TEST(CodeTests);

class CodeWScopeTests {
public:
    void run() {
        BSONObjBuilder b;
        b.appendCodeWScope("x", "function(arg){ var string = \"\\n\"; return x; }", BSON("x" << 1));
        BSONObj o = b.obj();
        ASSERT_EQUALS(
            "{ \"x\" : "
            "{ \"$code\" : "
            "\"function(arg){ var string = \\\"\\\\n\\\"; return x; }\" , "
            "\"$scope\" : { \"x\" : 1 } } }",
            o.jsonString());
    }
};
DBTEST_SHIM_TEST(CodeWScopeTests);

class TimestampTests {
public:
    void run() {
        BSONObjBuilder b;
        b.appendTimestamp("x", Timestamp_t(4, 10));
        BSONObj o = b.obj();
        ASSERT_EQUALS("{ \"x\" : { \"$timestamp\" : { \"t\" : 4, \"i\" : 10 } } }",
                      o.jsonString(Strict));
        ASSERT_EQUALS("{ \"x\" : { \"$timestamp\" : { \"t\" : 4, \"i\" : 10 } } }",
                      o.jsonString(JS));
        ASSERT_EQUALS("{ \"x\" : Timestamp( 4, 10 ) }", o.jsonString(TenGen));
    }
};
DBTEST_SHIM_TEST(TimestampTests);

class NullString {
public:
    void run() {
        BSONObjBuilder b;
        b.append("x", "a\0b", 4);
        BSONObj o = b.obj();
        ASSERT_EQUALS("{ \"x\" : \"a\\u0000b\" }", o.jsonString());
    }
};
DBTEST_SHIM_TEST(NullString);

class AllTypes {
public:
    void run() {
        OID oid;
        oid.init();

        BSONObjBuilder b;
        b.appendMinKey("a");
        b.append("b", 5.5);
        b.append("c", "abc");
        b.append("e", BSON("x" << 1));
        b.append("f", BSON_ARRAY(1 << 2 << 3));
        b.appendBinData("g", sizeof(AllTypes), bdtCustom, (const void*)this);
        b.appendUndefined("h");
        b.append("i", oid);
        b.appendBool("j", 1);
        b.appendDate("k", 123);
        b.appendNull("l");
        b.appendRegex("m", "a");
        b.appendDBRef("n", "foo", oid);
        b.appendCode("o", "function(){}");
        b.appendSymbol("p", "foo");
        b.appendCodeWScope("q", "function(){}", BSON("x" << 1));
        b.append("r", (int)5);
        b.appendTimestamp("s", Timestamp_t(123123, 123123));
        b.append("t", 12321312312LL);
        b.appendMaxKey("u");

        BSONObj o = b.obj();
        o.jsonString();
        // cout << o.jsonString() << endl;
    }
};
DBTEST_SHIM_TEST(AllTypes);

}  // namespace JsonStringTests

namespace FromJsonTests {

class Base {
public:
    virtual ~Base() {}
    void run() {
        ASSERT(fromjson(json()).valid());
        assertEquals(bson(), fromjson(tojson(bson())), "mode: <default>");
        assertEquals(bson(), fromjson(tojson(bson(), Strict)), "mode: strict");
        assertEquals(bson(), fromjson(tojson(bson(), TenGen)), "mode: tengen");
        assertEquals(bson(), fromjson(tojson(bson(), JS)), "mode: js");
    }

protected:
    virtual BSONObj bson() const = 0;
    virtual string json() const = 0;

private:
    void assertEquals(const BSONObj& expected, const BSONObj& actual, const char* msg) {
        const bool bad = expected.woCompare(actual);
        if (bad) {
            ::mongo::log() << "want:" << expected.jsonString() << " size: " << expected.objsize()
                           << endl;
            ::mongo::log() << "got :" << actual.jsonString() << " size: " << actual.objsize()
                           << endl;
            ::mongo::log() << expected.hexDump() << endl;
            ::mongo::log() << actual.hexDump() << endl;
            ::mongo::log() << msg << endl;
            ::mongo::log() << "orig json:" << this->json();
        }
        ASSERT(!bad);
    }
};

class Bad {
public:
    virtual ~Bad() {}
    void run() {
        ASSERT_THROWS(fromjson(json()), MsgAssertionException);
    }

protected:
    virtual string json() const = 0;
};

class Empty : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        return b.obj();
    }
    virtual string json() const {
        return "{}";
    }
};
DBTEST_SHIM_TEST(Empty);

class EmptyWithSpace : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        return b.obj();
    }
    virtual string json() const {
        return "{ }";
    }
};
DBTEST_SHIM_TEST(EmptyWithSpace);

class SingleString : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("a", "b");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : \"b\" }";
    }
};
DBTEST_SHIM_TEST(SingleString);

class EmptyStrings : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("", "");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"\" : \"\" }";
    }
};
DBTEST_SHIM_TEST(EmptyStrings);

class ReservedFieldName : public Bad {
    virtual string json() const {
        return "{ \"$oid\" : \"b\" }";
    }
};
DBTEST_SHIM_TEST(ReservedFieldName);

class ReservedFieldName1 : public Bad {
    virtual string json() const {
        return "{ \"$ref\" : \"b\" }";
    }
};
DBTEST_SHIM_TEST(ReservedFieldName1);

class NumberFieldName : public Bad {
    virtual string json() const {
        return "{ 0 : \"b\" }";
    }
};
DBTEST_SHIM_TEST(NumberFieldName);

class InvalidFieldName : public Bad {
    virtual string json() const {
        return "{ test.test : \"b\" }";
    }
};
DBTEST_SHIM_TEST(InvalidFieldName);

class QuotedNullName : public Bad {
    virtual string json() const {
        return "{ \"nc\0nc\" : \"b\" }";
    }
};
DBTEST_SHIM_TEST(QuotedNullName);

class NoValue : public Bad {
    virtual string json() const {
        return "{ a : }";
    }
};
DBTEST_SHIM_TEST(NoValue);

class InvalidValue : public Bad {
    virtual string json() const {
        return "{ a : a }";
    }
};
DBTEST_SHIM_TEST(InvalidValue);

class OkDollarFieldName : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("$where", 1);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"$where\" : 1 }";
    }
};
DBTEST_SHIM_TEST(OkDollarFieldName);

class SingleNumber : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("a", 1);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : 1 }";
    }
};
DBTEST_SHIM_TEST(SingleNumber);

class RealNumber : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("a", strtod("0.7", 0));
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : 0.7 }";
    }
};
DBTEST_SHIM_TEST(RealNumber);

class FancyNumber : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("a", strtod("-4.4433e-2", 0));
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : -4.4433e-2 }";
    }
};
DBTEST_SHIM_TEST(FancyNumber);

class TwoElements : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("a", 1);
        b.append("b", "foo");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : 1, \"b\" : \"foo\" }";
    }
};
DBTEST_SHIM_TEST(TwoElements);

class Subobject : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("a", 1);
        BSONObjBuilder c;
        c.append("z", b.done());
        return c.obj();
    }
    virtual string json() const {
        return "{ \"z\" : { \"a\" : 1 } }";
    }
};
DBTEST_SHIM_TEST(Subobject);

class DeeplyNestedObject : public Base {
    virtual string buildJson(int depth) const {
        if (depth == 0) {
            return "{\"0\":true}";
        } else {
            std::stringstream ss;
            ss << "{\"" << depth << "\":" << buildJson(depth - 1) << "}";
            depth--;
            return ss.str();
        }
    }
    virtual BSONObj buildBson(int depth) const {
        BSONObjBuilder builder;
        if (depth == 0) {
            builder.append("0", true);
            return builder.obj();
        } else {
            std::stringstream ss;
            ss << depth;
            depth--;
            builder.append(ss.str(), buildBson(depth));
            return builder.obj();
        }
    }
    virtual BSONObj bson() const {
        return buildBson(35);
    }
    virtual string json() const {
        return buildJson(35);
    }
};
DBTEST_SHIM_TEST(DeeplyNestedObject);

class ArrayEmpty : public Base {
    virtual BSONObj bson() const {
        vector<int> arr;
        BSONObjBuilder b;
        b.append("a", arr);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : [] }";
    }
};
DBTEST_SHIM_TEST(ArrayEmpty);

class TopLevelArrayEmpty : public Base {
    virtual BSONObj bson() const {
        return BSONArray();
    }
    virtual string json() const {
        return "[]";
    }
};
DBTEST_SHIM_TEST(TopLevelArrayEmpty);

class TopLevelArray : public Base {
    virtual BSONObj bson() const {
        BSONArrayBuilder builder;
        builder.append(123);
        builder.append("abc");
        return builder.arr();
    }
    virtual string json() const {
        return "[ 123, \"abc\" ]";
    }
};
DBTEST_SHIM_TEST(TopLevelArray);

class Array : public Base {
    virtual BSONObj bson() const {
        vector<int> arr;
        arr.push_back(1);
        arr.push_back(2);
        arr.push_back(3);
        BSONObjBuilder b;
        b.append("a", arr);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : [ 1, 2, 3 ] }";
    }
};
DBTEST_SHIM_TEST(Array);

class True : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendBool("a", true);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : true }";
    }
};
DBTEST_SHIM_TEST(True);

class False : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendBool("a", false);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : false }";
    }
};
DBTEST_SHIM_TEST(False);

class Null : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendNull("a");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : null }";
    }
};
DBTEST_SHIM_TEST(Null);

class Undefined : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendUndefined("a");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : undefined }";
    }
};
DBTEST_SHIM_TEST(Undefined);

class UndefinedStrict : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendUndefined("a");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$undefined\" : true } }";
    }
};
DBTEST_SHIM_TEST(UndefinedStrict);

class UndefinedStrictBad : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$undefined\" : false } }";
    }
};
DBTEST_SHIM_TEST(UndefinedStrictBad);

class EscapedCharacters : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("a", "\" \\ / \b \f \n \r \t \v");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : \"\\\" \\\\ \\/ \\b \\f \\n \\r \\t \\v\" }";
    }
};
DBTEST_SHIM_TEST(EscapedCharacters);

class NonEscapedCharacters : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("a", "% { a z $ # '  ");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : \"\\% \\{ \\a \\z \\$ \\# \\' \\ \" }";
    }
};
DBTEST_SHIM_TEST(NonEscapedCharacters);

class AllowedControlCharacter : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("a", "\x7f");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : \"\x7f\" }";
    }
};
DBTEST_SHIM_TEST(AllowedControlCharacter);

class InvalidControlCharacter : public Bad {
    virtual string json() const {
        return "{ \"a\" : \"\x1f\" }";
    }
};
DBTEST_SHIM_TEST(InvalidControlCharacter);

class NumbersInFieldName : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("b1", "b");
        return b.obj();
    }
    virtual string json() const {
        return "{ b1 : \"b\" }";
    }
};
DBTEST_SHIM_TEST(NumbersInFieldName);

class EscapeFieldName : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("\n", "b");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"\\n\" : \"b\" }";
    }
};
DBTEST_SHIM_TEST(EscapeFieldName);

class EscapedUnicodeToUtf8 : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        unsigned char u[7];
        u[0] = 0xe0 | 0x0a;
        u[1] = 0x80;
        u[2] = 0x80;
        u[3] = 0xe0 | 0x0a;
        u[4] = 0x80;
        u[5] = 0x80;
        u[6] = 0;
        b.append("a", (char*)u);
        BSONObj built = b.obj();
        // ASSERT_EQUALS( string( (char *) u ), built.firstElement().valuestr() );
        return built;
    }
    virtual string json() const {
        return "{ \"a\" : \"\\ua000\\uA000\" }";
    }
};
DBTEST_SHIM_TEST(EscapedUnicodeToUtf8);

class Utf8AllOnes : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        unsigned char u[8];
        u[0] = 0x01;

        u[1] = 0x7f;

        u[2] = 0xdf;
        u[3] = 0xbf;

        u[4] = 0xef;
        u[5] = 0xbf;
        u[6] = 0xbf;

        u[7] = 0;

        b.append("a", (char*)u);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : \"\\u0001\\u007f\\u07ff\\uffff\" }";
    }
};
DBTEST_SHIM_TEST(Utf8AllOnes);

class Utf8FirstByteOnes : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        unsigned char u[6];
        u[0] = 0xdc;
        u[1] = 0x80;

        u[2] = 0xef;
        u[3] = 0xbc;
        u[4] = 0x80;

        u[5] = 0;

        b.append("a", (char*)u);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : \"\\u0700\\uff00\" }";
    }
};
DBTEST_SHIM_TEST(Utf8FirstByteOnes);

class Utf8Invalid : public Bad {
    virtual string json() const {
        return "{ \"a\" : \"\\u0ZZZ\" }";
    }
};
DBTEST_SHIM_TEST(Utf8Invalid);

class Utf8TooShort : public Bad {
    virtual string json() const {
        return "{ \"a\" : \"\\u000\" }";
    }
};
DBTEST_SHIM_TEST(Utf8TooShort);

class DBRefConstructor : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        BSONObjBuilder subBuilder(b.subobjStart("a"));
        subBuilder.append("$ref", "ns");
        subBuilder.append("$id", "000000000000000000000000");
        subBuilder.done();
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : Dbref( \"ns\", \"000000000000000000000000\" ) }";
    }
};
DBTEST_SHIM_TEST(DBRefConstructor);

// Added for consistency with the mongo shell
class DBRefConstructorCapitals : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        BSONObjBuilder subBuilder(b.subobjStart("a"));
        subBuilder.append("$ref", "ns");
        subBuilder.append("$id", "000000000000000000000000");
        subBuilder.done();
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : DBRef( \"ns\", \"000000000000000000000000\" ) }";
    }
};
DBTEST_SHIM_TEST(DBRefConstructorCapitals);

class DBRefConstructorDbName : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        BSONObjBuilder subBuilder(b.subobjStart("a"));
        subBuilder.append("$ref", "ns");
        subBuilder.append("$id", "000000000000000000000000");
        subBuilder.append("$db", "dbname");
        subBuilder.done();
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : Dbref( \"ns\", \"000000000000000000000000\", \"dbname\" ) }";
    }
};
DBTEST_SHIM_TEST(DBRefConstructorDbName);

class DBRefConstructorNumber : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        BSONObjBuilder subBuilder(b.subobjStart("a"));
        subBuilder.append("$ref", "ns");
        subBuilder.append("$id", 1);
        subBuilder.done();
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : Dbref( \"ns\", 1 ) }";
    }
};
DBTEST_SHIM_TEST(DBRefConstructorNumber);

class DBRefConstructorObject : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        BSONObjBuilder subBuilder(b.subobjStart("a"));
        subBuilder.append("$ref", "ns");
        BSONObjBuilder idSubBuilder(subBuilder.subobjStart("$id"));
        idSubBuilder.append("b", true);
        idSubBuilder.done();
        subBuilder.done();
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : Dbref( \"ns\", { \"b\" : true } ) }";
    }
};
DBTEST_SHIM_TEST(DBRefConstructorObject);

class DBRefNumberId : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        BSONObjBuilder subBuilder(b.subobjStart("a"));
        subBuilder.append("$ref", "ns");
        subBuilder.append("$id", 1);
        subBuilder.done();
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$ref\" : \"ns\", \"$id\" : 1 } }";
    }
};
DBTEST_SHIM_TEST(DBRefNumberId);

class DBRefObjectAsId : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        BSONObjBuilder subBuilder(b.subobjStart("a"));
        subBuilder.append("$ref", "ns");
        BSONObjBuilder idSubBuilder(subBuilder.subobjStart("$id"));
        idSubBuilder.append("b", true);
        idSubBuilder.done();
        subBuilder.done();
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$ref\" : \"ns\", \"$id\" : { \"b\" : true } } }";
    }
};
DBTEST_SHIM_TEST(DBRefObjectAsId);

class DBRefStringId : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        BSONObjBuilder subBuilder(b.subobjStart("a"));
        subBuilder.append("$ref", "ns");
        subBuilder.append("$id", "000000000000000000000000");
        subBuilder.done();
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$ref\" : \"ns\", \"$id\" : \"000000000000000000000000\" } }";
    }
};
DBTEST_SHIM_TEST(DBRefStringId);

class DBRefObjectIDObject : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        OID o;
        BSONObjBuilder subBuilder(b.subobjStart("a"));
        subBuilder.append("$ref", "ns");
        subBuilder.append("$id", o);
        subBuilder.done();
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$ref\" : \"ns\", \"$id\" : { \"$oid\" : \"000000000000000000000000\" "
               "} } }";
    }
};
DBTEST_SHIM_TEST(DBRefObjectIDObject);

class DBRefObjectIDConstructor : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        OID o;
        BSONObjBuilder subBuilder(b.subobjStart("a"));
        subBuilder.append("$ref", "ns");
        subBuilder.append("$id", o);
        subBuilder.done();
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$ref\" : \"ns\", \"$id\" : ObjectId( \"000000000000000000000000\" ) "
               "} }";
    }
};
DBTEST_SHIM_TEST(DBRefObjectIDConstructor);

class DBRefDbName : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        BSONObjBuilder subBuilder(b.subobjStart("a"));
        subBuilder.append("$ref", "ns");
        subBuilder.append("$id", "000000000000000000000000");
        subBuilder.append("$db", "dbname");
        subBuilder.done();
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$ref\" : \"ns\", \"$id\" : \"000000000000000000000000\""
               ", \"$db\" : \"dbname\" } }";
    }
};
DBTEST_SHIM_TEST(DBRefDbName);

class Oid : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendOID("_id");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"_id\" : { \"$oid\" : \"000000000000000000000000\" } }";
    }
};
DBTEST_SHIM_TEST(Oid);

class Oid2 : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        char OIDbytes[OID::kOIDSize];
        memset(&OIDbytes, 0x0f, OID::kOIDSize);
        OID o = OID::from(OIDbytes);
        b.appendOID("_id", &o);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"_id\" : ObjectId( \"0f0f0f0f0f0f0f0f0f0f0f0f\" ) }";
    }
};
DBTEST_SHIM_TEST(Oid2);

class OidTooLong : public Bad {
    virtual string json() const {
        return "{ \"_id\" : { \"$oid\" : \"0000000000000000000000000\" } }";
    }
};
DBTEST_SHIM_TEST(OidTooLong);

class Oid2TooLong : public Bad {
    virtual string json() const {
        return "{ \"_id\" : ObjectId( \"0f0f0f0f0f0f0f0f0f0f0f0f0\" ) }";
    }
};
DBTEST_SHIM_TEST(Oid2TooLong);

class OidTooShort : public Bad {
    virtual string json() const {
        return "{ \"_id\" : { \"$oid\" : \"00000000000000000000000\" } }";
    }
};
DBTEST_SHIM_TEST(OidTooShort);

class Oid2TooShort : public Bad {
    virtual string json() const {
        return "{ \"_id\" : ObjectId( \"0f0f0f0f0f0f0f0f0f0f0f0\" ) }";
    }
};
DBTEST_SHIM_TEST(Oid2TooShort);

class OidInvalidChar : public Bad {
    virtual string json() const {
        return "{ \"_id\" : { \"$oid\" : \"00000000000Z000000000000\" } }";
    }
};
DBTEST_SHIM_TEST(OidInvalidChar);

class Oid2InvalidChar : public Bad {
    virtual string json() const {
        return "{ \"_id\" : ObjectId( \"0f0f0f0f0f0fZf0f0f0f0f0f\" ) }";
    }
};
DBTEST_SHIM_TEST(Oid2InvalidChar);

class StringId : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("_id", "000000000000000000000000");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"_id\" : \"000000000000000000000000\" }";
    }
};
DBTEST_SHIM_TEST(StringId);

class BinData : public Base {
    virtual BSONObj bson() const {
        char z[3];
        z[0] = 'a';
        z[1] = 'b';
        z[2] = 'c';
        BSONObjBuilder b;
        b.appendBinData("a", 3, BinDataGeneral, z);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"YWJj\", \"$type\" : \"00\" } }";
    }
};

class BinData1 : public Base {
    virtual BSONObj bson() const {
        char z[3];
        z[0] = 'a';
        z[1] = 'b';
        z[2] = 'c';
        BSONObjBuilder b;
        b.appendBinData("a", 3, Function, z);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"YWJj\", \"$type\" : \"01\" } }";
    }
};
DBTEST_SHIM_TEST(BinData1);

class BinData2 : public Base {
    virtual BSONObj bson() const {
        char z[3];
        z[0] = 'a';
        z[1] = 'b';
        z[2] = 'c';
        BSONObjBuilder b;
        b.appendBinData("a", 3, ByteArrayDeprecated, z);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"YWJj\", \"$type\" : \"02\" } }";
    }
};
DBTEST_SHIM_TEST(BinData2);

class BinData3 : public Base {
    virtual BSONObj bson() const {
        char z[3];
        z[0] = 'a';
        z[1] = 'b';
        z[2] = 'c';
        BSONObjBuilder b;
        b.appendBinData("a", 3, bdtUUID, z);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"YWJj\", \"$type\" : \"03\" } }";
    }
};
DBTEST_SHIM_TEST(BinData3);

class BinData4 : public Base {
    virtual BSONObj bson() const {
        char z[3];
        z[0] = 'a';
        z[1] = 'b';
        z[2] = 'c';
        BSONObjBuilder b;
        b.appendBinData("a", 3, newUUID, z);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"YWJj\", \"$type\" : \"04\" } }";
    }
};
DBTEST_SHIM_TEST(BinData4);

class BinData5 : public Base {
    virtual BSONObj bson() const {
        char z[3];
        z[0] = 'a';
        z[1] = 'b';
        z[2] = 'c';
        BSONObjBuilder b;
        b.appendBinData("a", 3, MD5Type, z);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"YWJj\", \"$type\" : \"05\" } }";
    }
};
DBTEST_SHIM_TEST(BinData5);

class BinData80 : public Base {
    virtual BSONObj bson() const {
        char z[3];
        z[0] = 'a';
        z[1] = 'b';
        z[2] = 'c';
        BSONObjBuilder b;
        b.appendBinData("a", 3, bdtCustom, z);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"YWJj\", \"$type\" : \"80\" } }";
    }
};
DBTEST_SHIM_TEST(BinData80);

class BinDataPaddedSingle : public Base {
    virtual BSONObj bson() const {
        char z[2];
        z[0] = 'a';
        z[1] = 'b';
        BSONObjBuilder b;
        b.appendBinData("a", 2, BinDataGeneral, z);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"YWI=\", \"$type\" : \"00\" } }";
    }
};
DBTEST_SHIM_TEST(BinDataPaddedSingle);

class BinDataPaddedDouble : public Base {
    virtual BSONObj bson() const {
        char z[1];
        z[0] = 'a';
        BSONObjBuilder b;
        b.appendBinData("a", 1, BinDataGeneral, z);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"YQ==\", \"$type\" : \"00\" } }";
    }
};
DBTEST_SHIM_TEST(BinDataPaddedDouble);

class BinDataAllChars : public Base {
    virtual BSONObj bson() const {
        unsigned char z[] = {0x00, 0x10, 0x83, 0x10, 0x51, 0x87, 0x20, 0x92, 0x8B, 0x30,
                             0xD3, 0x8F, 0x41, 0x14, 0x93, 0x51, 0x55, 0x97, 0x61, 0x96,
                             0x9B, 0x71, 0xD7, 0x9F, 0x82, 0x18, 0xA3, 0x92, 0x59, 0xA7,
                             0xA2, 0x9A, 0xAB, 0xB2, 0xDB, 0xAF, 0xC3, 0x1C, 0xB3, 0xD3,
                             0x5D, 0xB7, 0xE3, 0x9E, 0xBB, 0xF3, 0xDF, 0xBF};
        BSONObjBuilder b;
        b.appendBinData("a", 48, BinDataGeneral, z);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : "
               "\"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/\", \"$type\" : "
               "\"00\" } }";
    }
};
DBTEST_SHIM_TEST(BinDataAllChars);

class BinDataBadLength : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"YQ=\", \"$type\" : \"00\" } }";
    }
};
DBTEST_SHIM_TEST(BinDataBadLength);

class BinDataBadLength1 : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"YQ\", \"$type\" : \"00\" } }";
    }
};
DBTEST_SHIM_TEST(BinDataBadLength1);

class BinDataBadLength2 : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"YQX==\", \"$type\" : \"00\" } }";
    }
};
DBTEST_SHIM_TEST(BinDataBadLength2);

class BinDataBadLength3 : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"YQX\", \"$type\" : \"00\" } }";
    }
};
DBTEST_SHIM_TEST(BinDataBadLength3);

class BinDataBadLength4 : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"YQXZ=\", \"$type\" : \"00\" } }";
    }
};
DBTEST_SHIM_TEST(BinDataBadLength4);

class BinDataBadLength5 : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"YQXZ==\", \"$type\" : \"00\" } }";
    }
};
DBTEST_SHIM_TEST(BinDataBadLength5);

class BinDataBadChars : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"a...\", \"$type\" : \"00\" } }";
    }
};
DBTEST_SHIM_TEST(BinDataBadChars);

class BinDataTypeTooShort : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"AAAA\", \"$type\" : \"0\" } }";
    }
};
DBTEST_SHIM_TEST(BinDataTypeTooShort);

class BinDataTypeTooLong : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"AAAA\", \"$type\" : \"000\" } }";
    }
};
DBTEST_SHIM_TEST(BinDataTypeTooLong);

class BinDataTypeBadChars : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"AAAA\", \"$type\" : \"ZZ\" } }";
    }
};
DBTEST_SHIM_TEST(BinDataTypeBadChars);

class BinDataEmptyType : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"AAAA\", \"$type\" : \"\" } }";
    }
};
DBTEST_SHIM_TEST(BinDataEmptyType);

class BinDataNoType : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"AAAA\" } }";
    }
};
DBTEST_SHIM_TEST(BinDataNoType);

class BinDataInvalidType : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$binary\" : \"AAAA\", \"$type\" : \"100\" } }";
    }
};
DBTEST_SHIM_TEST(BinDataInvalidType);

class Date : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendDate("a", 0);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$date\" : 0 } }";
    }
};  // DBTEST_SHIM_TEST(Date); SEE COMMENT IN OLD TEST REGISTRATION SECTION

class DateNegZero : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendDate("a", -0);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$date\" : -0 } }";
    }
};  // DBTEST_SHIM_TEST(DateNegZero); SEE COMMENT IN OLD TEST REGISTRATION SECTION

class DateNonzero : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendDate("a", 1000000000);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$date\" : 1000000000 } }";
    }
};
DBTEST_SHIM_TEST(DateNonzero);

class DateStrictTooLong : public Bad {
    virtual string json() const {
        stringstream ss;
        ss << "{ \"a\" : { \"$date\" : " << ~(0ULL) << "1"
           << " } }";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(DateStrictTooLong);

class DateTooLong : public Bad {
    virtual string json() const {
        stringstream ss;
        ss << "{ \"a\" : Date( " << ~(0ULL) << "1"
           << " ) }";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(DateTooLong);

class DateIsString : public Bad {
    virtual string json() const {
        stringstream ss;
        ss << "{ \"a\" : { \"$date\" : \"100\" } }";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(DateIsString);

class DateIsString1 : public Bad {
    virtual string json() const {
        stringstream ss;
        ss << "{ \"a\" : Date(\"a\") }";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(DateIsString1);

class DateIsString2 : public Bad {
    virtual string json() const {
        stringstream ss;
        ss << "{ \"a\" : new Date(\"a\") }";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(DateIsString2);

class DateIsFloat : public Bad {
    virtual string json() const {
        stringstream ss;
        ss << "{ \"a\" : { \"$date\" : 1.1 } }";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(DateIsFloat);

class DateIsFloat1 : public Bad {
    virtual string json() const {
        stringstream ss;
        ss << "{ \"a\" : Date(1.1) }";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(DateIsFloat1);

class DateIsFloat2 : public Bad {
    virtual string json() const {
        stringstream ss;
        ss << "{ \"a\" : new Date(1.1) }";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(DateIsFloat2);

class DateIsExponent : public Bad {
    virtual string json() const {
        stringstream ss;
        ss << "{ \"a\" : { \"$date\" : 10e3 } }";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(DateIsExponent);

class DateIsExponent1 : public Bad {
    virtual string json() const {
        stringstream ss;
        ss << "{ \"a\" : Date(10e3) }";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(DateIsExponent1);

class DateIsExponent2 : public Bad {
    virtual string json() const {
        stringstream ss;
        ss << "{ \"a\" : new Date(10e3) }";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(DateIsExponent2);
/* Need to handle this because jsonString outputs the value of Date_t as unsigned.
 * See SERVER-8330 and SERVER-8573 */
class DateStrictMaxUnsigned : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendDate("a", -1);
        return b.obj();
    }
    virtual string json() const {
        stringstream ss;
        ss << "{ \"a\" : { \"$date\" : " << std::numeric_limits<unsigned long long>::max()
           << " } }";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(DateStrictMaxUnsigned);

class DateMaxUnsigned : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendDate("a", -1);
        return b.obj();
    }
    virtual string json() const {
        stringstream ss;
        ss << "{ \"a\" : Date( " << std::numeric_limits<unsigned long long>::max() << " ) }";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(DateMaxUnsigned);

class DateStrictNegative : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendDate("a", -1);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$date\" : -1 } }";
    }
};
DBTEST_SHIM_TEST(DateStrictNegative);

class DateNegative : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendDate("a", -1);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : Date( -1 ) }";
    }
};
DBTEST_SHIM_TEST(DateNegative);

class NumberLongTest : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendNumber("a", 20000LL);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : NumberLong( 20000 ) }";
    }
};
DBTEST_SHIM_TEST(NumberLongTest);

class NumberLongMin : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendNumber("a", std::numeric_limits<long long>::min());
        return b.obj();
    }
    virtual string json() const {
        std::stringstream ss;
        ss << "{'a': NumberLong(";
        ss << std::numeric_limits<long long>::min();
        ss << ") }";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(NumberLongMin);

class NumberIntTest : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendNumber("a", 20000);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : NumberInt( 20000 ) }";
    }
};
DBTEST_SHIM_TEST(NumberIntTest);

class NumberLongNeg : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendNumber("a", -20000LL);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : NumberLong( -20000 ) }";
    }
};
DBTEST_SHIM_TEST(NumberLongNeg);

class NumberIntNeg : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendNumber("a", -20000);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : NumberInt( -20000 ) }";
    }
};
DBTEST_SHIM_TEST(NumberIntNeg);

class NumberLongBad : public Bad {
    virtual string json() const {
        return "{ \"a\" : NumberLong( 'sdf' ) }";
    }
};
DBTEST_SHIM_TEST(NumberLongBad);

class NumberIntBad : public Bad {
    virtual string json() const {
        return "{ \"a\" : NumberInt( 'sdf' ) }";
    }
};
DBTEST_SHIM_TEST(NumberIntBad);

class Timestamp : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendTimestamp("a", Timestamp_t(20, 5));
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : Timestamp( 20, 5 ) }";
    }
};
DBTEST_SHIM_TEST(Timestamp);

class TimestampNoIncrement : public Bad {
    virtual string json() const {
        return "{ \"a\" : Timestamp( 20 ) }";
    }
};
DBTEST_SHIM_TEST(TimestampNoIncrement);

class TimestampZero : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendTimestamp("a", Timestamp_t());
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : Timestamp( 0, 0 ) }";
    }
};
DBTEST_SHIM_TEST(TimestampZero);

class TimestampNoArgs : public Bad {
    virtual string json() const {
        return "{ \"a\" : Timestamp() }";
    }
};
DBTEST_SHIM_TEST(TimestampNoArgs);

class TimestampFloatSeconds : public Bad {
    virtual string json() const {
        return "{ \"a\" : Timestamp( 20.0, 1 ) }";
    }
};
DBTEST_SHIM_TEST(TimestampFloatSeconds);

class TimestampFloatIncrement : public Bad {
    virtual string json() const {
        return "{ \"a\" : Timestamp( 20, 1.0 ) }";
    }
};
DBTEST_SHIM_TEST(TimestampFloatIncrement);

class TimestampNegativeSeconds : public Bad {
    virtual string json() const {
        return "{ \"a\" : Timestamp( -20, 5 ) }";
    }
};
DBTEST_SHIM_TEST(TimestampNegativeSeconds);

class TimestampNegativeIncrement : public Bad {
    virtual string json() const {
        return "{ \"a\" : Timestamp( 20, -5 ) }";
    }
};
DBTEST_SHIM_TEST(TimestampNegativeIncrement);

class TimestampInvalidSeconds : public Bad {
    virtual string json() const {
        return "{ \"a\" : Timestamp( q, 5 ) }";
    }
};
DBTEST_SHIM_TEST(TimestampInvalidSeconds);

class TimestampObject : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendTimestamp("a", Timestamp_t(20, 5));
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$timestamp\" : { \"t\" : 20 , \"i\" : 5 } } }";
    }
};
DBTEST_SHIM_TEST(TimestampObject);

class TimestampObjectInvalidFieldName : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$timestamp\" : { \"time\" : 20 , \"increment\" : 5 } } }";
    }
};
DBTEST_SHIM_TEST(TimestampObjectInvalidFieldName);

class TimestampObjectNoIncrement : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$timestamp\" : { \"t\" : 20 } } }";
    }
};
DBTEST_SHIM_TEST(TimestampObjectNoIncrement);

class TimestampObjectNegativeSeconds : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$timestamp\" : { \"t\" : -20 , \"i\" : 5 } } }";
    }
};
DBTEST_SHIM_TEST(TimestampObjectNegativeSeconds);

class TimestampObjectNegativeIncrement : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$timestamp\" : { \"t\" : 20 , \"i\" : -5 } } }";
    }
};
DBTEST_SHIM_TEST(TimestampObjectNegativeIncrement);

class TimestampObjectInvalidSeconds : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$timestamp\" : { \"t\" : q , \"i\" : 5 } } }";
    }
};
DBTEST_SHIM_TEST(TimestampObjectInvalidSeconds);

class TimestampObjectZero : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendTimestamp("a", Timestamp_t());
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$timestamp\" : { \"t\" : 0, \"i\" : 0} } }";
    }
};
DBTEST_SHIM_TEST(TimestampObjectZero);

class TimestampObjectNoArgs : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$timestamp\" : { } } }";
    }
};
DBTEST_SHIM_TEST(TimestampObjectNoArgs);

class TimestampObjectFloatSeconds : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$timestamp\" : { \"t\" : 1.0, \"i\" : 0} } }";
    }
};
DBTEST_SHIM_TEST(TimestampObjectFloatSeconds);

class TimestampObjectFloatIncrement : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$timestamp\" : { \"t\" : 20, \"i\" : 1.0} } }";
    }
};
DBTEST_SHIM_TEST(TimestampObjectFloatIncrement);

class Regex : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendRegex("a", "b", "i");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$regex\" : \"b\", \"$options\" : \"i\" } }";
    }
};
DBTEST_SHIM_TEST(Regex);

class RegexNoOptionField : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendRegex("a", "b", "");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$regex\" : \"b\" } }";
    }
};
DBTEST_SHIM_TEST(RegexNoOptionField);

class RegexEscape : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendRegex("a", "\t", "i");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$regex\" : \"\\t\", \"$options\" : \"i\" } }";
    }
};
DBTEST_SHIM_TEST(RegexEscape);

class RegexWithQuotes : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendRegex("a", "\"", "");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : /\"/ }";
    }
};
DBTEST_SHIM_TEST(RegexWithQuotes);

class RegexWithQuotes1 : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendRegex("a", "\"", "");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { $regex : \"\\\"\" }}";
    }
};
DBTEST_SHIM_TEST(RegexWithQuotes1);

class RegexInvalidField : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$regex\" : \"b\", \"field\" : \"i\" } }";
    }
};
DBTEST_SHIM_TEST(RegexInvalidField);

class RegexInvalidOption : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$regex\" : \"b\", \"$options\" : \"1\" } }";
    }
};
DBTEST_SHIM_TEST(RegexInvalidOption);

class RegexInvalidOption2 : public Bad {
    virtual string json() const {
        return "{ \"a\" : /b/c }";
    }
};
DBTEST_SHIM_TEST(RegexInvalidOption2);

class RegexInvalidOption3 : public Bad {
    virtual string json() const {
        return "{ \"a\" : /b/ic }";
    }
};
DBTEST_SHIM_TEST(RegexInvalidOption3);

class RegexInvalidOption4 : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$regex\" : \"b\", \"$options\" : \"a\" } }";
    }
};
DBTEST_SHIM_TEST(RegexInvalidOption4);

class RegexInvalidOption5 : public Bad {
    virtual string json() const {
        return "{ \"a\" : /b/a }";
    }
};
DBTEST_SHIM_TEST(RegexInvalidOption5);

class RegexEmptyOption : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendRegex("a", "b", "");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$regex\" : \"b\", \"$options\" : \"\" } }";
    }
};
DBTEST_SHIM_TEST(RegexEmptyOption);

class RegexEmpty : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendRegex("a", "", "");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$regex\" : \"\", \"$options\" : \"\"} }";
    }
};
DBTEST_SHIM_TEST(RegexEmpty);

class RegexEmpty1 : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendRegex("a", "", "");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" :  //  }";
    }
};
DBTEST_SHIM_TEST(RegexEmpty1);

class RegexOverlap : public Bad {
    virtual string json() const {
        return "{ \"a\" : { \"$regex\" : // } }";
    }
};
DBTEST_SHIM_TEST(RegexOverlap);

class Malformed : public Bad {
    string json() const {
        return "{";
    }
};
DBTEST_SHIM_TEST(Malformed);

class Malformed1 : public Bad {
    string json() const {
        return "}";
    }
};
DBTEST_SHIM_TEST(Malformed1);

class Malformed2 : public Bad {
    string json() const {
        return "{test}";
    }
};
DBTEST_SHIM_TEST(Malformed2);

class Malformed3 : public Bad {
    string json() const {
        return "{test";
    }
};
DBTEST_SHIM_TEST(Malformed3);

class Malformed4 : public Bad {
    string json() const {
        return "{ test : 1";
    }
};
DBTEST_SHIM_TEST(Malformed4);

class Malformed5 : public Bad {
    string json() const {
        return "{ test : 1 , }";
    }
};
DBTEST_SHIM_TEST(Malformed5);

class Malformed6 : public Bad {
    string json() const {
        return "{ test : 1 , tst}";
    }
};
DBTEST_SHIM_TEST(Malformed6);

class Malformed7 : public Bad {
    string json() const {
        return "{ a : []";
    }
};
DBTEST_SHIM_TEST(Malformed7);

class Malformed8 : public Bad {
    string json() const {
        return "{ a : { test : 1 }";
    }
};
DBTEST_SHIM_TEST(Malformed8);

class Malformed9 : public Bad {
    string json() const {
        return "{ a : [ { test : 1]}";
    }
};
DBTEST_SHIM_TEST(Malformed9);

class Malformed10 : public Bad {
    string json() const {
        return "{ a : [ { test : 1], b : 2}";
    }
};
DBTEST_SHIM_TEST(Malformed10);

class Malformed11 : public Bad {
    string json() const {
        return "{ a : \"test\"string }";
    }
};
DBTEST_SHIM_TEST(Malformed11);

class Malformed12 : public Bad {
    string json() const {
        return "{ a : test\"string\" }";
    }
};
DBTEST_SHIM_TEST(Malformed12);

class Malformed13 : public Bad {
    string json() const {
        return "{ a\"bad\" : \"teststring\" }";
    }
};
DBTEST_SHIM_TEST(Malformed13);

class Malformed14 : public Bad {
    string json() const {
        return "{ \"a\"test : \"teststring\" }";
    }
};
DBTEST_SHIM_TEST(Malformed14);

class Malformed15 : public Bad {
    string json() const {
        return "{ \"atest : \"teststring\" }";
    }
};
DBTEST_SHIM_TEST(Malformed15);

class Malformed16 : public Bad {
    string json() const {
        return "{ atest\" : \"teststring\" }";
    }
};
DBTEST_SHIM_TEST(Malformed16);

class Malformed17 : public Bad {
    string json() const {
        return "{ atest\" : 1 }";
    }
};
DBTEST_SHIM_TEST(Malformed17);

class Malformed18 : public Bad {
    string json() const {
        return "{ atest : \"teststring }";
    }
};
DBTEST_SHIM_TEST(Malformed18);

class Malformed19 : public Bad {
    string json() const {
        return "{ atest : teststring\" }";
    }
};
DBTEST_SHIM_TEST(Malformed19);

class UnquotedFieldName : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("a_b", 1);
        return b.obj();
    }
    virtual string json() const {
        return "{ a_b : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldName);

class UnquotedFieldNameBad : public Bad {
    string json() const {
        return "{ 123 : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad);

class UnquotedFieldNameBad1 : public Bad {
    string json() const {
        return "{ -123 : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad1);

class UnquotedFieldNameBad2 : public Bad {
    string json() const {
        return "{ .123 : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad2);

class UnquotedFieldNameBad3 : public Bad {
    string json() const {
        return "{ -.123 : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad3);

class UnquotedFieldNameBad4 : public Bad {
    string json() const {
        return "{ -1.23 : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad4);

class UnquotedFieldNameBad5 : public Bad {
    string json() const {
        return "{ 1e23 : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad5);

class UnquotedFieldNameBad6 : public Bad {
    string json() const {
        return "{ -1e23 : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad6);

class UnquotedFieldNameBad7 : public Bad {
    string json() const {
        return "{ -1e-23 : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad7);

class UnquotedFieldNameBad8 : public Bad {
    string json() const {
        return "{ -hello : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad8);

class UnquotedFieldNameBad9 : public Bad {
    string json() const {
        return "{ il.legal : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad9);

class UnquotedFieldNameBad10 : public Bad {
    string json() const {
        return "{ 10gen : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad10);

class UnquotedFieldNameBad11 : public Bad {
    string json() const {
        return "{ _123. : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad11);

class UnquotedFieldNameBad12 : public Bad {
    string json() const {
        return "{ he-llo : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad12);

class UnquotedFieldNameBad13 : public Bad {
    string json() const {
        return "{ bad\nchar : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad13);

class UnquotedFieldNameBad14 : public Bad {
    string json() const {
        return "{ thiswill\fail : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad14);

class UnquotedFieldNameBad15 : public Bad {
    string json() const {
        return "{ failu\re : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad15);

class UnquotedFieldNameBad16 : public Bad {
    string json() const {
        return "{ t\test : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad16);

class UnquotedFieldNameBad17 : public Bad {
    string json() const {
        return "{ \break: 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad17);

class UnquotedFieldNameBad18 : public Bad {
    string json() const {
        // here we fill the memory directly to test unicode values
        // In this case we set \u0700 and \uFF00
        // Setting it directly in memory avoids MSVC error c4566
        unsigned char u[6];
        u[0] = 0xdc;
        u[1] = 0x80;

        u[2] = 0xef;
        u[3] = 0xbc;
        u[4] = 0x80;

        u[5] = 0;
        std::stringstream ss;
        ss << "{ " << u << " : 1 }";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad18);

class UnquotedFieldNameBad19 : public Bad {
    string json() const {
        return "{ bl\\u3333p: 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad19);

class UnquotedFieldNameBad20 : public Bad {
    string json() const {
        return "{ bl-33p: 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameBad20);

class UnquotedFieldNameDollar : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("$a_b", 1);
        return b.obj();
    }
    virtual string json() const {
        return "{ $a_b : 1 }";
    }
};
DBTEST_SHIM_TEST(UnquotedFieldNameDollar);

class SingleQuotes : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("ab'c\"", "bb\b '\"");
        return b.obj();
    }
    virtual string json() const {
        return "{ 'ab\\'c\"' : 'bb\\b \\'\"' }";
    }
};
DBTEST_SHIM_TEST(SingleQuotes);

class QuoteTest : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("\"", "test");
        return b.obj();
    }
    virtual string json() const {
        return "{ '\"' : \"test\" }";
    }
};
DBTEST_SHIM_TEST(QuoteTest);

class QuoteTest1 : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("'", "test");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"'\" : \"test\" }";
    }
};
DBTEST_SHIM_TEST(QuoteTest1);

class QuoteTest2 : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("\"", "test");
        return b.obj();
    }
    virtual string json() const {
        return "{ '\"' : \"test\" }";
    }
};
DBTEST_SHIM_TEST(QuoteTest2);

class QuoteTest3 : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("\"'\"", "test");
        return b.obj();
    }
    virtual string json() const {
        return "{ '\"\\\'\"' : \"test\" }";
    }
};
DBTEST_SHIM_TEST(QuoteTest3);

class QuoteTest4 : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("'\"'", "test");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"'\\\"'\" : \"test\" }";
    }
};
DBTEST_SHIM_TEST(QuoteTest4);

class QuoteTest5 : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("test", "'");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"test\" : \"'\" }";
    }
};
DBTEST_SHIM_TEST(QuoteTest5);

class QuoteTest6 : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("test", "\"");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"test\" : '\"' }";
    }
};
DBTEST_SHIM_TEST(QuoteTest6);

class ObjectId : public Base {
    virtual BSONObj bson() const {
        OID id;
        id.init("deadbeeff00ddeadbeeff00d");
        BSONObjBuilder b;
        b.appendOID("_id", &id);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"_id\": ObjectId( \"deadbeeff00ddeadbeeff00d\" ) }";
    }
};
DBTEST_SHIM_TEST(ObjectId);

class ObjectId2 : public Base {
    virtual BSONObj bson() const {
        OID id;
        id.init("deadbeeff00ddeadbeeff00d");
        BSONObjBuilder b;
        b.appendOID("foo", &id);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"foo\": ObjectId( \"deadbeeff00ddeadbeeff00d\" ) }";
    }
};
DBTEST_SHIM_TEST(ObjectId2);

class NumericTypes : public Base {
public:
    void run() {
        Base::run();

        BSONObj o = fromjson(json());

        ASSERT(o["int"].type() == NumberInt);
        ASSERT(o["long"].type() == NumberLong);
        ASSERT(o["double"].type() == NumberDouble);

        ASSERT(o["long"].numberLong() == 9223372036854775807ll);
    }

    virtual BSONObj bson() const {
        return BSON("int" << 123 << "long" << 9223372036854775807ll  // 2**63 - 1
                          << "double" << 3.14);
    }
    virtual string json() const {
        return "{ \"int\": 123, \"long\": 9223372036854775807, \"double\": 3.14 }";
    }
};
DBTEST_SHIM_TEST(NumericTypes);

class NumericTypesJS : public Base {
public:
    void run() {
        Base::run();

        BSONObj o = fromjson(json());

        ASSERT(o["int"].type() == NumberInt);
        ASSERT(o["long"].type() == NumberLong);
        ASSERT(o["double"].type() == NumberDouble);

        ASSERT(o["long"].numberLong() == 9223372036854775807ll);
    }

    virtual BSONObj bson() const {
        return BSON("int" << 123 << "long" << 9223372036854775807ll  // 2**63 - 1
                          << "double" << 3.14);
    }
    virtual string json() const {
        return "{ 'int': NumberInt(123), "
               "'long': NumberLong(9223372036854775807), "
               "'double': 3.14 }";
    }
};
DBTEST_SHIM_TEST(NumericTypesJS);

class NumericLongMin : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendNumber("a", std::numeric_limits<long long>::min());
        return b.obj();
    }
    virtual string json() const {
        std::stringstream ss;
        ss << "{'a': ";
        ss << std::numeric_limits<long long>::min();
        ss << " }";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(NumericLongMin);

class NumericIntMin : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendNumber("a", std::numeric_limits<int>::min());
        return b.obj();
    }
    virtual string json() const {
        std::stringstream ss;
        ss << "{'a': ";
        ss << std::numeric_limits<int>::min();
        ss << " }";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(NumericIntMin);


class NumericLimits : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder builder;
        BSONArrayBuilder numArray(builder.subarrayStart(""));
        numArray.append(std::numeric_limits<long long>::max());
        numArray.append(std::numeric_limits<long long>::min());
        numArray.append(std::numeric_limits<int>::max());
        numArray.append(std::numeric_limits<int>::min());
        numArray.done();
        return builder.obj();
    }
    virtual string json() const {
        std::stringstream ss;
        ss << "{'': [";
        ss << std::numeric_limits<long long>::max() << ",";
        ss << std::numeric_limits<long long>::min() << ",";
        ss << std::numeric_limits<int>::max() << ",";
        ss << std::numeric_limits<int>::min();
        ss << "] }";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(NumericLimits);

// Overflows double by giving it an exponent that is too large
class NumericLimitsBad : public Bad {
    virtual string json() const {
        std::stringstream ss;
        ss << "{ test : ";
        ss << std::numeric_limits<double>::max() << "1111111111";
        ss << "}";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(NumericLimitsBad);

class NumericLimitsBad1 : public Bad {
    virtual string json() const {
        std::stringstream ss;
        ss << "{ test : ";
        ss << std::numeric_limits<double>::min() << "11111111111";
        ss << "}";
        return ss.str();
    }
};
DBTEST_SHIM_TEST(NumericLimitsBad1);

class NegativeNumericTypes : public Base {
public:
    void run() {
        Base::run();

        BSONObj o = fromjson(json());

        ASSERT(o["int"].type() == NumberInt);
        ASSERT(o["long"].type() == NumberLong);
        ASSERT(o["double"].type() == NumberDouble);

        ASSERT(o["long"].numberLong() == -9223372036854775807ll);
    }

    virtual BSONObj bson() const {
        return BSON("int" << -123 << "long" << -9223372036854775807ll  // -1 * (2**63 - 1)
                          << "double" << -3.14);
    }
    virtual string json() const {
        return "{ \"int\": -123, \"long\": -9223372036854775807, \"double\": -3.14 }";
    }
};
DBTEST_SHIM_TEST(NegativeNumericTypes);

class EmbeddedDatesBase : public Base {
public:
    virtual void run() {
        BSONObj o = fromjson(json());
        ASSERT_EQUALS(3, (o["time.valid"].type()));
        BSONObj e = o["time.valid"].embeddedObjectUserCheck();
        ASSERT_EQUALS(9, e["$gt"].type());
        ASSERT_EQUALS(9, e["$lt"].type());
        Base::run();
    }

    BSONObj bson() const {
        BSONObjBuilder e;
        e.appendDate("$gt", 1257829200000LL);
        e.appendDate("$lt", 1257829200100LL);

        BSONObjBuilder b;
        b.append("time.valid", e.obj());
        return b.obj();
    }
    virtual string json() const = 0;
};

struct EmbeddedDatesFormat1 : EmbeddedDatesBase {
    string json() const {
        return "{ \"time.valid\" : { $gt : { \"$date\" :  1257829200000 } , $lt : { \"$date\" : "
               "1257829200100 } } }";
    }
};
DBTEST_SHIM_TEST(EmbeddedDatesFormat1);
struct EmbeddedDatesFormat2 : EmbeddedDatesBase {
    string json() const {
        return "{ \"time.valid\" : { $gt : Date(1257829200000) , $lt : Date( 1257829200100 ) } }";
    }
};
DBTEST_SHIM_TEST(EmbeddedDatesFormat2);
struct EmbeddedDatesFormat3 : EmbeddedDatesBase {
    string json() const {
        return "{ \"time.valid\" : { $gt : new Date(1257829200000) , $lt : new Date( 1257829200100 "
               ") } }";
    }
};
DBTEST_SHIM_TEST(EmbeddedDatesFormat3);

class NullString : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.append("x", "a\0b", 4);
        return b.obj();
    }
    virtual string json() const {
        return "{ \"x\" : \"a\\u0000b\" }";
    }
};
DBTEST_SHIM_TEST(NullString);

class NullFieldUnquoted : public Bad {
    virtual string json() const {
        return "{ x\\u0000y : \"a\" }";
    }
};
DBTEST_SHIM_TEST(NullFieldUnquoted);

class MinKeyAlone : public Bad {
    virtual string json() const {
        return "{ \"$minKey\" : 1 }";
    }
};
DBTEST_SHIM_TEST(MinKeyAlone);

class MaxKeyAlone : public Bad {
    virtual string json() const {
        return "{ \"$maxKey\" : 1 }";
    }
};
DBTEST_SHIM_TEST(MaxKeyAlone);

class MinKey : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendMinKey("a");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$minKey\" : 1 } }";
    }
};
DBTEST_SHIM_TEST(MinKey);

class MaxKey : public Base {
    virtual BSONObj bson() const {
        BSONObjBuilder b;
        b.appendMaxKey("a");
        return b.obj();
    }
    virtual string json() const {
        return "{ \"a\" : { \"$maxKey\" : 1 } }";
    }
};
DBTEST_SHIM_TEST(MaxKey);


}  // namespace FromJsonTests

}  // namespace JsonTests
