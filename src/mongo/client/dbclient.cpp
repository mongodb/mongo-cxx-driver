// dbclient.cpp - connect to a Mongo database as a database, from C++

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

#define MONGO_LOG_DEFAULT_COMPONENT ::mongo::logger::LogComponent::kNetwork

#include "mongo/platform/basic.h"

#include "mongo/base/init.h"
#include "mongo/bson/util/bson_extract.h"
#include "mongo/bson/util/builder.h"
#include "mongo/client/constants.h"
#include "mongo/client/command_writer.h"
#include "mongo/client/dbclient_rs.h"
#include "mongo/client/dbclientcursor.h"
#include "mongo/client/dbclientcursorshim.h"
#include "mongo/client/dbclientcursorshimarray.h"
#include "mongo/client/dbclientcursorshimcursorid.h"
#include "mongo/client/dbclientcursorshimtransform.h"
#include "mongo/client/dbclient_writer.h"
#include "mongo/client/insert_write_operation.h"
#include "mongo/client/options.h"
#include "mongo/client/replica_set_monitor.h"
#include "mongo/client/update_write_operation.h"
#include "mongo/client/delete_write_operation.h"
#include "mongo/client/sasl_client_authenticate.h"
#include "mongo/client/wire_protocol_writer.h"
#include "mongo/client/write_result.h"
#include "mongo/db/jsobj.h"
#include "mongo/db/json.h"
#include "mongo/db/namespace_string.h"
#include "mongo/util/assert_util.h"
#include "mongo/util/log.h"
#include "mongo/util/net/ssl_manager.h"
#include "mongo/util/password_digest.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/regex.hpp>

#include <algorithm>
#include <cstdlib>

#ifdef MONGO_SSL
#include "mongo/client/native_sasl_client_session.h"
#endif

namespace mongo {

    using std::auto_ptr;
    using std::endl;
    using std::list;
    using std::map;
    using std::string;
    using std::stringstream;
    using std::vector;

    AtomicInt64 DBClientBase::ConnectionIdSequence;

    const char* const saslCommandUserSourceFieldName = "userSource";

    const int defaultMaxBsonObjectSize = 16 * 1024 * 1024;
    const int defaultMaxMessageSizeBytes = defaultMaxBsonObjectSize * 2;
    const int defaultMaxWriteBatchSize = 1000;

    namespace {
        const char kMongoDBURL[] =
            // scheme: non-capturing
            "mongodb://"

            // credentials: two inner captures for user and password
            "(?:([^:]+)(?::([^@]+))?@)?"

            // servers: grabs all host:port or UNIX socket names
            "((?:(?:[^\\/]+|/.+.sock?),?)+)"

            // database: matches anything but the chars that cannot
            // be part of a MongoDB database name.
            "(?:/([^/\\.\\ \"*<>:\\|\\?]*))?"

            // options
            "(?:\\?(?:(.+=.+)&?)+)*";

    } // namespace

    void ConnectionString::_fillServers( string s, bool legacy ) {

        //
        // Custom-handled servers/replica sets start with '$'
        // According to RFC-1123/952, this will not overlap with valid hostnames
        // (also disallows $replicaSetName hosts)
        //

        if( s.find( '$' ) == 0 ) _type = CUSTOM;

        if (legacy) {
            string::size_type idx = s.find( '/' );
            if (idx != string::npos && idx != 0) {
                _setName = s.substr( 0 , idx );
                s = s.substr( idx + 1 );
                if( _type != CUSTOM ) _type = SET;
            }
        }

        string::size_type idx;
        while ( ( idx = s.find( ',' ) ) != string::npos ) {
            _servers.push_back(HostAndPort(s.substr(0, idx)));
            s = s.substr( idx + 1 );
        }
        _servers.push_back(HostAndPort(s));

    }

    void ConnectionString::_finishInit() {

        // Needed here as well b/c the parsing logic isn't used in all constructors
        // TODO: Refactor so that the parsing logic *is* used in all constructors
        if ( _type == MASTER && _servers.size() > 0 ){
            if( _servers[0].host().find( '$' ) == 0 ){
                _type = CUSTOM;
            }
        }

        stringstream ss;
        if ( _type == SET )
            ss << _setName << "/";
        for ( unsigned i=0; i<_servers.size(); i++ ) {
            if ( i > 0 )
                ss << ",";
            ss << _servers[i].toString();
        }
        _string = ss.str();
    }

    namespace {
        const char kAuthMechanismPropertiesKey[] = "mechanism_properties";

        // CANONICALIZE_HOST_NAME is currently unsupported
        const char kAuthServiceName[] = "SERVICE_NAME";
        const char kAuthServiceRealm[] = "SERVICE_REALM";

        const char kAuthMechMongoCR[] = "MONGODB-CR";
        const char kAuthMechScramSha1[] = "SCRAM-SHA-1";
        const char kAuthMechDefault[] = "DEFAULT";

        const char* const kSupportedAuthMechanismProperties[] = {
            kAuthServiceName,
            kAuthServiceRealm
        };

        // bootleg std::end for c-style arrays.
        template<class T, std::size_t N>
        T* endOf(T (&arr)[N]) { return arr + N; }

        BSONObj parseAuthMechanismProperties(const std::string& propStr) {
            BSONObjBuilder bob;
            std::vector<std::string> props;
            boost::algorithm::split(props, propStr, boost::algorithm::is_any_of(",:"));
            for (std::vector<std::string>::const_iterator it = props.begin();
                 it != props.end(); ++it) {
                std::string prop((boost::algorithm::to_upper_copy(*it))); // normalize case
                uassert(ErrorCodes::FailedToParse,
                         str::stream() << "authMechanismProperty: " << *it
                                       << " is not supported",
                         std::count(kSupportedAuthMechanismProperties,
                                    endOf(kSupportedAuthMechanismProperties),
                                    prop));
                ++it;
                uassert(ErrorCodes::FailedToParse,
                         str::stream() << "authMechanismProperty: "
                                       << prop << " must have a value",
                         it != props.end());
                bob.append(prop, *it);
            }
            return bob.obj();
        }

        string authKeyCopyDBMongoCR(const string& username,
                                    const string& password,
                                    const string& nonce)
        {
            md5digest d;
            string passwordDigest = createPasswordDigest( username, password );
            {
                md5_state_t st;
                md5_init(&st);
                md5_append(&st, reinterpret_cast<const md5_byte_t *>(nonce.c_str()), nonce.size() );
                md5_append(&st, reinterpret_cast<const md5_byte_t *>(username.data()), username.length());
                md5_append(&st, reinterpret_cast<const md5_byte_t *>(passwordDigest.c_str()), passwordDigest.size() );
                md5_finish(&st, d);
            }
            return digestToString( d );
        }

    }  // namespace

    BSONObj ConnectionString::_makeAuthObjFromOptions(int maxWireVersion) const {
        BSONObjBuilder bob;

        // Add the username and optional password
        invariant(!_user.empty());
        std::string username(_user);  // may have to tack on service realm before we append

        if (!_password.empty())
            bob.append(saslCommandPasswordFieldName, _password);

        BSONElement elt = _options.getField("authSource");
        if (!elt.eoo()) {
            bob.appendAs(elt, saslCommandUserDBFieldName);
        } else if (!_database.empty()) {
            bob.append(saslCommandUserDBFieldName, _database);
        } else {
            bob.append(saslCommandUserDBFieldName, "admin");
        }

        elt = _options.getField("authMechanism");
        if (!elt.eoo()) {
            bob.appendAs(elt, saslCommandMechanismFieldName);
        } else if (maxWireVersion >= 3) {
            bob.append(saslCommandMechanismFieldName, kAuthMechScramSha1);
        } else {
            bob.append(saslCommandMechanismFieldName, kAuthMechMongoCR);
        }

        elt = _options.getField("authMechanismProperties");
        if (!elt.eoo()) {
            BSONObj parsed(parseAuthMechanismProperties(elt.String()));

            bool hasNameProp = parsed.hasField(kAuthServiceName);
            bool hasRealmProp = parsed.hasField(kAuthServiceRealm);

            uassert(ErrorCodes::FailedToParse,
                    "Cannot specify both gssapiServiceName and SERVICE_NAME",
                    !(hasNameProp && _options.hasField("gssapiServiceName")));
            // we append the parsed object so that mechanisms that don't accept it can assert.
            bob.append(kAuthMechanismPropertiesKey, parsed);
            // we still append using the old way the SASL code expects it
            if (hasNameProp) {
                bob.append(saslCommandServiceNameFieldName, parsed[kAuthServiceName].String());
            }
            // if we specified a realm, we just append it to the username as the SASL code
            // expects it that way.
            if (hasRealmProp) {
                username.append("@").append(parsed[kAuthServiceRealm].String());
            }
        }

        elt = _options.getField("gssapiServiceName");
        if (!elt.eoo())
            bob.appendAs(elt, saslCommandServiceNameFieldName);

        bob.append("user", username);

        return bob.obj();
    }

    boost::mutex ConnectionString::_connectHookMutex;
    ConnectionString::ConnectionHook* ConnectionString::_connectHook = NULL;

    DBClientBase* ConnectionString::connect( string& errmsg, double socketTimeout ) const {

        switch ( _type ) {
        case MASTER: {
            auto_ptr<DBClientConnection> c(new DBClientConnection(true));
            c->setSoTimeout( socketTimeout );
            LOG(1) << "creating new connection to:" << _servers[0] << endl;
            if ( ! c->connect( _servers[0] , errmsg ) ) {
                return 0;
            }

            if (!_user.empty()) {
                c->auth(_makeAuthObjFromOptions(c->getMaxWireVersion()));
            }

            LOG(1) << "connected connection!" << endl;
            return c.release();
        }

        case PAIR:
        case SET: {
            auto_ptr<DBClientReplicaSet> set(
                new DBClientReplicaSet(_setName, _servers, socketTimeout));

            if( ! set->connect() ) {
                errmsg = "connect failed to replica set ";
                errmsg += toString();
                return 0;
            }

            if (!_user.empty()) {
                set->auth(_makeAuthObjFromOptions(set->getMaxWireVersion()));
            }

            return set.release();
        }

        case CUSTOM: {

            // Lock in case other things are modifying this at the same time
            boost::lock_guard<boost::mutex> lk( _connectHookMutex );

            // Allow the replacement of connections with other connections - useful for testing.

            uassert( 16335, "custom connection to " + this->toString() +
                        " specified with no connection hook", _connectHook );

            // Double-checked lock, since this will never be active during normal operation
            DBClientBase* replacementConn = _connectHook->connect( *this, errmsg, socketTimeout );

            log() << "replacing connection to " << this->toString() << " with "
                  << ( replacementConn ? replacementConn->getServerAddress() : "(empty)" ) << endl;

            return replacementConn;
        }

        case INVALID:
            throw UserException( 13421 , "trying to connect to invalid ConnectionString" );
            break;
        }

        verify( 0 );
        return 0;
    }

