/** @file jsobj.cpp - BSON implementation
    http://dochub.mongodb.org/core/bson
*/

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

#include "mongo/db/jsobj.h"

#include <limits>
#include <cmath>

#include <boost/lexical_cast.hpp>
#include <boost/static_assert.hpp>

#include "mongo/bson/bson_validate.h"
#include "mongo/bson/oid.h"
#include "mongo/bson/util/atomic_int.h"
#include "mongo/db/json.h"
#include "mongo/bson/optime.h"
#include "mongo/platform/float_utils.h"
#include "mongo/util/base64.h"
#include "mongo/util/embedded_builder.h"
#include "mongo/util/md5.hpp"
#include "mongo/util/mongoutils/str.h"
#include "mongo/util/stringutils.h"
#include "mongo/util/time_support.h"


// make sure our assumptions are valid
BOOST_STATIC_ASSERT( sizeof(short) == 2 );
BOOST_STATIC_ASSERT( sizeof(int) == 4 );
BOOST_STATIC_ASSERT( sizeof(long long) == 8 );
BOOST_STATIC_ASSERT( sizeof(double) == 8 );
BOOST_STATIC_ASSERT( sizeof(mongo::Date_t) == 8 );
BOOST_STATIC_ASSERT( sizeof(mongo::OID) == 12 );

namespace mongo {

    using std::dec;
    using std::endl;
    using std::hex;
    using std::numeric_limits;
    using std::set;
    using std::string;
    using std::stringstream;

    namespace str = mongoutils::str;

    BSONElement eooElement;

    GENOIDLabeler GENOID;

    DateNowLabeler DATENOW;
    NullLabeler BSONNULL;
    UndefinedLabeler BSONUndefined;

    MinKeyLabeler MINKEY;
    MaxKeyLabeler MAXKEY;

