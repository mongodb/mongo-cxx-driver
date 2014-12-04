// namespacestring-inl.h


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

#include "mongo/util/debug_util.h"

namespace mongo {

    inline StringData NamespaceString::db() const {
        return _dotIndex == std::string::npos ?
            StringData() :
            StringData( _ns.c_str(), _dotIndex );
    }

    inline StringData NamespaceString::coll() const {
        return _dotIndex == std::string::npos ?
            StringData() :
            StringData( _ns.c_str() + _dotIndex + 1, _ns.size() - 1 - _dotIndex );
    }

    inline bool NamespaceString::normal(const StringData& ns) {
        if ( ns.find( '$' ) == std::string::npos )
            return true;
        return oplog(ns);
    }

    inline bool NamespaceString::oplog(const StringData& ns) {
        return ns.startsWith("local.oplog.");
    }

    inline bool NamespaceString::special(const StringData& ns) {
        return !normal(ns) || ns.substr(ns.find('.')).startsWith(".system.");
    }

    inline bool NamespaceString::validDBName( const StringData& db ) {
        if ( db.size() == 0 || db.size() > 64 )
            return false;

        for (StringData::const_iterator iter = db.begin(), end = db.end(); iter != end; ++iter) {
            switch (*iter) {
            case '\0':
            case '/':
            case '\\':
            case '.':
            case ' ':
            case '"':
                return false;
#ifdef _WIN32
            // We prohibit all FAT32-disallowed characters on Windows
            case '*':
            case '<':
            case '>':
            case ':':
            case '|':
            case '?':
                return false;
#endif
            default:
                continue;
            }
        }
        return true;
    }

    inline bool NamespaceString::validCollectionComponent(const StringData& ns){
        size_t idx = ns.find( '.' );
        if ( idx == std::string::npos )
            return false;

        return validCollectionName(ns.substr(idx + 1)) || oplog(ns);
    }

    inline bool NamespaceString::validCollectionName(const StringData& coll){
        if (coll.empty())
            return false;

        if (coll[0] == '.')
            return false;

        for (StringData::const_iterator iter = coll.begin(), end = coll.end();
                iter != end; ++iter) {
            switch (*iter) {
            case '\0':
            case '$':
                return false;
            default:
                continue;
            }
        }

        return true;
    }

    inline NamespaceString::NamespaceString() : _ns(), _dotIndex(0) {}
    inline NamespaceString::NamespaceString( const StringData& nsIn ) {
        _ns = nsIn.toString(); // copy to our buffer
        _dotIndex = _ns.find( '.' );
    }

    inline NamespaceString::NamespaceString( const StringData& dbName,
                                             const StringData& collectionName )
        : _ns(dbName.size() + collectionName.size() + 1, '\0') {

        uassert(17235,
                "'.' is an invalid character in a database name",
                dbName.find('.') == std::string::npos);
        uassert(17246,
                "Collection names cannot start with '.'",
                collectionName.empty() || collectionName[0] != '.');
        std::string::iterator it = std::copy(dbName.begin(), dbName.end(), _ns.begin());
        *it = '.';
        ++it;
        it = std::copy(collectionName.begin(), collectionName.end(), it);
        _dotIndex = dbName.size();
        dassert(it == _ns.end());
        dassert(_ns[_dotIndex] == '.');
        uassert(17295, "namespaces cannot have embedded null characters",
                   _ns.find('\0') == std::string::npos);
    }

    inline int nsDBHash( const std::string& ns ) {
        int hash = 7;
        for ( size_t i = 0; i < ns.size(); i++ ) {
            if ( ns[i] == '.' )
                break;
            hash += 11 * ( ns[i] );
            hash *= 3;
        }
        return hash;
    }

    inline bool nsDBEquals( const std::string& a, const std::string& b ) {
        for ( size_t i = 0; i < a.size(); i++ ) {

            if ( a[i] == '.' ) {
                // b has to either be done or a '.'

                if ( b.size() == i )
                    return true;

                if ( b[i] == '.' )
                    return true;

                return false;
            }

            // a is another character
            if ( b.size() == i )
                return false;

            if ( b[i] != a[i] )
                    return false;
        }

        // a is done
        // make sure b is done
        if ( b.size() == a.size() ||
             b[a.size()] == '.' )
            return true;

        return false;
    }

    /* future : this doesn't need to be an inline. */
    inline std::string NamespaceString::getSisterNS( const StringData& local ) const {
        verify( local.size() && local[0] != '.' );
        return db().toString() + "." + local.toString();
    }

    inline std::string NamespaceString::getSystemIndexesCollection() const {
        return db().toString() + ".system.indexes";
    }

    inline std::string NamespaceString::getCommandNS() const {
        return db().toString() + ".$cmd";
    }

}