    bool ConnectionString::sameLogicalEndpoint( const ConnectionString& other ) const {
        if ( _type != other._type )
            return false;

        switch ( _type ) {
        case INVALID:
            return true;
        case MASTER:
            return _servers[0] == other._servers[0];
        case PAIR:
            if ( _servers[0] == other._servers[0] )
                return _servers[1] == other._servers[1];
            return
                ( _servers[0] == other._servers[1] ) &&
                ( _servers[1] == other._servers[0] );
        case SET:
            return _setName == other._setName;
        case CUSTOM:
            return _string == other._string;
        }
        verify( false );
    }

    ConnectionString ConnectionString::parse( const string& url , string& errmsg ) {
        if ( boost::algorithm::starts_with( url, "mongodb://" ) )
            return _parseURL( url, errmsg );
        errmsg = string("invalid connection string [") + url + "]";
        return ConnectionString(); // INVALID
    }

    ConnectionString ConnectionString::parseDeprecated( const string& address, string& errmsg ) {
        string::size_type i = address.find( '/' );
        if ( i != string::npos && i != 0) {
            // replica set
            return ConnectionString( SET , address.substr( i + 1 ) , address.substr( 0 , i ) );
        }

        int numCommas = str::count( address , ',' );

        if( numCommas == 0 )
            return ConnectionString( HostAndPort( address ) );

        if ( numCommas == 1 )
            return ConnectionString( PAIR , address );

        errmsg = string("invalid connection string [") + address + "]";
        return ConnectionString(); // INVALID
    }

    ConnectionString ConnectionString::_parseURL( const string& url, string& errmsg ) {

        const boost::regex mongoUrlRe(kMongoDBURL);

        boost::smatch matches;
        if (!boost::regex_match(url, matches, mongoUrlRe)) {
            errmsg = "Failed to parse mongodb:// URL: " + url;
            return ConnectionString();
        }

        // We have 5 top level captures, plus the whole input.
        invariant(matches.size() == 6);

        if (!matches[3].matched) {
            errmsg = "No server(s) specified";
            return ConnectionString();
        }

        std::map<std::string, std::string> options;

        if (matches[5].matched) {
            const std::string optionsMatch = matches[5].str();

            std::vector< boost::iterator_range<std::string::const_iterator> > optionsTokens;
            boost::algorithm::split(
                optionsTokens, optionsMatch, boost::algorithm::is_any_of("=&"));

            if (optionsTokens.size() % 2 != 0) {
                errmsg = "Missing a key or value in the options for mongodb:// URL: " + url;
                return ConnectionString();
            }

            for (size_t i = 0; i != optionsTokens.size(); i = i + 2)
                options[std::string(optionsTokens[i].begin(), optionsTokens[i].end())] =
                    std::string(optionsTokens[i + 1].begin(), optionsTokens[i + 1].end());
        }

        // Validate options against global driver state, and transform
        // or append relevant options to the auth struct.

        std::map<std::string, std::string>::const_iterator optIter;

        // If a replica set option was specified, store it in the 'setName' field.
        bool haveSetName;
        std::string setName;
        if ((haveSetName = ((optIter = options.find("replicaSet")) != options.end())))
            setName = optIter->second;

        // If an SSL option was specified that conflicts with the global setting, error out.
        // The driver doesn't offer per connection settings.
        if ((optIter = options.find("ssl")) != options.end()) {
            if (optIter->second != (client::Options::current().SSLEnabled() ? "true" : "false")) {
                errmsg = "Cannot override global driver SSL state in connection URL";
                return ConnectionString();
            }
        }

        // Add all remaining options into the bson object
        BSONObjBuilder optionsBob;
        for (optIter = options.begin(); optIter != options.end(); ++optIter)
            optionsBob.append(optIter->first, optIter->second);

        std::string servers = matches[3].str();
        const bool direct = !haveSetName && (servers.find(',') == std::string::npos);

        if (!direct && setName.empty()) {
            errmsg = "Cannot list multiple servers in URL without 'replicaSet' option";
            return ConnectionString();
        }

        return ConnectionString(
            direct ? MASTER : SET,
            matches[1].str(),
            matches[2].str(),
            servers,
            matches[4].str(),
            setName,
            optionsBob.obj());

    }

    string ConnectionString::typeToString( ConnectionType type ) {
        switch ( type ) {
        case INVALID:
            return "invalid";
        case MASTER:
            return "master";
        case PAIR:
            return "pair";
        case SET:
            return "set";
        case CUSTOM:
            return "custom";
        }
        verify(0);
        return "";
    }

    const BSONField<BSONObj> Query::ReadPrefField("$readPreference");
    const BSONField<string> Query::ReadPrefModeField("mode");
    const BSONField<BSONArray> Query::ReadPrefTagsField("tags");
    static const char* maxTimeMsField = "$maxTimeMS";

    Query::Query( const string &json ) : obj( fromjson( json ) ) {}

    Query::Query( const char *json ) : obj( fromjson( json ) ) {}

    Query& Query::hint(const string &indexName) {
        appendComplex( "$hint", indexName );
        return *this;
    }

    Query& Query::where(const string &jscode, BSONObj scope) {
        /* use where() before sort() and hint() and explain(), else this will assert. */
        verify( ! isComplex() );
        BSONObjBuilder b;
        b.appendElements(obj);
        b.appendWhere(jscode, scope);
        obj = b.obj();
        return *this;
    }

    void Query::makeComplex() {
        if ( isComplex() )
            return;
        BSONObjBuilder b;
        b.append( "query", obj );
        obj = b.obj();
    }

    Query& Query::sort(const BSONObj& s) {
        appendComplex( "orderby", s );
        return *this;
    }

    Query& Query::hint(BSONObj keyPattern) {
        appendComplex( "$hint", keyPattern );
        return *this;
    }

    Query& Query::maxTimeMs(int millis) {
        appendComplex( maxTimeMsField, millis );
        return *this;
    }

    Query& Query::explain() {
        appendComplex( "$explain", true );
        return *this;
    }

    Query& Query::snapshot() {
        appendComplex( "$snapshot", true );
        return *this;
    }

    Query& Query::minKey( const BSONObj &val ) {
        appendComplex( "$min", val );
        return *this;
    }

    Query& Query::maxKey( const BSONObj &val ) {
        appendComplex( "$max", val );
        return *this;
    }

    bool Query::isComplex(const BSONObj& obj, bool* hasDollar) {
        if (obj.hasElement("query")) {
            if (hasDollar) *hasDollar = false;
            return true;
        }

        if (obj.hasElement("$query")) {
            if (hasDollar) *hasDollar = true;
            return true;
        }

        return false;
    }

    BSONObj Query::getModifiers() const {
        std::set<string> names;
        obj.getFieldNames(names);

        BSONObjBuilder complexFields;

        std::set<string>::const_iterator nameIterator;
        for(nameIterator = names.begin(); nameIterator != names.end(); ++nameIterator) {
            if (nameIterator->find('$') == 0)
                complexFields.append(obj.getField(*nameIterator));
        }

        return complexFields.obj();
    }

    Query& Query::readPref(ReadPreference pref, const BSONArray& tags) {
        string mode;

        switch (pref) {
        case ReadPreference_PrimaryOnly:
            mode = "primary";
            break;

        case ReadPreference_PrimaryPreferred:
            mode = "primaryPreferred";
            break;

        case ReadPreference_SecondaryOnly:
            mode = "secondary";
            break;

        case ReadPreference_SecondaryPreferred:
            mode = "secondaryPreferred";
            break;

        case ReadPreference_Nearest:
            mode = "nearest";
            break;
        }

        BSONObjBuilder readPrefDocBuilder;
        readPrefDocBuilder << ReadPrefModeField(mode);

        if (!tags.isEmpty()) {
            readPrefDocBuilder << ReadPrefTagsField(tags);
        }

        appendComplex(ReadPrefField.name().c_str(), readPrefDocBuilder.done());
        return *this;
    }

    bool Query::isComplex( bool * hasDollar ) const {
        return isComplex(obj, hasDollar);
    }

    bool Query::hasReadPreference(const BSONObj& queryObj) {
        const bool hasReadPrefOption = queryObj["$queryOptions"].isABSONObj() &&
                        queryObj["$queryOptions"].Obj().hasField(ReadPrefField.name());
        return (Query::isComplex(queryObj) &&
                    queryObj.hasField(ReadPrefField.name())) ||
                hasReadPrefOption;
    }

    bool Query::hasReadPreference() const {
        return hasReadPreference(obj);
    }

    bool Query::hasHint() const {
        return obj.hasField( "$hint" );
    }

    bool Query::hasMaxTimeMs() const {
        return obj.hasField( maxTimeMsField );
    }

    BSONObj Query::getFilter() const {
        bool hasDollar;
        if ( ! isComplex( &hasDollar ) )
            return obj;

        return obj.getObjectField( hasDollar ? "$query" : "query" );
    }
    BSONObj Query::getSort() const {
        if ( ! isComplex() )
            return BSONObj();
        BSONObj ret = obj.getObjectField( "orderby" );
        if (ret.isEmpty())
            ret = obj.getObjectField( "$orderby" );
        return ret;
    }
    BSONElement Query::getHint() const {
        if ( ! isComplex() )
            return BSONElement();
        return obj.getField( "$hint" );
    }

    BSONObj Query::getReadPref() const {
        return obj.getObjectField(ReadPrefField.name());
    }

    int Query::getMaxTimeMs() const {
        return obj.getIntField(maxTimeMsField);
    }

    bool Query::isExplain() const {
        return isComplex() && obj.getBoolField( "$explain" );
    }

    string Query::toString() const {
        return obj.toString();
    }

    /* --- dbclientcommands --- */