    // need to move to bson/, but has dependency on base64 so move that to bson/util/ first.
    inline string BSONElement::jsonString( JsonStringFormat format, bool includeFieldNames, int pretty ) const {
        int sign;

        stringstream s;
        if ( includeFieldNames )
            s << '"' << escape( fieldName() ) << "\" : ";
        switch ( type() ) {
        case mongo::String:
        case Symbol:
            s << '"' << escape( string(valuestr(), valuestrsize()-1) ) << '"';
            break;
        case NumberLong:
            if (format == TenGen) {
                s << "NumberLong(" << _numberLong() << ")";
            }
            else {
                s << "{ \"$numberLong\" : \"" << _numberLong() << "\" }";
            }
            break;
        case NumberInt:
            if(format == JS) {
                s << "NumberInt(" << _numberInt() << ")";
                break;
            }
        case NumberDouble:
            if ( number() >= -numeric_limits< double >::max() &&
                    number() <= numeric_limits< double >::max() ) {
                s.precision( 16 );
                s << number();
            }
            // This is not valid JSON, but according to RFC-4627, "Numeric values that cannot be
            // represented as sequences of digits (such as Infinity and NaN) are not permitted." so
            // we are accepting the fact that if we have such values we cannot output valid JSON.
            else if ( mongo::isNaN(number()) ) {
                s << "NaN";
            }
            else if ( mongo::isInf(number(), &sign) ) {
                s << ( sign == 1 ? "Infinity" : "-Infinity");
            }
            else {
                StringBuilder ss;
                ss << "Number " << number() << " cannot be represented in JSON";
                string message = ss.str();
                massert( 10311 ,  message.c_str(), false );
            }
            break;
        case mongo::Bool:
            s << ( boolean() ? "true" : "false" );
            break;
        case jstNULL:
            s << "null";
            break;
        case Undefined:
            if ( format == Strict ) {
                s << "{ \"$undefined\" : true }";
            }
            else {
                s << "undefined";
            }
            break;
        case Object:
            s << embeddedObject().jsonString( format, pretty );
            break;
        case mongo::Array: {
            if ( embeddedObject().isEmpty() ) {
                s << "[]";
                break;
            }
            s << "[ ";
            BSONObjIterator i( embeddedObject() );
            BSONElement e = i.next();
            if ( !e.eoo() ) {
                int count = 0;
                while ( 1 ) {
                    if( pretty ) {
                        s << '\n';
                        for( int x = 0; x < pretty; x++ )
                            s << "  ";
                    }

                    if (strtol(e.fieldName(), 0, 10) > count) {
                        s << "undefined";
                    }
                    else {
                        s << e.jsonString( format, false, pretty?pretty+1:0 );
                        e = i.next();
                    }
                    count++;
                    if ( e.eoo() )
                        break;
                    s << ", ";
                }
            }
            s << " ]";
            break;
        }
        case DBRef: {
            mongo::OID *x = (mongo::OID *) (valuestr() + valuestrsize());
            if ( format == TenGen )
                s << "Dbref( ";
            else
                s << "{ \"$ref\" : ";
            s << '"' << valuestr() << "\", ";
            if ( format != TenGen )
                s << "\"$id\" : ";
            s << '"' << *x << "\" ";
            if ( format == TenGen )
                s << ')';
            else
                s << '}';
            break;
        }
        case jstOID:
            if ( format == TenGen ) {
                s << "ObjectId( ";
            }
            else {
                s << "{ \"$oid\" : ";
            }
            s << '"' << __oid() << '"';
            if ( format == TenGen ) {
                s << " )";
            }
            else {
                s << " }";
            }
            break;
        case BinData: {
            const int len = *( reinterpret_cast<const int*>( value() ) );
            BinDataType type = BinDataType( *( reinterpret_cast<const unsigned char*>( value() ) +
                                               sizeof( int ) ) );
            s << "{ \"$binary\" : \"";
            const char *start = reinterpret_cast<const char*>( value() ) + sizeof( int ) + 1;
            base64::encode( s , start , len );
            s << "\", \"$type\" : \"" << hex;
            s.width( 2 );
            s.fill( '0' );
            s << type << dec;
            s << "\" }";
            break;
        }
        case mongo::Date:
            if (format == Strict) {
                Date_t d = date();
                s << "{ \"$date\" : ";
                if (static_cast<long long>(d.millis) < 0) {
                    s << "{ \"$numberLong\" : \"" << static_cast<long long>(d.millis) << "\" }";
                }
                else {
                    s << "\"" << dateToISOStringLocal(date()) << "\"";
                }
                s << " }";
            }
            else {
                s << "Date( ";
                if (pretty) {
                    Date_t d = date();
                    if (static_cast<long long>(d.millis) < 0) {
                        // FIXME: This is not parseable by the shell, since it may not fit in a
                        // float
                        s << d.millis;
                    }
                    else {
                        s << "\"" << dateToISOStringLocal(date()) << "\"";
                    }
                }
                else {
                    s << date().asInt64();
                }
                s << " )";
            }
            break;
        case RegEx:
            if ( format == Strict ) {
                s << "{ \"$regex\" : \"" << escape( regex() );
                s << "\", \"$options\" : \"" << regexFlags() << "\" }";
            }
            else {
                s << "/" << escape( regex() , true ) << "/";
                // FIXME Worry about alpha order?
                for ( const char *f = regexFlags(); *f; ++f ) {
                    switch ( *f ) {
                    case 'g':
                    case 'i':
                    case 'm':
                        s << *f;
                    default:
                        break;
                    }
                }
            }
            break;

        case CodeWScope: {
            BSONObj scope = codeWScopeObject();
            if ( ! scope.isEmpty() ) {
                s << "{ \"$code\" : \"" << escape(_asCode()) << "\" , "
                  << "\"$scope\" : " << scope.jsonString() << " }";
                break;
            }
        }

        case Code:
            s << "\"" << escape(_asCode()) << "\"";
            break;

        case Timestamp:
            if ( format == TenGen ) {
                s << "Timestamp( " << ( timestampTime() / 1000 ) << ", " << timestampInc() << " )";
            }
            else {
                s << "{ \"$timestamp\" : { \"t\" : " << ( timestampTime() / 1000 ) << ", \"i\" : " << timestampInc() << " } }";
            }
            break;

        case MinKey:
            s << "{ \"$minKey\" : 1 }";
            break;

        case MaxKey:
            s << "{ \"$maxKey\" : 1 }";
            break;

        default:
            StringBuilder ss;
            ss << "Cannot create a properly formatted JSON string with "
               << "element: " << toString() << " of type: " << type();
            string message = ss.str();
            massert( 10312 ,  message.c_str(), false );
        }
        return s.str();
    }

