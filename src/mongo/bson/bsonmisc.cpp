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

#include "mongo/db/jsobj.h"

namespace mongo {

    int getGtLtOp(const BSONElement& e) {
        if ( e.type() != Object )
            return BSONObj::Equality;

        BSONElement fe = e.embeddedObject().firstElement();
        return fe.getGtLtOp();
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

    Labeler::Label GT( "$gt" );
    Labeler::Label GTE( "$gte" );
    Labeler::Label LT( "$lt" );
    Labeler::Label LTE( "$lte" );
    Labeler::Label NE( "$ne" );
    Labeler::Label NIN( "$nin" );
    Labeler::Label BSIZE( "$size" );

    GENOIDLabeler GENOID;
    DateNowLabeler DATENOW;
    NullLabeler BSONNULL;
    UndefinedLabeler BSONUndefined;

    MinKeyLabeler MINKEY;
    MaxKeyLabeler MAXKEY;

    BSONObjBuilderValueStream::BSONObjBuilderValueStream( BSONObjBuilder * builder ) {
        _builder = builder;
    }

    BSONObjBuilder& BSONObjBuilderValueStream::operator<<( const BSONElement& e ) {
        _builder->appendAs( e , _fieldName );
        _fieldName = StringData();
        return *_builder;
    }

    BufBuilder& BSONObjBuilderValueStream::subobjStart() {
        StringData tmp = _fieldName;
        _fieldName = StringData();
        return _builder->subobjStart(tmp);
    }

    BufBuilder& BSONObjBuilderValueStream::subarrayStart() {
        StringData tmp = _fieldName;
        _fieldName = StringData();
        return _builder->subarrayStart(tmp);
    }

    Labeler BSONObjBuilderValueStream::operator<<( const Labeler::Label &l ) {
        return Labeler( l, this );
    }

    void BSONObjBuilderValueStream::endField( const StringData& nextFieldName ) {
        if ( haveSubobj() ) {
            verify( _fieldName.rawData() );
            _builder->append( _fieldName, subobj()->done() );
            _subobj.reset();
        }
        _fieldName = nextFieldName;
    }

    BSONObjBuilder *BSONObjBuilderValueStream::subobj() {
        if ( !haveSubobj() )
            _subobj.reset( new BSONObjBuilder() );
        return _subobj.get();
    }

    BSONObjBuilder& Labeler::operator<<( const BSONElement& e ) {
        s_->subobj()->appendAs( e, l_.l_ );
        return *s_->_builder;
    }

} // namespace mongo