    bool DBClientWithCommands::isOk(const BSONObj& o) {
        return o["ok"].trueValue();
    }

    bool DBClientWithCommands::isNotMasterErrorString( const BSONElement& e ) {
        return e.type() == String && str::contains( e.valuestr() , "not master" );
    }


    enum QueryOptions DBClientWithCommands::availableOptions() {
        if ( !_haveCachedAvailableOptions ) {
            _cachedAvailableOptions = _lookupAvailableOptions();
            _haveCachedAvailableOptions = true;
        }
        return _cachedAvailableOptions;
    }

    enum QueryOptions DBClientWithCommands::_lookupAvailableOptions() {
        BSONObj ret;
        if ( runCommand( "admin", BSON( "availablequeryoptions" << 1 ), ret ) ) {
            return QueryOptions( ret.getIntField( "options" ) );
        }
        return QueryOptions(0);
    }

    void DBClientWithCommands::setRunCommandHook(RunCommandHookFunc func) {
        _runCommandHook = func;
    }

    void DBClientWithCommands::setPostRunCommandHook(PostRunCommandHookFunc func) {
        _postRunCommandHook = func;
    }

    bool DBClientWithCommands::runCommand(const string &dbname,
                                          const BSONObj& cmd,
                                          BSONObj &info,
                                          int options) {
        string ns = dbname + ".$cmd";
        if (_runCommandHook) {
            BSONObjBuilder cmdObj;
            cmdObj.appendElements(cmd);
            _runCommandHook(&cmdObj);

            info = findOne(ns, cmdObj.done(), 0 , options);
        }
        else {
            info = findOne(ns, cmd, 0 , options);
        }
        if (_postRunCommandHook) {
            _postRunCommandHook(info, getServerAddress());
        }
        return isOk(info);
    }

    /* note - we build a bson obj here -- for something that is super common like getlasterror you
              should have that object prebuilt as that would be faster.
    */
    bool DBClientWithCommands::simpleCommand(const string &dbname, BSONObj *info, const string &command) {
        BSONObj o;
        if ( info == 0 )
            info = &o;
        BSONObjBuilder b;
        b.append(command, 1);
        return runCommand(dbname, b.done(), *info);
    }

    unsigned long long DBClientWithCommands::count(const string &myns, const Query& query, int options, int limit, int skip ) {
        BSONObj cmd = _countCmd( myns , query , options , limit , skip );
        BSONObj res;
        if( !runCommand(nsToDatabase(myns), cmd, res, options) )
            uasserted(11010,string("count fails:") + res.toString());
        return res["n"].numberLong();
    }

    BSONObj DBClientWithCommands::_countCmd(const string &myns, const Query& query, int options, int limit, int skip ) {
        NamespaceString ns(myns);
        BSONObjBuilder b;
        b.append( "count" , ns.coll() );
        b.append( "query" , query.getFilter() );
        if ( limit )
            b.append( "limit" , limit );
        if ( skip )
            b.append( "skip" , skip );

        if ( query.isComplex() ) {
            if ( query.hasHint() ) {
                BSONElement hint(query.getHint());
                if ( hint.isABSONObj() )
                    b.append( "hint", hint.Obj() );
                else
                    b.append( "hint", hint.String() );
            }

            // We don't want $hint anymore as we've already used it inside the command
            b.appendElements(query.getModifiers().removeField("$hint"));
        }

        return b.obj();
    }

    BSONObj DBClientWithCommands::getLastErrorDetailed(bool fsync, bool j, int w, int wtimeout) {
        return getLastErrorDetailed("admin", fsync, j, w, wtimeout);
    }

    BSONObj DBClientWithCommands::getLastErrorDetailed(const std::string& db,
                                                       bool fsync,
                                                       bool j,
                                                       int w,
                                                       int wtimeout) {
        BSONObj info;
        BSONObjBuilder b;
        b.append( "getlasterror", 1 );

        if ( fsync )
            b.append( "fsync", 1 );
        if ( j )
            b.append( "j", 1 );

        // only affects request when greater than one node
        if ( w >= 1 )
            b.append( "w", w );
        else if ( w == -1 )
            b.append( "w", "majority" );

        if ( wtimeout > 0 )
            b.append( "wtimeout", wtimeout );

        runCommand(db, b.obj(), info);

        return info;
    }

    string DBClientWithCommands::getLastError(bool fsync, bool j, int w, int wtimeout) {
        return getLastError("admin", fsync, j, w, wtimeout);
    }

    string DBClientWithCommands::getLastError(const std::string& db,
                                              bool fsync,
                                              bool j,
                                              int w,
                                              int wtimeout) {
        BSONObj info = getLastErrorDetailed(db, fsync, j, w, wtimeout);
        return getLastErrorString( info );
    }

    string DBClientWithCommands::getLastErrorString(const BSONObj& info) {
        if (info["ok"].trueValue()) {
            BSONElement e = info["err"];
            if (e.eoo()) return "";
            if (e.type() == Object) return e.toString();
            return e.str();
        } else {
            // command failure
            BSONElement e = info["errmsg"];
            if (e.eoo()) return "";
            if (e.type() == Object) return "getLastError command failed: " + e.toString();
            return "getLastError command failed: " + e.str();
        }
    }

    const BSONObj getpreverrorcmdobj = fromjson("{getpreverror:1}");

    BSONObj DBClientWithCommands::getPrevError() {
        BSONObj info;
        runCommand("admin", getpreverrorcmdobj, info);
        return info;
    }

    BSONObj getnoncecmdobj = fromjson("{getnonce:1}");

    string DBClientWithCommands::createPasswordDigest( const string & username , const string & clearTextPassword ) {
        return mongo::createPasswordDigest(username, clearTextPassword);
    }

    void DBClientWithCommands::_auth(const BSONObj& params) {
        std::string mechanism;

        uassertStatusOK(bsonExtractStringField(params,
                                               saslCommandMechanismFieldName,
                                               &mechanism));

        uassert(17232, "You cannot specify both 'db' and 'userSource'. Please use only 'db'.",
                !(params.hasField(saslCommandUserDBFieldName)
                  && params.hasField(saslCommandUserSourceFieldName)));

        if (mechanism == kAuthMechMongoCR) {
            std::string db;
            if (params.hasField(saslCommandUserSourceFieldName)) {
                uassertStatusOK(bsonExtractStringField(params,
                                                       saslCommandUserSourceFieldName,
                                                       &db));
            }
            else {
                uassertStatusOK(bsonExtractStringField(params,
                                                       saslCommandUserDBFieldName,
                                                       &db));
            }
            std::string user;
            uassertStatusOK(bsonExtractStringField(params,
                                                   saslCommandUserFieldName,
                                                   &user));
            std::string password;
            uassertStatusOK(bsonExtractStringField(params,
                                                   saslCommandPasswordFieldName,
                                                   &password));
            bool digestPassword;
            uassertStatusOK(bsonExtractBooleanFieldWithDefault(params,
                                                               saslCommandDigestPasswordFieldName,
                                                               true,
                                                               &digestPassword));
            uassert(ErrorCodes::AuthenticationFailed,
                    "Cannot set mechanism_properties when using MONGODB_CR",
                    !params.hasField(kAuthMechanismPropertiesKey));

            BSONObj result;
            uassert(result["code"].Int(),
                    result.toString(),
                    _authMongoCR(db, user, password, &result, digestPassword));
        }
#ifdef MONGO_SSL
        else if (mechanism == StringData("MONGODB-X509", StringData::LiteralTag())){
            std::string db;
            if (params.hasField(saslCommandUserSourceFieldName)) {
                uassertStatusOK(bsonExtractStringField(params,
                                                       saslCommandUserSourceFieldName,
                                                       &db));
            }
            else {
                uassertStatusOK(bsonExtractStringField(params,
                                                       saslCommandUserDBFieldName,
                                                       &db));
            }
            std::string user;
            uassertStatusOK(bsonExtractStringField(params,
                                                   saslCommandUserFieldName,
                                                   &user));

            uassert(ErrorCodes::AuthenticationFailed,
                    "Please enable SSL on the client-side to use the MONGODB-X509 "
                    "authentication mechanism.",
                    getSSLManager() != NULL);

            uassert(ErrorCodes::AuthenticationFailed,
                    "Username \"" + user + 
                    "\" does not match the provided client certificate user \"" +
                    getSSLManager()->getClientSubjectName() + "\"",
                    user ==  getSSLManager()->getClientSubjectName());

            uassert(ErrorCodes::AuthenticationFailed,
                    "Cannot set mechanism_properties when using MONGODB_X509",
                    !params.hasField(kAuthMechanismPropertiesKey));

            BSONObj result;
            uassert(result["code"].Int(),
                    result.toString(),
                    _authX509(db, user, &result));
        }
#endif
        else if (saslClientAuthenticate != NULL) {
            uassertStatusOK(saslClientAuthenticate(this, params));
        }
        else {
            std::string error = mechanism + " mechanism support not compiled into client library."
                                            " (Some mechanisms require the driver be compiled"
                                            " with the flags --ssl or --use-sasl-client)";
            uasserted(ErrorCodes::BadValue, error);
        }
    };

    void DBClientWithCommands::auth(const BSONObj& params) {
        _auth(params);
    }

    bool DBClientWithCommands::auth(const string &dbname,
                                    const string &username,
                                    const string &password_text,
                                    string& errmsg,
                                    bool digestPassword) {
        try {
            const char* mech = kAuthMechMongoCR;
            if( _maxWireVersion >= 3 ) {
                mech = kAuthMechScramSha1;
            }
            _auth(BSON(saslCommandMechanismFieldName << mech <<
                       saslCommandUserDBFieldName << dbname <<
                       saslCommandUserFieldName << username <<
                       saslCommandPasswordFieldName << password_text <<
                       saslCommandDigestPasswordFieldName << digestPassword));
            return true;
        } catch(const UserException& ex) {
            if (ex.getCode() != ErrorCodes::AuthenticationFailed)
                throw;
            errmsg = ex.what();
            return false;
        }
    }