    int BSONElement::getGtLtOp( int def ) const {
        const char *fn = fieldName();
        if ( fn[0] == '$' && fn[1] ) {
            if ( fn[2] == 't' ) {
                if ( fn[1] == 'g' ) {
                    if ( fn[3] == 0 ) return BSONObj::GT;
                    else if ( fn[3] == 'e' && fn[4] == 0 ) return BSONObj::GTE;
                }
                else if ( fn[1] == 'l' ) {
                    if ( fn[3] == 0 ) return BSONObj::LT;
                    else if ( fn[3] == 'e' && fn[4] == 0 ) return BSONObj::LTE;
                }
            }
            else if ( fn[1] == 'n' && fn[2] == 'e' ) {
                if ( fn[3] == 0 )
                    return BSONObj::NE;
                if ( fn[3] == 'a' && fn[4] == 'r') // matches anything with $near prefix
                    return BSONObj::opNEAR;
            }
            else if ( fn[1] == 'm' ) {
                if ( fn[2] == 'o' && fn[3] == 'd' && fn[4] == 0 )
                    return BSONObj::opMOD;
                if ( fn[2] == 'a' && fn[3] == 'x' && fn[4] == 'D' && fn[5] == 'i' && fn[6] == 's' && fn[7] == 't' && fn[8] == 'a' && fn[9] == 'n' && fn[10] == 'c' && fn[11] == 'e' && fn[12] == 0 )
                    return BSONObj::opMAX_DISTANCE;
            }
            else if ( fn[1] == 't' && fn[2] == 'y' && fn[3] == 'p' && fn[4] == 'e' && fn[5] == 0 )
                return BSONObj::opTYPE;
            else if ( fn[1] == 'i' && fn[2] == 'n' && fn[3] == 0) {
                return BSONObj::opIN;
            } else if ( fn[1] == 'n' && fn[2] == 'i' && fn[3] == 'n' && fn[4] == 0 )
                return BSONObj::NIN;
            else if ( fn[1] == 'a' && fn[2] == 'l' && fn[3] == 'l' && fn[4] == 0 )
                return BSONObj::opALL;
            else if ( fn[1] == 's' && fn[2] == 'i' && fn[3] == 'z' && fn[4] == 'e' && fn[5] == 0 )
                return BSONObj::opSIZE;
            else if ( fn[1] == 'e' ) {
                if ( fn[2] == 'x' && fn[3] == 'i' && fn[4] == 's' && fn[5] == 't' && fn[6] == 's' && fn[7] == 0 )
                    return BSONObj::opEXISTS;
                if ( fn[2] == 'l' && fn[3] == 'e' && fn[4] == 'm' && fn[5] == 'M' && fn[6] == 'a' && fn[7] == 't' && fn[8] == 'c' && fn[9] == 'h' && fn[10] == 0 )
                    return BSONObj::opELEM_MATCH;
            }
            else if ( fn[1] == 'r' && fn[2] == 'e' && fn[3] == 'g' && fn[4] == 'e' && fn[5] == 'x' && fn[6] == 0 )
                return BSONObj::opREGEX;
            else if ( fn[1] == 'o' && fn[2] == 'p' && fn[3] == 't' && fn[4] == 'i' && fn[5] == 'o' && fn[6] == 'n' && fn[7] == 's' && fn[8] == 0 )
                return BSONObj::opOPTIONS;
            else if ( fn[1] == 'w' && fn[2] == 'i' && fn[3] == 't' && fn[4] == 'h' && fn[5] == 'i' && fn[6] == 'n' && fn[7] == 0 )
                return BSONObj::opWITHIN;
            else if (str::equals(fn + 1, "geoIntersects"))
                return BSONObj::opGEO_INTERSECTS;
            else if (str::equals(fn + 1, "geoNear"))
                return BSONObj::opNEAR;
            else if (str::equals(fn + 1, "geoWithin"))
                return BSONObj::opWITHIN;
        }
        return def;
    }

    /** transform a BSON array into a vector of BSONElements.
        we match array # positions with their vector position, and ignore
        any fields with non-numeric field names.
        */
    std::vector<BSONElement> BSONElement::Array() const {
        chk(mongo::Array);
        std::vector<BSONElement> v;
        BSONObjIterator i(Obj());
        while( i.more() ) {
            BSONElement e = i.next();
            const char *f = e.fieldName();

            unsigned u;
            Status status = parseNumberFromString( f, &u );
            if ( status.isOK() ) {
                verify( u < 1000000 );
                if( u >= v.size() )
                    v.resize(u+1);
                v[u] = e;
            }
            else {
                // ignore?
            }
        }
        return v;
    }



    /* Matcher --------------------------------------*/

// If the element is something like:
//   a : { $gt : 3 }
// we append
//   a : 3
// else we just append the element.
//
    void appendElementHandlingGtLt(BSONObjBuilder& b, const BSONElement& e) {
        if ( e.type() == Object ) {
            BSONElement fe = e.embeddedObject().firstElement();
            const char *fn = fe.fieldName();
            if ( fn[0] == '$' && fn[1] && fn[2] == 't' ) {
                b.appendAs(fe, e.fieldName());
                return;
            }
        }
        b.append(e);
    }

    int getGtLtOp(const BSONElement& e) {
        if ( e.type() != Object )
            return BSONObj::Equality;

        BSONElement fe = e.embeddedObject().firstElement();
        return fe.getGtLtOp();
    }

    FieldCompareResult compareDottedFieldNames( const string& l , const string& r ,
                                               const LexNumCmp& cmp ) {
        static int maxLoops = 1024 * 1024;

        size_t lstart = 0;
        size_t rstart = 0;

        for ( int i=0; i<maxLoops; i++ ) {

            size_t a = l.find( '.' , lstart );
            size_t b = r.find( '.' , rstart );

            size_t lend = a == string::npos ? l.size() : a;
            size_t rend = b == string::npos ? r.size() : b;

            const string& c = l.substr( lstart , lend - lstart );
            const string& d = r.substr( rstart , rend - rstart );

            int x = cmp.cmp( c.c_str(), d.c_str() );

            if ( x < 0 )
                return LEFT_BEFORE;
            if ( x > 0 )
                return RIGHT_BEFORE;

            lstart = lend + 1;
            rstart = rend + 1;

            if ( lstart >= l.size() ) {
                if ( rstart >= r.size() )
                    return SAME;
                return RIGHT_SUBFIELD;
            }
            if ( rstart >= r.size() )
                return LEFT_SUBFIELD;
        }

        log() << "compareDottedFieldNames ERROR  l: " << l << " r: " << r << "  TOO MANY LOOPS" << endl;
        verify(0);
        return SAME; // will never get here
    }

    /* BSONObj ------------------------------------------------------------*/

    string BSONObj::md5() const {
        md5digest d;
        md5_state_t st;
        md5_init(&st);
        md5_append( &st , (const md5_byte_t*)_objdata , objsize() );
        md5_finish(&st, d);
        return digestToString( d );
    }

    string BSONObj::jsonString( JsonStringFormat format, int pretty ) const {

        if ( isEmpty() ) return "{}";

        StringBuilder s;
        s << "{ ";
        BSONObjIterator i(*this);
        BSONElement e = i.next();
        if ( !e.eoo() )
            while ( 1 ) {
                s << e.jsonString( format, true, pretty?pretty+1:0 );
                e = i.next();
                if ( e.eoo() )
                    break;
                s << ",";
                if ( pretty ) {
                    s << '\n';
                    for( int x = 0; x < pretty; x++ )
                        s << "  ";
                }
                else {
                    s << " ";
                }
            }
        s << " }";
        return s.str();
    }

    bool BSONObj::valid() const {
        return validateBSON( objdata(), objsize() ).isOK();
    }

    int BSONObj::woCompare(const BSONObj& r, const Ordering &o, bool considerFieldName) const {
        if ( isEmpty() )
            return r.isEmpty() ? 0 : -1;
        if ( r.isEmpty() )
            return 1;

        BSONObjIterator i(*this);
        BSONObjIterator j(r);
        unsigned mask = 1;
        while ( 1 ) {
            // so far, equal...

            BSONElement l = i.next();
            BSONElement r = j.next();
            if ( l.eoo() )
                return r.eoo() ? 0 : -1;
            if ( r.eoo() )
                return 1;

            int x;
            {
                x = l.woCompare( r, considerFieldName );
                if( o.descending(mask) )
                    x = -x;
            }
            if ( x != 0 )
                return x;
            mask <<= 1;
        }
        return -1;
    }

    /* well ordered compare */
    int BSONObj::woCompare(const BSONObj &r, const BSONObj &idxKey,
                           bool considerFieldName) const {
        if ( isEmpty() )
            return r.isEmpty() ? 0 : -1;
        if ( r.isEmpty() )
            return 1;

        bool ordered = !idxKey.isEmpty();

        BSONObjIterator i(*this);
        BSONObjIterator j(r);
        BSONObjIterator k(idxKey);
        while ( 1 ) {
            // so far, equal...

            BSONElement l = i.next();
            BSONElement r = j.next();
            BSONElement o;
            if ( ordered )
                o = k.next();
            if ( l.eoo() )
                return r.eoo() ? 0 : -1;
            if ( r.eoo() )
                return 1;

            int x;
            /*
                        if( ordered && o.type() == String && strcmp(o.valuestr(), "ascii-proto") == 0 &&
                            l.type() == String && r.type() == String ) {
                            // note: no negative support yet, as this is just sort of a POC
                            x = _stricmp(l.valuestr(), r.valuestr());
                        }
                        else*/ {
                x = l.woCompare( r, considerFieldName );
                if ( ordered && o.number() < 0 )
                    x = -x;
            }
            if ( x != 0 )
                return x;
        }
        return -1;
    }

    BSONObj staticNull = fromjson( "{'':null}" );
    BSONObj makeUndefined() {
        BSONObjBuilder b;
        b.appendUndefined( "" );
        return b.obj();
    }
    BSONObj staticUndefined = makeUndefined();

    /* well ordered compare */
    int BSONObj::woSortOrder(const BSONObj& other, const BSONObj& sortKey , bool useDotted ) const {
        if ( isEmpty() )
            return other.isEmpty() ? 0 : -1;
        if ( other.isEmpty() )
            return 1;

        uassert( 10060 ,  "woSortOrder needs a non-empty sortKey" , ! sortKey.isEmpty() );

        BSONObjIterator i(sortKey);
        while ( 1 ) {
            BSONElement f = i.next();
            if ( f.eoo() )
                return 0;

            BSONElement l = useDotted ? getFieldDotted( f.fieldName() ) : getField( f.fieldName() );
            if ( l.eoo() )
                l = staticNull.firstElement();
            BSONElement r = useDotted ? other.getFieldDotted( f.fieldName() ) : other.getField( f.fieldName() );
            if ( r.eoo() )
                r = staticNull.firstElement();

            int x = l.woCompare( r, false );
            if ( f.number() < 0 )
                x = -x;
            if ( x != 0 )
                return x;
        }
        return -1;
    }

    bool BSONObj::isPrefixOf( const BSONObj& otherObj ) const {
        BSONObjIterator a( *this );
        BSONObjIterator b( otherObj );

        while ( a.more() && b.more() ) {
            BSONElement x = a.next();
            BSONElement y = b.next();
            if ( x != y )
                return false;
        }

        return ! a.more();
    }

    bool BSONObj::isFieldNamePrefixOf( const BSONObj& otherObj ) const {
        BSONObjIterator a( *this );
        BSONObjIterator b( otherObj );

        while ( a.more() && b.more() ) {
            BSONElement x = a.next();
            BSONElement y = b.next();
            if ( ! str::equals( x.fieldName() , y.fieldName() ) ) {
                return false;
            }
        }

        return ! a.more();
    }