    bool DBClientWithCommands::_authMongoCR(const string &dbname,
                                            const string &username,
                                            const string &password_text,
                                            BSONObj *info,
                                            bool digestPassword) {

        string password = password_text;
        if( digestPassword )
            password = createPasswordDigest( username , password_text );

        string nonce;
        if( !runCommand(dbname, getnoncecmdobj, *info) ) {
            return false;
        }
        {
            BSONElement e = info->getField("nonce");
            verify( e.type() == String );
            nonce = e.valuestr();
        }

        BSONObj authCmd;
        BSONObjBuilder b;
        {

            b << "authenticate" << 1 << "nonce" << nonce << "user" << username;
            md5digest d;
            {
                md5_state_t st;
                md5_init(&st);
                md5_append(&st, (const md5_byte_t *) nonce.c_str(), nonce.size() );
                md5_append(&st, (const md5_byte_t *) username.data(), username.length());
                md5_append(&st, (const md5_byte_t *) password.c_str(), password.size() );
                md5_finish(&st, d);
            }
            b << "key" << digestToString( d );
            authCmd = b.done();
        }

        if( runCommand(dbname, authCmd, *info) ) {
            return true;
        }

        return false;
    }

    bool DBClientWithCommands::_authX509(const string&dbname,
                                         const string &username,
                                         BSONObj *info){
        BSONObj authCmd;
        BSONObjBuilder cmdBuilder;
        cmdBuilder << "authenticate" << 1 << "mechanism" << "MONGODB-X509" << "user" << username;
        authCmd = cmdBuilder.done();

        if( runCommand(dbname, authCmd, *info) ) {
            return true;
        }

        return false;
    }

    void DBClientWithCommands::logout(const string &dbname, BSONObj& info) {
        runCommand(dbname, BSON("logout" << 1), info);
    }

    BSONObj ismastercmdobj = fromjson("{\"ismaster\":1}");

    bool DBClientWithCommands::isMaster(bool& isMaster, BSONObj *info) {
        BSONObj o;
        if ( info == 0 )
            info = &o;
        bool ok = runCommand("admin", ismastercmdobj, *info);
        isMaster = info->getField("ismaster").trueValue();
        return ok;
    }

    bool DBClientWithCommands::createCollection(
        const string &ns,
        long long size,
        bool capped,
        int max,
        BSONObj *info
    ) {
        return createCollectionWithOptions(ns, size, capped, max, BSONObj(), info);
    }

    bool DBClientWithCommands::createCollectionWithOptions(
        const string &ns,
        long long size,
        bool capped,
        int max,
        const BSONObj& extra,
        BSONObj *info
    ) {
        verify(!capped||size);
        BSONObj o;
        if ( info == 0 )    info = &o;
        BSONObjBuilder b;
        string db = nsToDatabase(ns);
        b.append("create", ns.c_str() + db.length() + 1);
        if ( size ) b.append("size", size);
        if ( capped ) b.append("capped", true);
        if ( max ) b.append("max", max);
        if ( !extra.isEmpty() ) b.appendElements(extra);
        return runCommand(db.c_str(), b.done(), *info);
    }

    bool DBClientWithCommands::copyDatabase(const string& fromdb,
                                            const string& todb,
                                            const string& fromhost,
                                            const string& mechanism,
                                            const string& username,
                                            const string& password,
                                            BSONObj *info) {
        BSONObj o;
        if ( info == 0 ) info = &o;
        BSONObjBuilder copydbCmd;
        copydbCmd.append("copydb", 1);
        copydbCmd.append("fromhost", fromhost);
        copydbCmd.append("fromdb", fromdb);
        copydbCmd.append("todb", todb);

        // If we don't have a username, or if we're copying locally,
        // just run the command without authenticating
        if ( username == "" || fromhost == "" ) {
            return runCommand("admin", copydbCmd.done(), *info);
        }

        // Otherwise, take or guess the auth mechanism
        string authMech;
        if ( mechanism != kAuthMechDefault ) {
            uassert( 0, "auth mechanism must be MONGODB-CR or SCRAM-SHA-1",
                     ( mechanism == kAuthMechMongoCR ||
                       mechanism == kAuthMechScramSha1));
            authMech = mechanism;
        }
#ifdef MONGO_SSL
        else if ( (static_cast<DBClientBase*>(this))->getMaxWireVersion() >= 3 ) {
            authMech = kAuthMechScramSha1;
        }
#endif
        else {
            authMech = kAuthMechMongoCR;
        }

        if ( authMech == kAuthMechMongoCR ) {
            // run MONGODB-CR copydb
            BSONObj nonceInfo;
            BSONObjBuilder nonceCmd;
            BSONElement e;
            string nonce;

            nonceCmd.append("copydbgetnonce", 1);
            nonceCmd.append("fromhost", fromhost);

            verify( runCommand("admin", nonceCmd.done(), nonceInfo) );
            {
                BSONElement e = nonceInfo.getField("nonce");
                verify( e.type() == String );
                nonce = e.valuestr();
            }

            copydbCmd.append("username", username);
            copydbCmd.append("nonce", nonce);
            copydbCmd.append("key", authKeyCopyDBMongoCR(username, password, nonce));
            return runCommand("admin", copydbCmd.done(), *info);
        }
        else {
            // run SCRAM-SHA-1 copydb, but only with SSL
#ifndef MONGO_SSL
            uassert( 0, "SCRAM-SHA-1 authentication requires driver to be built with SSL", false );
#endif
#ifdef MONGO_SSL
            string hashedPwd = createPasswordDigest(username, password);

            // create and initialize our sasl session
            boost::scoped_ptr<SaslClientSession> session(new NativeSaslClientSession());
            session->setParameter(SaslClientSession::parameterMechanism, kAuthMechScramSha1);
            session->setParameter(SaslClientSession::parameterUser, username);
            session->setParameter(SaslClientSession::parameterPassword, hashedPwd);
            session->initialize();

            // set up commands to feed the sasl state machine
            BSONObj saslFirstCommandPrefix = BSON("copydbsaslstart" << 1 <<
                                                  "fromhost" << fromhost <<
                                                  "fromdb" << fromdb <<
                                                  saslCommandMechanismFieldName << kAuthMechScramSha1);

            BSONObj saslFollowupCommandPrefix = BSON("copydb" << 1 <<
                                                     "fromhost" << fromhost <<
                                                     "fromdb" << fromdb <<
                                                     "todb" << todb);

            BSONObj saslCommandPrefix = saslFirstCommandPrefix;
            BSONObj inputObj = BSON(saslCommandPayloadFieldName << "");
            bool isServerDone = false;

            // send copydbsaslstart, then continue to send copydb until we are done.
            while (!session->isDone()) {
                string payload;
                BSONType type;

                Status status = saslExtractPayload(inputObj, &payload, &type);
                if (!status.isOK()) {
                    throw DBException( str::stream() << "sasl session failure: " << status.reason(), 0 );
                }

                string responsePayload;
                status = session->step(payload, &responsePayload);
                if (!status.isOK()) {
                    throw DBException( str::stream() << "sasl session failure: " << status.reason(), 0 );
                }

                // build command to send to server
                BSONObjBuilder commandBuilder;
                commandBuilder.appendElements(saslCommandPrefix);
                commandBuilder.appendBinData(saslCommandPayloadFieldName,
                                             int(responsePayload.size()),
                                             BinDataGeneral,
                                             responsePayload.c_str());
                BSONElement conversationId = inputObj[saslCommandConversationIdFieldName];
                if (!conversationId.eoo()) {
                    commandBuilder.append(conversationId);
                }

                BSONObj command = commandBuilder.obj();
                bool ok = runCommand("admin", command, inputObj);

                ErrorCodes::Error code =
                    ErrorCodes::fromInt(inputObj[saslCommandCodeFieldName].numberInt());

                if (!ok || code != ErrorCodes::OK) {
                    if (code == ErrorCodes::OK)
                        code = ErrorCodes::UnknownError;
                    // attempt to give a sane error message
                    if ( inputObj.hasField("errmsg") ) {
                        BSONElement e = inputObj.getField("errmsg");
                        uassert( 0, "fromhost " + fromhost + " doesn't support SCRAM-SHA-1, use MONGODB-CR",
                                 ( e.type() == String &&
                                   strstr( e.valuestr(), "no such cmd: saslStart")));
                    }
                    throw OperationException( inputObj );
                }

                isServerDone = inputObj[saslCommandDoneFieldName].trueValue();
                saslCommandPrefix = saslFollowupCommandPrefix;
            }
            if (!isServerDone) {
                invariant(false);
            }
            return true;
#endif /* MONGO_SSL */
        }
        invariant(false);
    }

    bool DBClientWithCommands::setDbProfilingLevel(const string &dbname, ProfilingLevel level, BSONObj *info ) {
        BSONObj o;
        if ( info == 0 ) info = &o;

        if ( level ) {
            // Create system.profile collection.  If it already exists this does nothing.
            // TODO: move this into the db instead of here so that all
            //       drivers don't have to do this.
            string ns = dbname + ".system.profile";
            createCollection(ns.c_str(), 1024 * 1024, true, 0, info);
        }

        BSONObjBuilder b;
        b.append("profile", (int) level);
        return runCommand(dbname, b.done(), *info);
    }

    BSONObj getprofilingcmdobj = fromjson("{\"profile\":-1}");

    bool DBClientWithCommands::getDbProfilingLevel(const string &dbname, ProfilingLevel& level, BSONObj *info) {
        BSONObj o;
        if ( info == 0 ) info = &o;
        if ( runCommand(dbname, getprofilingcmdobj, *info) ) {
            level = (ProfilingLevel) info->getIntField("was");
            return true;
        }
        return false;
    }

    DBClientWithCommands::MROutput DBClientWithCommands::MRInline (BSON("inline" << 1));

    BSONObj DBClientWithCommands::mapreduce(const string &ns, const string &jsmapf, const string &jsreducef, Query query, MROutput output) {
        BSONObjBuilder b;
        b.append("mapreduce", nsGetCollection(ns));
        b.appendCode("map", jsmapf);
        b.appendCode("reduce", jsreducef);

        if( !query.obj.isEmpty() )
            b.append("query", query.getFilter());

        if (query.isComplex())
            b.appendElements(query.getModifiers());

        b.append("out", output.out);
        BSONObj info;
        runCommand(nsGetDB(ns), b.done(), info);
        return info;
    }

    void DBClientWithCommands::group(
        const StringData& ns,
        const StringData& jsreduce,
        std::vector<BSONObj>* output,
        const BSONObj& initial,
        const Query& query,
        const BSONObj& key,
        const StringData& finalize
    ) {
        BSONObjBuilder groupObjBuilder;
        _buildGroupObj(ns, jsreduce, initial, query, finalize, &groupObjBuilder);

        if (!key.isEmpty())
            groupObjBuilder.append("key", key);

        _runGroup(ns, groupObjBuilder.obj(), query, output);
    }

    void DBClientWithCommands::groupWithKeyFunction(
        const StringData& ns,
        const StringData& jsreduce,
        std::vector<BSONObj>* output,
        const BSONObj& initial,
        const Query& query,
        const StringData& jskey,
        const StringData& finalize
    ) {
        BSONObjBuilder groupBuilder;
        _buildGroupObj(ns, jsreduce, initial, query, finalize, &groupBuilder);

        if (!jskey.empty())
            groupBuilder.append("$keyf", jskey);

        _runGroup(ns, groupBuilder.obj(), query, output);
    }

    void DBClientWithCommands::_buildGroupObj(
        const StringData& ns,
        const StringData& jsreduce,
        const BSONObj& initial,
        const Query& query,
        const StringData& finalize,
        BSONObjBuilder* groupObj
    ) {
        groupObj->append("ns", nsGetCollection(ns.toString()));
        groupObj->appendCode("$reduce", jsreduce);
        groupObj->append("initial", initial);

        if (!query.obj.isEmpty())
            groupObj->append("cond", query.getFilter());
        if (!finalize.empty())
            groupObj->append("finalize", finalize);
    }

    void DBClientWithCommands::_runGroup(
        const StringData& ns,
        const BSONObj& group,
        const Query& query,
        std::vector<BSONObj>* output
    ) {
        BSONObjBuilder commandBuilder;
        commandBuilder.append("group", group);

        if (query.isComplex())
            commandBuilder.appendElements(query.getModifiers());

        BSONObj result;
        bool ok = runCommand(nsGetDB(ns.toString()), commandBuilder.obj(), result);

        if (!ok)
            throw OperationException(result);

        BSONElement resultArray = result.getField("retval");
        BSONObjIterator resultIterator(resultArray.Obj());

        while (resultIterator.more()) {
            output->push_back(resultIterator.next().Obj().getOwned());
        }
    }

    BSONObj DBClientWithCommands::distinct(
        const StringData& ns,
        const StringData& field,
        const Query& query
    ) {
        BSONObjBuilder commandBuilder;
        commandBuilder.append("distinct", nsGetCollection(ns.toString()));
        commandBuilder.append("key", field);
        commandBuilder.append("query", query.getFilter());

        if (query.isComplex())
            commandBuilder.appendElements(query.getModifiers());

        BSONObj result;
        bool ok = runCommand(nsGetDB(ns.toString()), commandBuilder.obj(), result);

        if (!ok)
            throw OperationException(result);

        return result.getField("values").Obj().getOwned();
    }

    void DBClientWithCommands::_findAndModify(
        const StringData& ns,
        const BSONObj& query,
        const BSONObj& update,
        const BSONObj& sort,
        bool returnNew,
        bool upsert,
        const BSONObj& fields,
        BSONObjBuilder* out
    ) {
        BSONObjBuilder commandBuilder;

        commandBuilder.append("findAndModify", nsGetCollection(ns.toString()));

        if (update.isEmpty())
            commandBuilder.append("remove", true);
        else
            commandBuilder.append("update", update);

        if (!query.isEmpty())
            commandBuilder.append("query", query);

        if (!sort.isEmpty())
            commandBuilder.append("sort", sort);

        if (!fields.isEmpty())
            commandBuilder.append("fields", fields);

        commandBuilder.append("new", returnNew);
        commandBuilder.append("upsert", upsert);

        BSONObj result;
        bool ok = runCommand(nsGetDB(ns.toString()), commandBuilder.obj(), result);

        if (!ok)
            throw OperationException(result);

        out->appendElements(result.getObjectField("value"));
    }

    BSONObj DBClientWithCommands::findAndModify(
        const StringData& ns,
        const BSONObj& query,
        const BSONObj& update,
        bool upsert,
        bool returnNew,
        const BSONObj& sort,
        const BSONObj& fields
    ) {
        BSONObjBuilder result;
        _findAndModify(ns, query, update, sort, returnNew, upsert, fields, &result);
        return result.obj();
    }

    BSONObj DBClientWithCommands::findAndRemove(
        const StringData& ns,
        const BSONObj& query,
        const BSONObj& sort,
        const BSONObj& fields
    ) {
        BSONObjBuilder result;
        _findAndModify(ns, query, BSONObj(), sort, false, false, fields, &result);
        return result.obj();
    }

    bool DBClientWithCommands::eval(const string &dbname, const string &jscode, BSONObj& info, BSONElement& retValue, BSONObj *args, bool nolock) {
        BSONObjBuilder b;
        b.appendCode("$eval", jscode);
        if ( args ) {
            b.appendArray("args", *args);
        }
        if ( nolock ) {
            b.appendBool("nolock", true);
        }
        bool ok = runCommand(dbname, b.done(), info);
        if ( ok )
            retValue = info.getField("retval");
        return ok;
    }

    bool DBClientWithCommands::eval(const string &dbname, const string &jscode) {
        BSONObj info;
        BSONElement retValue;
        return eval(dbname, jscode, info, retValue);
    }

    list<string> DBClientWithCommands::getDatabaseNames() {
        BSONObj info;
        uassert(10005, "listdatabases failed", runCommand("admin",
                                                          BSON("listDatabases" << 1),
                                                          info,
                                                          QueryOption_SlaveOk));
        uassert( 10006 ,  "listDatabases.databases not array" , info["databases"].type() == Array );

        list<string> names;

        BSONObjIterator i( info["databases"].embeddedObjectUserCheck() );
        while ( i.more() ) {
            names.push_back( i.next().embeddedObjectUserCheck()["name"].valuestr() );
        }

        return names;
    }

    list<string> DBClientWithCommands::getCollectionNames( const string& db,
                                                           const BSONObj& filter ) {
        auto_ptr<DBClientCursor> infos = enumerateCollections( db, filter );
        list<string> names;

        while (infos->more()) {
            names.push_back(infos->nextSafe()["name"].valuestr());
        }

        return names;
    }

    list<BSONObj> DBClientWithCommands::getCollectionInfos( const string& db,
                                                            const BSONObj& filter ) {
        auto_ptr<DBClientCursor> info_cursor = enumerateCollections( db, filter );
        list<BSONObj> infos;

        while (info_cursor->more()) {
            infos.push_back(info_cursor->nextSafe().getOwned());
        }

        return infos;
    }

    // Entries in the system.namespaces collection are fully qualified so the database name
    // must be stripped from the query output for each collection's name
    bool transformLegacyCollectionInfos(const BSONObj& input, BSONObj* output) {
        const StringData ns = input["name"].checkAndGetStringData();

        // Filter the $ collections out
        if ( ns.find( '$' ) != string::npos )
            return false;

        // Strip the database from the name
        BSONObjBuilder b;
        b.append( "name", ns.substr( ns.find(".") + 1 ) );
        b.appendElementsUnique( input );
        *output = b.obj();
        return true;
    }

    auto_ptr<DBClientCursor> DBClientWithCommands::_legacyCollectionInfo( const string& db,
                                                                          const BSONObj& filter,
                                                                          int batchSize ) {
        // SERVER-14951 filter for old version fallback needs to db qualify the 'name' element
        BSONObjBuilder fallbackFilter;
        if ( filter.hasField( "name" ) && filter["name"].type() == String ) {
            fallbackFilter.append( "name", db + "." + filter["name"].str() );
        }
        fallbackFilter.appendElementsUnique( filter );

        string namespaces_ns = db + ".system.namespaces";

        auto_ptr<DBClientCursor> simple = query(namespaces_ns, fallbackFilter.obj(),
                                                0, 0, 0, QueryOption_SlaveOk, batchSize);

        simple->shim.reset(new DBClientCursorShimTransform(*simple, transformLegacyCollectionInfos));
        simple->nToReturn = 0;
        simple->setBatchSize(batchSize);

        return simple;
    }

    auto_ptr<DBClientCursor> DBClientWithCommands::enumerateCollections( const string& db,
                                                                         const BSONObj& filter,
                                                                         int batchSize ) {
        const std::string command_ns = db + ".$cmd";

        BSONObj cmd = BSON("listCollections" << 1 << "filter" << filter
            << "cursor" << (batchSize ? BSON("batchSize" << batchSize) : BSONObj()));

        auto_ptr<DBClientCursor> cursor = this->query(command_ns, cmd, 1, 0, NULL,
                                                      QueryOption_SlaveOk, 0);

        if ( cursor.get() ) {
            BSONObj result = cursor->peekFirst();

            if ( isOk(result) ) {
                // Command worked -- we are on MongoDB 2.7.6 or above
                DBClientCursorShim* cursor_shim;

                // Select the appropriate shim for this version of MongoDB
                if ( result.hasField("collections") ) {
                    // MongoDB 2.7.6 to 2.8.0-rc2 behavior
                    cursor_shim = new DBClientCursorShimArray(*cursor, "collections");
                }
                else {
                    // MongoDB 2.8.0-rc3+ behavior
                    cursor_shim = new DBClientCursorShimCursorID(*cursor);
                    static_cast<DBClientCursorShimCursorID*>(cursor_shim)->get_cursor();
                }

                // Insert the shim
                cursor->shim.reset(cursor_shim);
                cursor->nToReturn = 0;
                cursor->setBatchSize(batchSize);
            }
            else {
                // Command failed -- we are either on an older MongoDB or something else happened
                int error_code = result["code"].numberInt();
                string errmsg = result["errmsg"].valuestrsafe();

                if (
                    ( error_code == ErrorCodes::CommandNotFound ) ||
                    ( error_code == 13390 ) ||
                    ( errmsg.find( "no such cmd" ) != string::npos )
                ) {
                    // MongoDB < 2.7.6 behavior -- run legacy code to produce a cursor
                    cursor.reset(_legacyCollectionInfo(db, filter, batchSize).release());
                }
                else {
                    // Something else happened, uassert with the reason
                    uasserted( 18630, str::stream() << "listCollections failed: " << result );
                }
            }
        }

        return cursor;
    }

    bool DBClientWithCommands::exists( const string& ns ) {
        BSONObj filter = BSON( "name" << nsToCollectionSubstring( ns ) );
        auto_ptr<DBClientCursor> results = enumerateCollections( nsToDatabase( ns ), filter );
        return results->more();
    }