    template <typename BSONElementColl>
    void _getFieldsDotted( const BSONObj* obj, const StringData& name, BSONElementColl &ret, bool expandLastArray ) {
        BSONElement e = obj->getField( name );

        if ( e.eoo() ) {
            size_t idx = name.find( '.' );
            if ( idx != string::npos ) {
                StringData left = name.substr( 0, idx );
                StringData next = name.substr( idx + 1, name.size() );

                BSONElement e = obj->getField( left );

                if (e.type() == Object) {
                    e.embeddedObject().getFieldsDotted(next, ret, expandLastArray );
                }
                else if (e.type() == Array) {
                    bool allDigits = false;
                    if ( next.size() > 0 && isdigit( next[0] ) ) {
                        unsigned temp = 1;
                        while ( temp < next.size() && isdigit( next[temp] ) )
                            temp++;
                        allDigits = temp == next.size() || next[temp] == '.';
                    }
                    if (allDigits) {
                        e.embeddedObject().getFieldsDotted(next, ret, expandLastArray );
                    }
                    else {
                        BSONObjIterator i(e.embeddedObject());
                        while ( i.more() ) {
                            BSONElement e2 = i.next();
                            if (e2.type() == Object || e2.type() == Array)
                                e2.embeddedObject().getFieldsDotted(next, ret, expandLastArray );
                        }
                    }
                }
                else {
                    // do nothing: no match
                }
            }
        }
        else {
            if (e.type() == Array && expandLastArray) {
                BSONObjIterator i(e.embeddedObject());
                while ( i.more() )
                    ret.insert(i.next());
            }
            else {
                ret.insert(e);
            }
        }
    }

    void BSONObj::getFieldsDotted(const StringData& name, BSONElementSet &ret, bool expandLastArray ) const {
        _getFieldsDotted( this, name, ret, expandLastArray );
    }
    void BSONObj::getFieldsDotted(const StringData& name, BSONElementMSet &ret, bool expandLastArray ) const {
        _getFieldsDotted( this, name, ret, expandLastArray );
    }

    BSONElement BSONObj::getFieldDottedOrArray(const char *&name) const {
        const char *p = strchr(name, '.');

        BSONElement sub;

        if ( p ) {
            sub = getField( string(name, p-name) );
            name = p + 1;
        }
        else {
            sub = getField( name );
            name = name + strlen(name);
        }

        if ( sub.eoo() )
            return eooElement;
        else if ( sub.type() == Array || name[0] == '\0' )
            return sub;
        else if ( sub.type() == Object )
            return sub.embeddedObject().getFieldDottedOrArray( name );
        else
            return eooElement;
    }

    BSONObj BSONObj::extractFieldsUnDotted(const BSONObj& pattern) const {
        BSONObjBuilder b;
        BSONObjIterator i(pattern);
        while ( i.moreWithEOO() ) {
            BSONElement e = i.next();
            if ( e.eoo() )
                break;
            BSONElement x = getField(e.fieldName());
            if ( !x.eoo() )
                b.appendAs(x, "");
        }
        return b.obj();
    }

    BSONObj BSONObj::extractFields(const BSONObj& pattern , bool fillWithNull ) const {
        BSONObjBuilder b(32); // scanandorder.h can make a zillion of these, so we start the allocation very small
        BSONObjIterator i(pattern);
        while ( i.moreWithEOO() ) {
            BSONElement e = i.next();
            if ( e.eoo() )
                break;
            BSONElement x = getFieldDotted(e.fieldName());
            if ( ! x.eoo() )
                b.appendAs( x, e.fieldName() );
            else if ( fillWithNull )
                b.appendNull( e.fieldName() );
        }
        return b.obj();
    }

    BSONObj BSONObj::filterFieldsUndotted( const BSONObj &filter, bool inFilter ) const {
        BSONObjBuilder b;
        BSONObjIterator i( *this );
        while( i.moreWithEOO() ) {
            BSONElement e = i.next();
            if ( e.eoo() )
                break;
            BSONElement x = filter.getField( e.fieldName() );
            if ( ( x.eoo() && !inFilter ) ||
                    ( !x.eoo() && inFilter ) )
                b.append( e );
        }
        return b.obj();
    }

    BSONElement BSONObj::getFieldUsingIndexNames(const StringData& fieldName,
                                                 const BSONObj &indexKey) const {
        BSONObjIterator i( indexKey );
        int j = 0;
        while( i.moreWithEOO() ) {
            BSONElement f = i.next();
            if ( f.eoo() )
                return BSONElement();
            if ( f.fieldName() == fieldName )
                break;
            ++j;
        }
        BSONObjIterator k( *this );
        while( k.moreWithEOO() ) {
            BSONElement g = k.next();
            if ( g.eoo() )
                return BSONElement();
            if ( j == 0 ) {
                return g;
            }
            --j;
        }
        return BSONElement();
    }

    /* grab names of all the fields in this object */
    int BSONObj::getFieldNames(set<string>& fields) const {
        int n = 0;
        BSONObjIterator i(*this);
        while ( i.moreWithEOO() ) {
            BSONElement e = i.next();
            if ( e.eoo() )
                break;
            fields.insert(e.fieldName());
            n++;
        }
        return n;
    }