    /* --- dbclientconnection --- */

    void DBClientConnection::_auth(const BSONObj& params) {

        if( autoReconnect ) {
            /* note we remember the auth info before we attempt to auth -- if the connection is broken, we will
               then have it for the next autoreconnect attempt.
            */
            authCache[params[saslCommandUserDBFieldName].str()] = params.getOwned();
        }

        DBClientBase::_auth(params);
    }

    /** query N objects from the database into an array.  makes sense mostly when you want a small number of results.  if a huge number, use 
        query() and iterate the cursor. 
     */
    void DBClientInterface::findN(vector<BSONObj>& out, const string& ns, Query query, int nToReturn, int nToSkip, const BSONObj *fieldsToReturn, int queryOptions, int batchSize) {
        out.reserve(std::min(batchSize, nToReturn));

        auto_ptr<DBClientCursor> c =
            this->query(ns, query, nToReturn, nToSkip, fieldsToReturn, queryOptions, batchSize);

        uassert( 10276 ,  str::stream() << "DBClientBase::findN: transport error: " << getServerAddress() << " ns: " << ns << " query: " << query.toString(), c.get() );

        for( int i = 0; i < nToReturn; i++ ) {
            if ( !c->more() )
                break;
            out.push_back( c->nextSafe().copy() );
        }
    }

    BSONObj DBClientInterface::findOne(const string &ns, const Query& query, const BSONObj *fieldsToReturn, int queryOptions) {
        vector<BSONObj> v;
        findN(v, ns, query, 1, 0, fieldsToReturn, queryOptions);
        return v.empty() ? BSONObj() : v[0];
    }

    void DBClientInterface::save( const StringData& ns, const BSONObj& toSave, const WriteConcern* wc ) {
        if (!toSave.hasField("_id"))
            insert(ns.rawData(), toSave, 0, wc);
        else
            update(ns.rawData(), MONGO_QUERY("_id" << toSave.getField("_id")), toSave, true, false, wc);
    }

    bool DBClientConnection::connect(const HostAndPort& server, string& errmsg) {
        _server = server;
        _serverString = _server.toString();
        return _connect( errmsg );
    }

    bool DBClientConnection::_connect( string& errmsg ) {
        _serverString = _server.toString();
        _serverAddrString.clear();

        // we keep around SockAddr for connection life -- maybe MessagingPort
        // requires that?
        std::auto_ptr<SockAddr> serverSockAddr(new SockAddr(_server.host().c_str(),
                                                            _server.port()));
        if (!serverSockAddr->isValid()) {
            errmsg = str::stream() << "couldn't initialize connection to host "
                                   << _server.host().c_str() << ", address is invalid";
            return false;
        }

        server.reset(serverSockAddr.release());
        p.reset(new MessagingPort( _so_timeout, _logLevel ));

        if (_server.host().empty() ) {
            errmsg = str::stream() << "couldn't connect to server " << toString()
                                   << ", host is empty";
            return false;
        }

        _serverAddrString = server->getAddr();

        if ( _serverAddrString == "0.0.0.0" ) {
            errmsg = str::stream() << "couldn't connect to server " << toString()
                                   << ", address resolved to 0.0.0.0";
            return false;
        }

        if ( !p->connect(*server) ) {
            errmsg = str::stream() << "couldn't connect to server " << toString()
                                   << ", connection attempt failed";
            _failed = true;
            return false;
        }
        else {
            LOG( 1 ) << "connected to server " << toString() << endl;
        }

#ifdef MONGO_SSL
        if (client::Options::current().SSLEnabled() &&
            !p->secure(sslManager(), _server.host())) {
            return false;
        }
#endif
        BSONObj info;
        bool worked = simpleCommand("admin", &info, "ismaster");
        if (worked) {
            if (info.hasField("maxBsonObjectSize"))
                _maxBsonObjectSize = info.getIntField("maxBsonObjectSize");
            if (info.hasField("maxMessageSizeBytes"))
                _maxMessageSizeBytes = info.getIntField("maxMessageSizeBytes");
            if (info.hasField("maxWriteBatchSize"))
                _maxWriteBatchSize = info.getIntField("maxWriteBatchSize");
            if (info.hasField("minWireVersion"))
                _minWireVersion = info.getIntField("minWireVersion");
            if (info.hasField("maxWireVersion"))
                _maxWireVersion = info.getIntField("maxWireVersion");
        }

        return worked;
    }

    void DBClientConnection::logout(const string& dbname, BSONObj& info){
        authCache.erase(dbname);
        runCommand(dbname, BSON("logout" << 1), info);
    }

    bool DBClientConnection::runCommand(const string &dbname,
                                        const BSONObj& cmd,
                                        BSONObj &info,
                                        int options) {
        if (DBClientWithCommands::runCommand(dbname, cmd, info, options))
            return true;

        if (!_parentReplSetName.empty()) {
            handleNotMasterResponse(info["errmsg"]);
        }

        return false;
    }

    void DBClientConnection::_checkConnection() {
        if ( !_failed )
            return;

        if ( !autoReconnect )
            throw SocketException( SocketException::FAILED_STATE , toString() );

        // Don't hammer reconnects, backoff if needed
        autoReconnectBackoff.nextSleepMillis();

        LOG(_logLevel) << "trying reconnect to " << toString() << endl;
        string errmsg;
        _failed = false;
        if ( ! _connect(errmsg) ) {
            _failed = true;
            LOG(_logLevel) << "reconnect " << toString() << " failed " << errmsg << endl;
            throw SocketException( SocketException::CONNECT_ERROR , toString() );
        }

        LOG(_logLevel) << "reconnect " << toString() << " ok" << endl;
        for( map<string, BSONObj>::const_iterator i = authCache.begin(); i != authCache.end(); i++ ) {
            try {
                DBClientConnection::_auth(i->second);
            } catch (UserException& ex) {
                if (ex.getCode() != ErrorCodes::AuthenticationFailed)
                    throw;
                LOG(_logLevel) << "reconnect: auth failed " <<
                    i->second[saslCommandUserDBFieldName] <<
                    i->second[saslCommandUserFieldName] << ' ' <<
                    ex.what() << std::endl;
            }
        }
    }

    void DBClientConnection::setSoTimeout(double timeout) {
        _so_timeout = timeout;
        if (p) {
            p->setSocketTimeout(timeout);
        }
    }

    uint64_t DBClientConnection::getSockCreationMicroSec() const {
        if (p) {
            return p->getSockCreationMicroSec();
        }
        else {
            return INVALID_SOCK_CREATION_TIME;
        }
    }

    const uint64_t DBClientBase::INVALID_SOCK_CREATION_TIME =
            static_cast<uint64_t>(0xFFFFFFFFFFFFFFFFULL);

    DBClientBase::DBClientBase()
        : _wireProtocolWriter(new WireProtocolWriter(this))
        , _commandWriter(new CommandWriter(this))
    {
        _writeConcern = WriteConcern::acknowledged;
        _connectionId = ConnectionIdSequence.fetchAndAdd(1);
        _maxBsonObjectSize = defaultMaxBsonObjectSize;
        _maxMessageSizeBytes = defaultMaxMessageSizeBytes;
        _maxWriteBatchSize = defaultMaxWriteBatchSize;
    }

    DBClientBase::~DBClientBase() {
    }

    auto_ptr<DBClientCursor> DBClientBase::query(const string &ns, Query query, int nToReturn,
            int nToSkip, const BSONObj *fieldsToReturn, int queryOptions , int batchSize ) {
        auto_ptr<DBClientCursor> c( new DBClientCursor( this,
                                    ns, query.obj, nToReturn, nToSkip,
                                    fieldsToReturn, queryOptions , batchSize ) );
        if ( c->init() )
            return c;
        return auto_ptr< DBClientCursor >( 0 );
    }

    void DBClientBase::parallelScan(
        const StringData& ns,
        int numCursors,
        std::vector<DBClientCursor*>* cursors,
        stdx::function<DBClientBase* ()> connectionFactory
    ) {
        BSONObjBuilder bob;
        bob.append("parallelCollectionScan", nsGetCollection(ns.toString()));
        bob.append("numCursors", numCursors);

        BSONObj result;
        bool ok = runCommand(nsGetDB(ns.toString()), bob.obj(), result);

        if (!ok)
            throw OperationException(result);

        BSONObj resultArray = result.getObjectField("cursors");
        BSONObjIterator arrayIterator(resultArray);

        while (arrayIterator.more()) {
            long long cursorId = arrayIterator.next().Obj().getFieldDotted("cursor.id").numberLong();

            DBClientBase* conn = NULL;

            // Attempt to get a connection for this cursor from the user provided connectionFactory
            try {
                conn = connectionFactory();
            } catch (...) {
                while (arrayIterator.more()) {
                    try {
                        DBClientCursor killOnServer(this, ns.toString(), cursorId, 0, 0, 0);
                        cursorId = arrayIterator.next().Obj().getFieldDotted("cursor.id").numberLong();
                    } catch (...) {
                       // ignore any failure to create or destroy the above temporary cursors, they
                       // exist just to attempt to kill the cursors on the server for which we will
                       // not be able to return a cursor to the user
                    }
                }

                // Re-throw the users original exception...
                throw;
            }

            // Caller is responsible for cleaning up DBClientCursors
            DBClientCursor* cursor = new DBClientCursor(conn, ns.toString(), cursorId, 0, 0, 0);
            cursors->push_back(cursor);
        }
    }