    /* note: addFields always adds _id even if not specified
       returns n added not counting _id unless requested.
    */
    int BSONObj::addFields(BSONObj& from, set<string>& fields) {
        verify( isEmpty() && !isOwned() ); /* partial implementation for now... */

        BSONObjBuilder b;

        int N = fields.size();
        int n = 0;
        BSONObjIterator i(from);
        bool gotId = false;
        while ( i.moreWithEOO() ) {
            BSONElement e = i.next();
            const char *fname = e.fieldName();
            if ( fields.count(fname) ) {
                b.append(e);
                ++n;
                gotId = gotId || strcmp(fname, "_id")==0;
                if ( n == N && gotId )
                    break;
            }
            else if ( strcmp(fname, "_id")==0 ) {
                b.append(e);
                gotId = true;
                if ( n == N && gotId )
                    break;
            }
        }

        if ( n ) {
            *this = b.obj();
        }

        return n;
    }

    bool BSONObj::couldBeArray() const {
        BSONObjIterator i( *this );
        int index = 0;
        while( i.moreWithEOO() ){
            BSONElement e = i.next();
            if( e.eoo() ) break;

            // TODO:  If actually important, may be able to do int->char* much faster
            if( strcmp( e.fieldName(), ((string)( str::stream() << index )).c_str() ) != 0 )
                return false;
            index++;
        }
        return true;
    }

    BSONObj BSONObj::clientReadable() const {
        BSONObjBuilder b;
        BSONObjIterator i( *this );
        while( i.moreWithEOO() ) {
            BSONElement e = i.next();
            if ( e.eoo() )
                break;
            switch( e.type() ) {
            case MinKey: {
                BSONObjBuilder m;
                m.append( "$minElement", 1 );
                b.append( e.fieldName(), m.done() );
                break;
            }
            case MaxKey: {
                BSONObjBuilder m;
                m.append( "$maxElement", 1 );
                b.append( e.fieldName(), m.done() );
                break;
            }
            default:
                b.append( e );
            }
        }
        return b.obj();
    }

    BSONObj BSONObj::replaceFieldNames( const BSONObj &names ) const {
        BSONObjBuilder b;
        BSONObjIterator i( *this );
        BSONObjIterator j( names );
        BSONElement f = j.moreWithEOO() ? j.next() : BSONObj().firstElement();
        while( i.moreWithEOO() ) {
            BSONElement e = i.next();
            if ( e.eoo() )
                break;
            if ( !f.eoo() ) {
                b.appendAs( e, f.fieldName() );
                f = j.next();
            }
            else {
                b.append( e );
            }
        }
        return b.obj();
    }

    Status BSONObj::_okForStorage(bool root, bool deep) const {
        BSONObjIterator i( *this );

        // The first field is special in the case of a DBRef where the first field must be $ref
        bool first = true;
        while ( i.more() ) {
            BSONElement e = i.next();
            const char* name = e.fieldName();

            // Cannot start with "$", unless dbref which must start with ($ref, $id)
            if (str::startsWith(name, '$')) {
                if ( first &&
                     // $ref is a collection name and must be a String
                     str::equals(name, "$ref") && e.type() == String &&
                     str::equals(i.next().fieldName(), "$id") ) {

                    first = false;
                    // keep inspecting fields for optional "$db"
                    e = i.next();
                    name = e.fieldName(); // "" if eoo()

                    // optional $db field must be a String
                    if (str::equals(name, "$db") && e.type() == String) {
                        continue; //this element is fine, so continue on to siblings (if any more)
                    }

                    // Can't start with a "$", all other checks are done below (outside if blocks)
                    if (str::startsWith(name, '$'))  {
                        return Status(ErrorCodes::DollarPrefixedFieldName,
                                      str::stream() << name << " is not valid for storage.");
                    }
                }
                else {
                    // not an okay, $ prefixed field name.
                    return Status(ErrorCodes::DollarPrefixedFieldName,
                                  str::stream() << name << " is not valid for storage.");
                }
            }

            // Do not allow "." in the field name
            if (strchr(name, '.')) {
                return Status(ErrorCodes::DottedFieldName,
                              str::stream() << name << " is not valid for storage.");
            }

            // (SERVER-9502) Do not allow storing an _id field with a RegEx type or
            // Array type in a root document
            if (root && (e.type() == RegEx || e.type() == Array || e.type() == Undefined)
                     && str::equals(name,"_id")) {
                return Status(ErrorCodes::InvalidIdField,
                              str::stream() << name
                                            << " is not valid for storage because it is of type "
                                            << typeName(e.type()));
            }

            if ( deep && e.mayEncapsulate() ) {
                switch ( e.type() ) {
                case Object:
                case Array:
                    {
                        Status s = e.embeddedObject()._okForStorage(false, true);
                        // TODO: combine field names for better error messages
                        if ( ! s.isOK() )
                            return s;
                    }
                    break;
                case CodeWScope:
                    {
                        Status s = e.codeWScopeObject()._okForStorage(false, true);
                        // TODO: combine field names for better error messages
                        if ( ! s.isOK() )
                            return s;
                    }
                    break;
                default:
                    uassert( 12579, "unhandled cases in BSONObj okForStorage" , 0 );
                }
            }

            // After we have processed one field, we are no longer on the first field
            first = false;
        }
        return Status::OK();
    }

    void BSONObj::dump() const {
        LogstreamBuilder builder = out();
        builder << hex;
        const char *p = objdata();
        for ( int i = 0; i < objsize(); i++ ) {
            builder << i << '\t' << ( 0xff & ( (unsigned) *p ) );
            if ( *p >= 'A' && *p <= 'z' )
                builder << '\t' << *p;
            builder << endl;
            p++;
        }
    }

    void nested2dotted(BSONObjBuilder& b, const BSONObj& obj, const string& base) {
        BSONObjIterator it(obj);
        while (it.more()) {
            BSONElement e = it.next();
            if (e.type() == Object) {
                string newbase = base + e.fieldName() + ".";
                nested2dotted(b, e.embeddedObject(), newbase);
            }
            else {
                string newbase = base + e.fieldName();
                b.appendAs(e, newbase);
            }
        }
    }

    void dotted2nested(BSONObjBuilder& b, const BSONObj& obj) {
        //use map to sort fields
        BSONMap sorted = bson2map(obj);
        EmbeddedBuilder eb(&b);
        for(BSONMap::const_iterator it=sorted.begin(); it!=sorted.end(); ++it) {
            eb.appendAs(it->second, it->first);
        }
        eb.done();
    }

    /*-- test things ----------------------------------------------------*/

#pragma pack(1)
    struct MaxKeyData {
        MaxKeyData() {
            totsize=7;
            maxkey=MaxKey;
            name=0;
            eoo=EOO;
        }
        int totsize;
        char maxkey;
        char name;
        char eoo;
    } maxkeydata;
    BSONObj maxKey((const char *) &maxkeydata);

    struct MinKeyData {
        MinKeyData() {
            totsize=7;
            minkey=MinKey;
            name=0;
            eoo=EOO;
        }
        int totsize;
        char minkey;
        char name;
        char eoo;
    } minkeydata;
    BSONObj minKey((const char *) &minkeydata);

    /*
        struct JSObj0 {
            JSObj0() {
                totsize = 5;
                eoo = EOO;
            }
            int totsize;
            char eoo;
        } js0;
    */
#pragma pack()

    Labeler::Label GT( "$gt" );
    Labeler::Label GTE( "$gte" );
    Labeler::Label LT( "$lt" );
    Labeler::Label LTE( "$lte" );
    Labeler::Label NE( "$ne" );
    Labeler::Label NIN( "$nin" );
    Labeler::Label BSIZE( "$size" );

    void BSONObjBuilder::appendMinForType( const StringData& fieldName , int t ) {
        switch ( t ) {
                
        // Shared canonical types
        case NumberInt:
        case NumberDouble:
        case NumberLong:
            append( fieldName , - numeric_limits<double>::max() ); return;
        case Symbol:
        case String:
            append( fieldName , "" ); return;
        case Date: 
            // min varies with V0 and V1 indexes, so we go one type lower.
            appendBool(fieldName, true);
            //appendDate( fieldName , numeric_limits<long long>::min() ); 
            return;
        case Timestamp: // TODO integrate with Date SERVER-3304
            appendTimestamp( fieldName , 0 ); return;
        case Undefined: // shared with EOO
            appendUndefined( fieldName ); return;
                
        // Separate canonical types
        case MinKey:
            appendMinKey( fieldName ); return;
        case MaxKey:
            appendMaxKey( fieldName ); return;
        case jstOID: {
            OID o;
            memset(&o, 0, sizeof(o));
            appendOID( fieldName , &o);
            return;
        }
        case Bool:
            appendBool( fieldName , false); return;
        case jstNULL:
            appendNull( fieldName ); return;
        case Object:
            append( fieldName , BSONObj() ); return;
        case Array:
            appendArray( fieldName , BSONObj() ); return;
        case BinData:
            appendBinData( fieldName , 0 , BinDataGeneral , (const char *) 0 ); return;
        case RegEx:
            appendRegex( fieldName , "" ); return;
        case DBRef: {
            OID o;
            memset(&o, 0, sizeof(o));
            appendDBRef( fieldName , "" , o );
            return;
        }
        case Code:
            appendCode( fieldName , "" ); return;
        case CodeWScope:
            appendCodeWScope( fieldName , "" , BSONObj() ); return;
        };
        log() << "type not supported for appendMinElementForType: " << t << endl;
        uassert( 10061 ,  "type not supported for appendMinElementForType" , false );
    }