    std::auto_ptr<DBClientCursor>
    DBClientBase::aggregate(const std::string& ns,
                            const BSONObj& pipeline,
                            const BSONObj* aggregateOptions,
                            int queryOptions) {
        BSONObjBuilder builder;
        bool has_cursor = false;

        builder.append("aggregate", nsToCollectionSubstring(ns));
        builder.appendArray("pipeline", pipeline);

        if (aggregateOptions) {
            builder.appendElements(*aggregateOptions);

            if (aggregateOptions->hasField("cursor")) {
                has_cursor = true;
            }
        }

        if (!has_cursor) {
            /* If the user hasn't passed a cursor field, try to add one.  This
             * will allow v2.6 servers to return a cursor, rather than an
             * array, which should be higher performance and would avoid bson
             * limits on returned sets */
            BSONObjBuilder cursor(builder.subobjStart("cursor"));
            cursor.done();
        }

        BSONObj request = builder.obj();

        auto_ptr<DBClientCursor> c = this->query(nsToDatabase(ns) + ".$cmd",
                                                 request, 1, 0, NULL, queryOptions, 0);

        if (c.get()) {
            /* we need this derived class pointer for access to the cursorid
             * specific get_cursor() method */
            DBClientCursorShimCursorID* cursor_shim;

            c->shim.reset((cursor_shim = new DBClientCursorShimCursorID(*c)));

            c->nToReturn = 0;

            if (c->rawMore()) {
                BSONObj res = cursor_shim->get_cursor();

                if (res["ok"].numberInt())
                    return c;

                if (((res["code"].numberInt() == 17020) ||
                          (res["errmsg"].String() == "unrecognized field \"cursor")) &&
                         !has_cursor) {
                    /* the typo is intentional.  The server actually returns "\"cursor"
                     *
                     * This fall back is used when we optimistically added a
                     * cursor field hoping to talk to a 2.6 server, but ended
                     * up talking to 2.4.  We return the null cursor indicating
                     * error if the user did request a cursor since we cannot
                     * provide one via 2.4 aggregation */

                    auto_ptr<DBClientCursor> simple =
                        this->query(nsToDatabase(ns) + ".$cmd", request.removeField(
                                        "cursor"), 1, 0, NULL, queryOptions, 0);

                    simple->shim.reset(new DBClientCursorShimArray(*simple, "result"));
                    simple->nToReturn = 0;

                    return simple;
                }
            }
        }

        return auto_ptr<DBClientCursor>(NULL);
    }

    auto_ptr<DBClientCursor> DBClientBase::getMore( const string &ns, long long cursorId, int nToReturn, int options ) {
        auto_ptr<DBClientCursor> c( new DBClientCursor( this, ns, cursorId, nToReturn < 0 ? abs(nToReturn) : 0, options, abs(nToReturn)) );
        if ( c->init() )
            return c;
        return auto_ptr< DBClientCursor >( 0 );
    }

    struct DBClientFunConvertor {
        void operator()( DBClientCursorBatchIterator &i ) {
            while( i.moreInCurrentBatch() ) {
                _f( i.nextSafe() );
            }
        }
        stdx::function<void(const BSONObj &)> _f;
    };

    unsigned long long DBClientBase::query( stdx::function<void(const BSONObj&)> f, const string& ns, Query query, const BSONObj *fieldsToReturn, int queryOptions ) {
        DBClientFunConvertor fun;
        fun._f = f;
        stdx::function<void(DBClientCursorBatchIterator &)> ptr( fun );
        return this->query( ptr, ns, query, fieldsToReturn, queryOptions );
    }

    unsigned long long DBClientBase::query(
            stdx::function<void(DBClientCursorBatchIterator &)> f,
            const string& ns,
            Query query,
            const BSONObj *fieldsToReturn,
            int queryOptions ) {

        // mask options
        queryOptions &= (int)( QueryOption_NoCursorTimeout | QueryOption_SlaveOk );

        auto_ptr<DBClientCursor> c( this->query(ns, query, 0, 0, fieldsToReturn, queryOptions) );
        uassert( 16090, "socket error for mapping query", c.get() );

        unsigned long long n = 0;

        while ( c->more() ) {
            DBClientCursorBatchIterator i( *c );
            f( i );
            n += i.n();
        }
        return n;
    }

    std::auto_ptr<DBClientCursor> DBClientConnection::query(const std::string &ns, Query query, int nToReturn, int nToSkip,
                                                            const BSONObj *fieldsToReturn, int queryOptions, int batchSize ) {
        checkConnection();
        return DBClientBase::query( ns, query, nToReturn, nToSkip, fieldsToReturn, queryOptions , batchSize );
    }

    unsigned long long DBClientConnection::query(
            stdx::function<void(DBClientCursorBatchIterator &)> f,
            const string& ns,
            Query query,
            const BSONObj *fieldsToReturn,
            int queryOptions ) {

        if ( ! ( availableOptions() & QueryOption_Exhaust ) ) {
            return DBClientBase::query( f, ns, query, fieldsToReturn, queryOptions );
        }

        // mask options
        queryOptions &= (int)( QueryOption_NoCursorTimeout | QueryOption_SlaveOk );
        queryOptions |= (int)QueryOption_Exhaust;

        auto_ptr<DBClientCursor> c( this->query(ns, query, 0, 0, fieldsToReturn, queryOptions) );
        uassert( 13386, "socket error for mapping query", c.get() );

        unsigned long long n = 0;

        try {
            while( 1 ) {
                while( c->moreInCurrentBatch() ) {
                    DBClientCursorBatchIterator i( *c );
                    f( i );
                    n += i.n();
                }

                if( c->getCursorId() == 0 )
                    break;

                c->exhaustReceiveMore();
            }
        }
        catch(std::exception&) {
            /* connection CANNOT be used anymore as more data may be on the way from the server.
               we have to reconnect.
               */
            _failed = true;
            p->shutdown();
            throw;
        }

        return n;
    }

    void DBClientBase::_write(
        const string& ns,
        const vector<WriteOperation*>& writes,
        bool ordered,
        const WriteConcern* writeConcern,
        WriteResult* writeResult
    ) {
        const WriteConcern* operationWriteConcern = writeConcern ? writeConcern : &getWriteConcern();

        if (getMaxWireVersion() >= 2 && operationWriteConcern->requiresConfirmation())
            _commandWriter->write( ns, writes, ordered, operationWriteConcern, writeResult );
        else
            _wireProtocolWriter->write( ns, writes, ordered, operationWriteConcern, writeResult );

    }

    namespace {
        struct ScopedWriteOperations {
            ScopedWriteOperations() { }
            ~ScopedWriteOperations() {
                vector<WriteOperation*>::const_iterator it;
                for ( it = ops.begin(); it != ops.end(); ++it )
                    delete *it;
            }
            void enqueue(WriteOperation* op) { ops.push_back(op); }
            std::vector<WriteOperation*> ops;
        };
    }

    void DBClientBase::insert( const string & ns , BSONObj obj , int flags, const WriteConcern* wc ) {
        vector<BSONObj> toInsert;
        toInsert.push_back( obj );
        insert( ns, toInsert, flags, wc );
    }

    // prefer using the bulk API for this
    void DBClientBase::insert( const string & ns, const vector< BSONObj >& v, int flags , const WriteConcern* wc ) {
        ScopedWriteOperations inserts;

        vector<BSONObj>::const_iterator bsonObjIter;
        for (bsonObjIter = v.begin(); bsonObjIter != v.end(); ++bsonObjIter) {
            uassert(0, "document to be inserted exceeds maxBsonObjectSize",
                    (*bsonObjIter).objsize() <= getMaxBsonObjectSize());
            inserts.enqueue( new InsertWriteOperation(*bsonObjIter) );
        }

        bool ordered = !(flags & InsertOption_ContinueOnError);

        WriteResult writeResult;
        _write( ns, inserts.ops, ordered, wc, &writeResult );
    }

    void DBClientBase::remove( const string & ns , Query obj , bool justOne, const WriteConcern* wc ) {
        remove( ns, obj, justOne & RemoveOption_JustOne, wc);
    }

    void DBClientBase::remove( const string & ns , Query obj , int flags, const WriteConcern* wc ) {
        ScopedWriteOperations deletes;
        uassert(0, "remove selector exceeds maxBsonObjectSize",
                obj.obj.objsize() <= getMaxBsonObjectSize());
        deletes.enqueue( new DeleteWriteOperation(obj.obj, flags) );

        WriteResult writeResult;
        _write( ns, deletes.ops, true, wc, &writeResult );
    }

    void DBClientBase::update( const string & ns , Query query , BSONObj obj , bool upsert, bool multi, const WriteConcern* wc ) {
        int flags = 0;
        if ( upsert ) flags |= UpdateOption_Upsert;
        if ( multi ) flags |= UpdateOption_Multi;
        update( ns, query, obj, flags, wc );
    }

    void DBClientBase::update( const string & ns , Query query , BSONObj obj, int flags, const WriteConcern* wc ) {
        ScopedWriteOperations updates;
        uassert(0, "update selector exceeds maxBsonObjectSize",
                query.obj.objsize() <= getMaxBsonObjectSize());
        uassert(0, "update document exceeds maxBsonObjectSize",
                obj.objsize() <= getMaxBsonObjectSize());
        updates.enqueue( new UpdateWriteOperation(query.obj, obj, flags) );

        WriteResult writeResult;
        _write( ns, updates.ops, true, wc, &writeResult );
    }

    BulkOperationBuilder DBClientBase::initializeOrderedBulkOp(const std::string& ns) {
        return BulkOperationBuilder(this, ns, true);
    }

    BulkOperationBuilder DBClientBase::initializeUnorderedBulkOp(const std::string& ns) {
        return BulkOperationBuilder(this, ns, false);
    }

    list<BSONObj> DBClientWithCommands::getIndexSpecs( const string &ns, int options ) {
        list<BSONObj> specs;

        auto_ptr<DBClientCursor> specs_cursor = enumerateIndexes(ns, options);

        if ( specs_cursor.get() ) {
            while ( specs_cursor->more() ) {
                specs.push_back(specs_cursor->nextSafe().getOwned());
            }
        }

        return specs;
    }

    list<std::string> DBClientWithCommands::getIndexNames( const std::string& ns, int options ) {
        list<std::string> indexNames;

        auto_ptr<DBClientCursor> specs_cursor = enumerateIndexes(ns, options);

        if ( specs_cursor.get() ) {
            while ( specs_cursor->more() ) {
                indexNames.push_back(specs_cursor->nextSafe()["name"].valuestr());
            }
        }

        return indexNames;
    }

    auto_ptr<DBClientCursor> DBClientWithCommands::enumerateIndexes( const string& ns,
                                                                     int options,
                                                                     int batchSize ) {
        const NamespaceString nsstring(ns);

        BSONObj cmd = BSON("listIndexes" << nsstring.coll()
            << "cursor" << (batchSize ? BSON("batchSize" << batchSize) : BSONObj()));

        auto_ptr<DBClientCursor> cursor = this->query(nsstring.getCommandNS(), cmd, 1, 0, NULL,
                                                      options, 0);

        if ( cursor.get() ) {
            BSONObj result = cursor->peekFirst();

            if ( isOk(result) ) {
                // Command worked -- we are on MongoDB 2.7.6 or above
                DBClientCursorShim* cursor_shim;

                // Select the appropriate shim for this version of MongoDB
                if ( result.hasField("indexes") ) {
                    // MongoDB 2.7.6 to 2.8.0-rc2 behavior
                    cursor_shim = new DBClientCursorShimArray(*cursor, "indexes");
                }
                else {
                    // MongoDB 2.8.0-rc3+ behavior
                    cursor_shim = new DBClientCursorShimCursorID(*cursor);
                    static_cast<DBClientCursorShimCursorID*>(cursor_shim)->get_cursor();
                }

                // Insert the shim
                cursor->shim.reset(cursor_shim);
                cursor->nToReturn = 0;
                cursor->setBatchSize(batchSize);
            }
            else {
                // Command failed -- we are either on an older MongoDB or something else happened
                int error_code = result["code"].numberInt();
                string errmsg = result["errmsg"].valuestrsafe();

                if ( error_code == ErrorCodes::NamespaceNotFound ) {
                    cursor.reset(NULL);
                }
                else if (
                    ( error_code == ErrorCodes::CommandNotFound ) ||
                    ( error_code == 13390 ) ||
                    ( errmsg.find( "no such cmd" ) != string::npos )
                ) {
                    // MongoDB < 2.7.6 behavior -- query system.indexes
                    cursor.reset(query(nsstring.getSystemIndexesCollection(), BSON("ns" << ns),
                                       0, 0, 0, options, batchSize).release());
                }
                else {
                    // Something else happened, uassert with the reason
                    uasserted( 18631, str::stream() << "listIndexes failed: " << result );
                }
            }
        }

        return cursor;
    }


    void DBClientWithCommands::dropIndex( const string& ns , BSONObj keys ) {
        dropIndex( ns , genIndexName( keys ) );
    }


    void DBClientWithCommands::dropIndex( const string& ns , const string& indexName ) {
        BSONObj info;
        if ( ! runCommand( nsToDatabase( ns ) ,
                           BSON( "deleteIndexes" << nsToCollectionSubstring(ns) << "index" << indexName ) ,
                           info ) ) {
            LOG(_logLevel) << "dropIndex failed: " << info << endl;
            uassert( 10007 ,  "dropIndex failed" , 0 );
        }
    }

    void DBClientWithCommands::dropIndexes( const string& ns ) {
        BSONObj info;
        uassert( 10008,
                 "dropIndexes failed",
                 runCommand( nsToDatabase( ns ),
                             BSON( "deleteIndexes" << nsToCollectionSubstring(ns) << "index" << "*"),
                             info )
                 );
    }

    void DBClientWithCommands::reIndex( const string& ns ) {
        BSONObj info;
        uassert( 0,
                 "reIndex failed",
                 runCommand( nsToDatabase( ns ),
                             BSON( "reIndex" << nsToCollectionSubstring(ns)),
                             info )
               );
    }


    string DBClientWithCommands::genIndexName( const BSONObj& keys ) {
        stringstream ss;

        bool first = 1;
        for ( BSONObjIterator i(keys); i.more(); ) {
            BSONElement f = i.next();

            if ( first )
                first = 0;
            else
                ss << "_";

            ss << f.fieldName() << "_";
            if( f.isNumber() )
                ss << f.numberInt();
            else
                ss << f.str(); //this should match up with shell command
        }
        return ss.str();
    }

    void DBClientWithCommands::createIndex(const StringData& ns, const IndexSpec& descriptor) {

        const BSONObj descriptorObj = descriptor.toBSON();

        BSONObjBuilder command;
        command.append("createIndexes", nsToCollectionSubstring(ns));
        {
            BSONArrayBuilder indexes(command.subarrayStart("indexes"));
            indexes.append(descriptorObj);
        }
        const BSONObj commandObj = command.done();

        BSONObj infoObj;
        if (!runCommand(nsToDatabase(ns), commandObj, infoObj)) {

            // If runCommand failed either by returning no code or saying that the command is
            // not supported, then fall back to inserting into the system.indexes collection.

            if (!infoObj.hasField("code") ||
                (infoObj["code"].numberInt() == ErrorCodes::CommandNotFound)) {

                BSONObjBuilder insertCommand;
                insertCommand.append("ns", ns.toString());
                insertCommand.appendElements(descriptorObj);

                return insert(
                    nsToDatabase(ns) + ".system.indexes",
                    insertCommand.obj());
            }

            // Some other sort of error occurred; raise.
            throw OperationException(infoObj);
        }
    }

    /* -- DBClientCursor ---------------------------------------------- */

#ifdef MONGO_DEBUG_BUILD
#define CHECK_OBJECT( o , msg ) massert( 10337 ,  (string)"object not valid" + (msg) , (o).isValid() )
#else
#define CHECK_OBJECT( o , msg )
#endif

    void assembleRequest( const string &ns, BSONObj query, int nToReturn, int nToSkip, const BSONObj *fieldsToReturn, int queryOptions, Message &toSend ) {
        CHECK_OBJECT( query , "assembleRequest query" );
        // see query.h for the protocol we are using here.
        BufBuilder b;
        int opts = queryOptions;
        b.appendNum(opts);
        b.appendStr(ns);
        b.appendNum(nToSkip);
        b.appendNum(nToReturn);
        query.appendSelfToBufBuilder(b);
        if ( fieldsToReturn )
            fieldsToReturn->appendSelfToBufBuilder(b);
        toSend.setData(dbQuery, b.buf(), b.len());
    }

    DBClientConnection::DBClientConnection(bool _autoReconnect, DBClientReplicaSet*, double so_timeout):
                    _failed(false),
                    autoReconnect(_autoReconnect),
                    autoReconnectBackoff(1000, 2000),
                    _so_timeout(so_timeout) {
        _numConnections.fetchAndAdd(1);
    }

    void DBClientConnection::say( Message &toSend, bool isRetry , string * actualServer ) {
        checkConnection();
        try {
            port().say( toSend );
        }
        catch( SocketException & ) {
            _failed = true;
            throw;
        }
    }

    void DBClientConnection::sayPiggyBack( Message &toSend ) {
        port().piggyBack( toSend );
    }

    bool DBClientConnection::recv( Message &m ) {
        if (port().recv(m)) {
            return true;
        }

        _failed = true;
        return false;
    }

    bool DBClientConnection::call( Message &toSend, Message &response, bool assertOk , string * actualServer ) {
        /* todo: this is very ugly messagingport::call returns an error code AND can throw
                 an exception.  we should make it return void and just throw an exception anytime
                 it fails
        */
        checkConnection();
        try {
            if ( !port().call(toSend, response) ) {
                _failed = true;
                if ( assertOk )
                    uasserted( 10278 , str::stream() << "dbclient error communicating with server: " << getServerAddress() );

                return false;
            }
        }
        catch( SocketException & ) {
            _failed = true;
            throw;
        }
        return true;
    }

    BSONElement getErrField(const BSONObj& o) {
        BSONElement first = o.firstElement();
        if( strcmp(first.fieldName(), "$err") == 0 )
            return first;

        // temp - will be DEV only later
        /*DEV*/ 
        if( 1 ) {
            BSONElement e = o["$err"];
            if( !e.eoo() ) { 
                wassert(false);
            }
            return e;
        }

        return BSONElement();
    }

    bool hasErrField( const BSONObj& o ){
        return ! getErrField( o ).eoo();
    }

    void DBClientConnection::checkResponse( const char *data, int nReturned, bool* retry, string* host ) {
        /* check for errors.  the only one we really care about at
         * this stage is "not master" 
        */
        
        *retry = false;
        *host = _serverString;

        if (!_parentReplSetName.empty() && nReturned) {
            verify(data);
            BSONObj bsonView(data);
            handleNotMasterResponse(getErrField(bsonView));
        }
    }

    void DBClientConnection::killCursor( long long cursorId ) {
        StackBufBuilder b;
        b.appendNum( (int)0 ); // reserved
        b.appendNum( (int)1 ); // number
        b.appendNum( cursorId );

        Message m;
        m.setData( dbKillCursors , b.buf() , b.len() );
        
        if ( _lazyKillCursor )
            sayPiggyBack( m );
        else
            say(m);
    }

    void DBClientConnection::setParentReplSetName(const string& replSetName) {
        _parentReplSetName = replSetName;
    }

    void DBClientConnection::handleNotMasterResponse(const BSONElement& elemToCheck) {
        if (!isNotMasterErrorString(elemToCheck)) {
            return;
        }

        MONGO_LOG_COMPONENT(1, logger::LogComponent::kReplication)
            << "got not master from: " << _serverString
            << " of repl set: " << _parentReplSetName;

        ReplicaSetMonitorPtr monitor = ReplicaSetMonitor::get(_parentReplSetName);
        if (monitor) {
            monitor->failedHost(_server);
        }

        _failed = true;
    }

#ifdef MONGO_SSL
    static boost::mutex s_mtx;
    static SSLManagerInterface* s_sslMgr(NULL);

    SSLManagerInterface* DBClientConnection::sslManager() {
        boost::lock_guard<boost::mutex> lk(s_mtx);
        if (s_sslMgr) 
            return s_sslMgr;
        s_sslMgr = getSSLManager();
        
        return s_sslMgr;
    }
#endif

    AtomicInt32 DBClientConnection::_numConnections;
    bool DBClientConnection::_lazyKillCursor = true;


    bool serverAlive( const string &uri ) {
        DBClientConnection c( false, 0, 20 ); // potentially the connection to server could fail while we're checking if it's alive - so use timeouts
        string err;
        if ( !c.connect( HostAndPort(uri), err ) )
            return false;
        if ( !c.simpleCommand( "admin", 0, "ping" ) )
            return false;
        return true;
    }


    /** @return the database name portion of an ns string */
    string nsGetDB( const string &ns ) {
        string::size_type pos = ns.find( "." );
        if ( pos == string::npos )
            return ns;

        return ns.substr( 0 , pos );
    }

    /** @return the collection name portion of an ns string */
    string nsGetCollection( const string &ns ) {
        string::size_type pos = ns.find( "." );
        if ( pos == string::npos )
            return "";

        return ns.substr( pos + 1 );
    }


} // namespace mongo