    void BSONObjBuilder::appendMaxForType( const StringData& fieldName , int t ) {
        switch ( t ) {
                
        // Shared canonical types
        case NumberInt:
        case NumberDouble:
        case NumberLong:
            append( fieldName , numeric_limits<double>::max() ); return;
        case Symbol:
        case String:
            appendMinForType( fieldName, Object ); return;
        case Date:
            appendDate( fieldName , numeric_limits<long long>::max() ); return;
        case Timestamp: // TODO integrate with Date SERVER-3304
            append( fieldName , OpTime::max() ); return;
        case Undefined: // shared with EOO
            appendUndefined( fieldName ); return;

        // Separate canonical types
        case MinKey:
            appendMinKey( fieldName ); return;
        case MaxKey:
            appendMaxKey( fieldName ); return;
        case jstOID: {
            OID o;
            memset(&o, 0xFF, sizeof(o));
            appendOID( fieldName , &o);
            return;
        }
        case Bool:
            appendBool( fieldName , true ); return;
        case jstNULL:
            appendNull( fieldName ); return;
        case Object:
            appendMinForType( fieldName, Array ); return;
        case Array:
            appendMinForType( fieldName, BinData ); return;
        case BinData:
            appendMinForType( fieldName, jstOID ); return;
        case RegEx:
            appendMinForType( fieldName, DBRef ); return;
        case DBRef:
            appendMinForType( fieldName, Code ); return;                
        case Code:
            appendMinForType( fieldName, CodeWScope ); return;
        case CodeWScope:
            // This upper bound may change if a new bson type is added.
            appendMinForType( fieldName , MaxKey ); return;
        }
        log() << "type not supported for appendMaxElementForType: " << t << endl;
        uassert( 14853 ,  "type not supported for appendMaxElementForType" , false );
    }

    bool fieldsMatch(const BSONObj& lhs, const BSONObj& rhs) {
        BSONObjIterator l(lhs);
        BSONObjIterator r(rhs);

        while (l.more() && r.more()){
            if (strcmp(l.next().fieldName(), r.next().fieldName())) {
                return false;
            }
        }

        return !(l.more() || r.more()); // false if lhs and rhs have diff nFields()
    }

    /** Compare two bson elements, provided as const char *'s, by field name. */
    class BSONIteratorSorted::ElementFieldCmp {
    public:
        ElementFieldCmp( bool isArray );
        bool operator()( const char *s1, const char *s2 ) const;
    private:
        LexNumCmp _cmp;
    };
    
    BSONIteratorSorted::ElementFieldCmp::ElementFieldCmp( bool isArray ) :
    _cmp( !isArray ) {
    }

    bool BSONIteratorSorted::ElementFieldCmp::operator()( const char *s1, const char *s2 )
    const {
        // Skip the type byte and compare field names.
        return _cmp( s1 + 1, s2 + 1 );
    }        
    
    BSONIteratorSorted::BSONIteratorSorted( const BSONObj &o, const ElementFieldCmp &cmp ) {
        _nfields = o.nFields();
        _fields = new const char*[_nfields];
        int x = 0;
        BSONObjIterator i( o );
        while ( i.more() ) {
            _fields[x++] = i.next().rawdata();
            verify( _fields[x-1] );
        }
        verify( x == _nfields );
        std::sort( _fields , _fields + _nfields , cmp );
        _cur = 0;
    }
    
    BSONObjIteratorSorted::BSONObjIteratorSorted( const BSONObj &object ) :
    BSONIteratorSorted( object, ElementFieldCmp( false ) ) {
    }

    BSONArrayIteratorSorted::BSONArrayIteratorSorted( const BSONArray &array ) :
    BSONIteratorSorted( array, ElementFieldCmp( true ) ) {
    }

    bool BSONObjBuilder::appendAsNumber( const StringData& fieldName , const string& data ) {
        if ( data.size() == 0 || data == "-" || data == ".")
            return false;

        unsigned int pos=0;
        if ( data[0] == '-' )
            pos++;

        bool hasDec = false;

        for ( ; pos<data.size(); pos++ ) {
            if ( isdigit(data[pos]) )
                continue;

            if ( data[pos] == '.' ) {
                if ( hasDec )
                    return false;
                hasDec = true;
                continue;
            }

            return false;
        }

        if ( hasDec ) {
            double d = atof( data.c_str() );
            append( fieldName , d );
            return true;
        }

        if ( data.size() < 8 ) {
            append( fieldName , atoi( data.c_str() ) );
            return true;
        }

        try {
            long long num = boost::lexical_cast<long long>( data );
            append( fieldName , num );
            return true;
        }
        catch(boost::bad_lexical_cast &) {
            return false;
        }
    }

    /* take a BSONType and return the name of that type as a char* */
    const char* typeName (BSONType type) {
        switch (type) {
            case MinKey: return "MinKey";
            case EOO: return "EOO";
            case NumberDouble: return "NumberDouble";
            case String: return "String";
            case Object: return "Object";
            case Array: return "Array";
            case BinData: return "BinaryData";
            case Undefined: return "Undefined";
            case jstOID: return "OID";
            case Bool: return "Bool";
            case Date: return "Date";
            case jstNULL: return "NULL";
            case RegEx: return "RegEx";
            case DBRef: return "DBRef";
            case Code: return "Code";
            case Symbol: return "Symbol";
            case CodeWScope: return "CodeWScope";
            case NumberInt: return "NumberInt32";
            case Timestamp: return "Timestamp";
            case NumberLong: return "NumberLong64";
            // JSTypeMax doesn't make sense to turn into a string; overlaps with highest-valued type
            case MaxKey: return "MaxKey";
            default: return "Invalid";
        }
    }

} // namespace mongo
