/** @file dbclientinterface.h

    Core MongoDB C++ driver interfaces are defined here.
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

#pragma once

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

#include "mongo/config.h"

#include "mongo/base/string_data.h"
#include "mongo/client/bulk_operation_builder.h"
#include "mongo/client/exceptions.h"
#include "mongo/client/export_macros.h"
#include "mongo/client/index_spec.h"
#include "mongo/client/write_concern.h"
#include "mongo/client/write_options.h"
#include "mongo/db/jsobj.h"
#include "mongo/logger/log_severity.h"
#include "mongo/platform/atomic_word.h"
#include "mongo/stdx/functional.h"
#include "mongo/util/mongoutils/str.h"
#include "mongo/util/net/message.h"
#include "mongo/util/net/message_port.h"

namespace mongo {

    /** the query field 'options' can have these bits set: */
    enum MONGO_CLIENT_API QueryOptions {
        /** Tailable means cursor is not closed when the last data is retrieved.  rather, the cursor marks
           the final object's position.  you can resume using the cursor later, from where it was located,
           if more data were received.  Set on dbQuery and dbGetMore.

           like any "latent cursor", the cursor may become invalid at some point -- for example if that
           final object it references were deleted.  Thus, you should be prepared to requery if you get back
           ResultFlag_CursorNotFound.
        */
        QueryOption_CursorTailable = 1 << 1,

        /** allow query of replica slave.  normally these return an error except for namespace "local".
        */
        QueryOption_SlaveOk = 1 << 2,

        // findingStart mode is used to find the first operation of interest when
        // we are scanning through a repl log.  For efficiency in the common case,
        // where the first operation of interest is closer to the tail than the head,
        // we start from the tail of the log and work backwards until we find the
        // first operation of interest.  Then we scan forward from that first operation,
        // actually returning results to the client.  During the findingStart phase,
        // we release the db mutex occasionally to avoid blocking the db process for
        // an extended period of time.
        QueryOption_OplogReplay = 1 << 3,

        /** The server normally times out idle cursors after an inactivity period to prevent excess memory uses
            Set this option to prevent that.
        */
        QueryOption_NoCursorTimeout = 1 << 4,

        /** Use with QueryOption_CursorTailable.  If we are at the end of the data, block for a while rather
            than returning no data. After a timeout period, we do return as normal.
        */
        QueryOption_AwaitData = 1 << 5,

        /** Stream the data down full blast in multiple "more" packages, on the assumption that the client
            will fully read all data queried.  Faster when you are pulling a lot of data and know you want to
            pull it all down.  Note: it is not allowed to not read all the data unless you close the connection.

            Use the query( stdx::function<void(const BSONObj&)> f, ... ) version of the connection's query()
            method, and it will take care of all the details for you.
        */
        QueryOption_Exhaust = 1 << 6,

        /** When sharded, this means its ok to return partial results
            Usually we will fail a query if all required shards aren't up
            If this is set, it'll be a partial result set
         */
        QueryOption_PartialResults = 1 << 7 ,

        QueryOption_AllSupported = QueryOption_CursorTailable |
            QueryOption_SlaveOk |
            QueryOption_OplogReplay |
            QueryOption_NoCursorTimeout |
            QueryOption_AwaitData |
            QueryOption_Exhaust |
            QueryOption_PartialResults,
    };

    /**
     * Start from *top* of bits, these are generic write options that apply to all
     */
    enum MONGO_CLIENT_API WriteOptions {
        /** logical writeback option */
        WriteOption_FromWriteback = 1 << 31
    };

    //
    // For legacy reasons, the reserved field pre-namespace of certain types of messages is used
    // to store options as opposed to the flags after the namespace.  This should be transparent to
    // the api user, but we need these constants to disassemble/reassemble the messages correctly.
    //

    enum MONGO_CLIENT_API ReservedOptions {
        Reserved_InsertOption_ContinueOnError = 1 << 0 ,
        Reserved_FromWriteback = 1 << 1
    };

    enum MONGO_CLIENT_API ReadPreference {
        /**
         * Read from primary only. All operations produce an error (throw an
         * exception where applicable) if primary is unavailable. Cannot be
         * combined with tags.
         */
        ReadPreference_PrimaryOnly = 0,

        /**
         * Read from primary if available, otherwise a secondary. Tags will
         * only be applied in the event that the primary is unavailable and
         * a secondary is read from. In this event only secondaries matching
         * the tags provided would be read from.
         */
        ReadPreference_PrimaryPreferred,

        /**
         * Read from secondary if available, otherwise error.
         */
        ReadPreference_SecondaryOnly,

        /**
         * Read from a secondary if available, otherwise read from the primary.
         */
        ReadPreference_SecondaryPreferred,

        /**
         * Read from any member.
         */
        ReadPreference_Nearest,
    };

    class MONGO_CLIENT_API DBClientBase;
    class MONGO_CLIENT_API DBClientConnection;

    /**
     * ConnectionString can parse MongoDB URIs with the following format:
     *
     *    mongodb://[usr:pwd@]host1[:port1]...[,hostN[:portN]]][/[db][?options]]
     *
     * For a complete list of URI string options, see
     * https://wiki.mongodb.com/display/DH/Connection+String+Format
     *
     * Examples:
     *
     *    A replica set with three members (one running on default port 27017):
     *      string uri = mongodb://localhost,localhost:27018,localhost:27019
     *
     *    Authenticated connection to db 'bedrock' with user 'barney' and pwd 'rubble':
     *      string url = mongodb://barney:rubble@localhost/bedrock
     *
     *    Use parse() to parse the url, then validate and connect:
     *      string errmsg;
     *      ConnectionString cs = ConnectionString::parse( url, errmsg );
     *      if ( ! cs.isValid() ) throw "bad connection string: " + errmsg;
     *      DBClientBase * conn = cs.connect( errmsg );
     *
     * NOTE:
     *
     *    The 'rs_name/host1:port,host2:port' format has been deprecated, and parse()
     *    will no longer recognize this as a valid URI. To use the deprecated format,
     *    use parseDeprecated() instead.
     */
    class MONGO_CLIENT_API ConnectionString {
    public:

        enum ConnectionType { INVALID , MASTER , PAIR , SET , CUSTOM };

        ConnectionString() {
            _type = INVALID;
        }

        // Note: This should only be used for direct connections to a single server.  For replica
        // set and SyncClusterConnections, use ConnectionString::parse.
        ConnectionString( const HostAndPort& server ) {
            _type = MASTER;
            _servers.push_back( server );
            _finishInit();
        }

        ConnectionString( ConnectionType type , const std::string& s , const std::string& setName = "" ) {
            _type = type;
            _setName = setName;
            _fillServers( s );

            switch ( _type ) {
            case MASTER:
                verify( _servers.size() == 1 );
                break;
            case SET:
                verify( _setName.size() );
                verify( _servers.size() >= 1 ); // 1 is ok since we can derive
                break;
            case PAIR:
                verify( _servers.size() == 2 );
                break;
            default:
                verify( _servers.size() > 0 );
            }

            _finishInit();
        }

        ConnectionString( const std::string& s , ConnectionType favoredMultipleType ) {
            _type = INVALID;

            _fillServers( s );
            if ( _type != INVALID ) {
                // set already
            }
            else if ( _servers.size() == 1 ) {
                _type = MASTER;
            }
            else {
                _type = favoredMultipleType;
                verify( _type == SET );
            }
            _finishInit();
        }

        bool isValid() const { return _type != INVALID; }

        std::string toString() const { return _string; }

        DBClientBase* connect( std::string& errmsg, double socketTimeout = 0 ) const;

        std::string getSetName() const { return _setName; }

        const std::vector<HostAndPort>& getServers() const { return _servers; }

        ConnectionType type() const { return _type; }

        const std::string& getUser() const { return _user; }

        const std::string& getPassword() const { return _password; }

        const BSONObj& getOptions() const { return _options; }

        const std::string& getDatabase() const { return _database; }

        /**
         * This returns true if this and other point to the same logical entity.
         * For single nodes, thats the same address.
         * For replica sets, thats just the same replica set name.
         * For pair (deprecated) or sync cluster connections, that's the same hosts in any ordering.
         */
        bool sameLogicalEndpoint( const ConnectionString& other ) const;

        static ConnectionString MONGO_CLIENT_FUNC parse( const std::string& address , std::string& errmsg );

        static ConnectionString MONGO_CLIENT_FUNC parseDeprecated( const std::string& address , std::string& errmsg );

        static std::string MONGO_CLIENT_FUNC typeToString( ConnectionType type );

        //
        // Allow overriding the default connection behavior
        // This is needed for some tests, which otherwise would fail because they are unable to contact
        // the correct servers.
        //

        class ConnectionHook {
        public:
            virtual ~ConnectionHook(){}

            // Returns an alternative connection object for a string
            virtual DBClientBase* connect( const ConnectionString& c,
                                             std::string& errmsg,
                                             double socketTimeout ) = 0;
        };

        static void setConnectionHook( ConnectionHook* hook ){
            boost::lock_guard<boost::mutex> lk( _connectHookMutex );
            _connectHook = hook;
        }

        static ConnectionHook* getConnectionHook() {
            boost::lock_guard<boost::mutex> lk( _connectHookMutex );
            return _connectHook;
        }

        // Allows ConnectionStrings to be stored more easily in sets/maps
        bool operator<(const ConnectionString& other) const {
            return _string < other._string;
        }

        //
        // FOR TESTING ONLY - useful to be able to directly mock a connection string without
        // including the entire client library.
        //

        static ConnectionString mock( const HostAndPort& server ) {
            ConnectionString connStr;
            connStr._servers.push_back( server );
            connStr._string = server.toString();
            return connStr;
        }

    private:
        ConnectionString( ConnectionType type,
                          const std::string& user,
                          const std::string& password,
                          const std::string& servers,
                          const std::string& database,
                          const std::string& setName,
                          const BSONObj& options )
            : _type( type )
            , _servers( )
            , _setName( setName )
            , _user( user )
            , _password( password )
            , _database( database )
            , _options( options ) {

            _fillServers( servers, false );
            switch ( _type ) {
            case MASTER:
                verify( _servers.size() == 1 );
                break;
            case SET:
                verify( _setName.size() );
                verify( _servers.size() >= 1 ); // 1 is ok since we can derive
                break;
            case PAIR:
                verify( _servers.size() == 2 );
                break;
            default:
                verify( _servers.size() > 0 );
            }

            _finishInit();
        }

        static ConnectionString _parseURL( const std::string& url, std::string& errmsg );

        void _fillServers( std::string s, bool legacy = true );
        void _finishInit();

        BSONObj _makeAuthObjFromOptions(int maxWireVersion) const;

        ConnectionType _type;
        std::vector<HostAndPort> _servers;
        std::string _string;
        std::string _setName;

        std::string _user;
        std::string _password;

        std::string _database;
        BSONObj _options;

        static boost::mutex _connectHookMutex;
        static ConnectionHook* _connectHook;
    };

    class BSONObj;
    class DBClientCursor;
    class DBClientCursorBatchIterator;

    /** Represents a Mongo query expression.  Typically one uses the MONGO_QUERY(...) macro to construct a Query object.
        Examples:
           MONGO_QUERY( "age" << 33 << "school" << "UCLA" ).sort("name")
           MONGO_QUERY( "age" << GT << 30 << LT << 50 )
    */
    class MONGO_CLIENT_API Query {
    public:
        static const BSONField<BSONObj> ReadPrefField;
        static const BSONField<std::string> ReadPrefModeField;
        static const BSONField<BSONArray> ReadPrefTagsField;

        BSONObj obj;
        Query() : obj(BSONObj()) { }
        Query(const BSONObj& b) : obj(b) { }
        Query(const std::string &json);
        Query(const char * json);

        /** Add a sort (ORDER BY) criteria to the query expression.
            @param sortPattern the sort order template.  For example to order by name ascending, time descending:
              { name : 1, ts : -1 }
            i.e.
              BSON( "name" << 1 << "ts" << -1 )
            or
              fromjson(" name : 1, ts : -1 ")
        */
        Query& sort(const BSONObj& sortPattern);

        /** Add a sort (ORDER BY) criteria to the query expression.
            This version of sort() assumes you want to sort on a single field.
            @param asc = 1 for ascending order
            asc = -1 for descending order
        */
        Query& sort(const std::string &field, int asc = 1) { sort( BSON( field << asc ) ); return *this; }

        /** Provide a hint to the query.
            @param keyPattern Key pattern for the index to use.
            Example:
              hint("{ts:1}")
        */
        Query& hint(BSONObj keyPattern);
        Query& hint(const std::string& indexName);

        /**
         * Specifies a cumulative time limit in milliseconds for processing an operation.
         * MongoDB will interrupt the operation at the earliest following interrupt point.
         */
        Query& maxTimeMs(int millis);

        /** Provide min and/or max index limits for the query.
            min <= x < max
         */
        Query& minKey(const BSONObj &val);
        /**
           max is exclusive
         */
        Query& maxKey(const BSONObj &val);

        /** Return explain information about execution of this query instead of the actual query results.
            Normally it is easier to use the mongo shell to run db.find(...).explain().
        */
        Query& explain();

        /** Use snapshot mode for the query.  Snapshot mode assures no duplicates are returned, or objects missed, which were
            present at both the start and end of the query's execution (if an object is new during the query, or deleted during
            the query, it may or may not be returned, even with snapshot mode).

            Note that short query responses (less than 1MB) are always effectively snapshotted.

            Currently, snapshot mode may not be used with sorting or explicit hints.
        */
        Query& snapshot();

        /** Queries to the Mongo database support a $where parameter option which contains
            a javascript function that is evaluated to see whether objects being queried match
            its criteria.  Use this helper to append such a function to a query object.
            Your query may also contain other traditional Mongo query terms.

            @param jscode The javascript function to evaluate against each potential object
                   match.  The function must return true for matched objects.  Use the this
                   variable to inspect the current object.
            @param scope SavedContext for the javascript object.  List in a BSON object any
                   variables you would like defined when the jscode executes.  One can think
                   of these as "bind variables".

            Examples:
              conn.findOne("test.coll", Query("{a:3}").where("this.b == 2 || this.c == 3"));
              Query badBalance = Query().where("this.debits - this.credits < 0");
        */
        Query& where(const std::string &jscode, BSONObj scope);
        Query& where(const std::string &jscode) { return where(jscode, BSONObj()); }

        /**
         * Sets the read preference for this query.
         *
         * @param pref the read preference mode for this query.
         * @param tags the set of tags to use for this query.
         */
        Query& readPref(ReadPreference pref, const BSONArray& tags);

        /**
         * @return true if this query has an orderby, hint, or some other field
         */
        bool isComplex( bool * hasDollar = 0 ) const;
        BSONObj getModifiers() const;
        static bool MONGO_CLIENT_FUNC isComplex(const BSONObj& obj, bool* hasDollar = 0);

        BSONObj getFilter() const;
        BSONObj getSort() const;
        BSONElement getHint() const;
        BSONObj getReadPref() const;
        int getMaxTimeMs() const;
        bool isExplain() const;

        /**
         * @return true if the query object contains a read preference specification object.
         */
        static bool MONGO_CLIENT_FUNC hasReadPreference(const BSONObj& queryObj);
        bool hasReadPreference() const;
        bool hasHint() const;
        bool hasMaxTimeMs() const;

        std::string toString() const;
        operator std::string() const { return toString(); }
    private:
        void makeComplex();
        template< class T >
        void appendComplex( const char *fieldName, const T& val ) {
            makeComplex();
            BSONObjBuilder b;
            b.appendElements(obj);
            b.append(fieldName, val);
            obj = b.obj();
        }
    };

    /**
     * Represents a full query description, including all options required for the query to be passed on
     * to other hosts
     */
    class MONGO_CLIENT_API QuerySpec {

        std::string _ns;
        int _ntoskip;
        int _ntoreturn;
        int _options;
        BSONObj _query;
        BSONObj _fields;
        Query _queryObj;

    public:

        QuerySpec( const std::string& ns,
                   const BSONObj& query, const BSONObj& fields,
                   int ntoskip, int ntoreturn, int options )
            : _ns( ns ), _ntoskip( ntoskip ), _ntoreturn( ntoreturn ), _options( options ),
              _query( query.getOwned() ), _fields( fields.getOwned() ) , _queryObj( _query ) {
        }

        QuerySpec() {}

        bool isEmpty() const { return _ns.size() == 0; }

        bool isExplain() const { return _queryObj.isExplain(); }
        BSONObj filter() const { return _queryObj.getFilter(); }

        BSONElement hint() const { return _queryObj.getHint(); }
        BSONObj sort() const { return _queryObj.getSort(); }
        BSONObj query() const { return _query; }
        BSONObj fields() const { return _fields; }
        BSONObj* fieldsData() { return &_fields; }

        // don't love this, but needed downstrem
        const BSONObj* fieldsPtr() const { return &_fields; }

        std::string ns() const { return _ns; }
        int ntoskip() const { return _ntoskip; }
        int ntoreturn() const { return _ntoreturn; }
        int options() const { return _options; }

        void setFields( BSONObj& o ) { _fields = o.getOwned(); }

        std::string toString() const {
            return str::stream() << "QSpec " <<
                BSON( "ns" << _ns << "n2skip" << _ntoskip << "n2return" << _ntoreturn << "options" << _options
                      << "query" << _query << "fields" << _fields );
        }

    };


    /** Typically one uses the MONGO_QUERY(...) macro to construct a Query object.
        Example: MONGO_QUERY( "age" << 33 << "school" << "UCLA" )
    */
#define MONGO_QUERY(x) ::mongo::Query( BSON(x) )

    // Useful utilities for namespaces
    /** @return the database name portion of an ns string */
    MONGO_CLIENT_API std::string MONGO_CLIENT_FUNC nsGetDB( const std::string &ns );

    /** @return the collection name portion of an ns string */
    MONGO_CLIENT_API std::string MONGO_CLIENT_FUNC nsGetCollection( const std::string &ns );

    /**
       interface that handles communication with the db
     */
    class MONGO_CLIENT_API DBConnector {
    public:
        virtual ~DBConnector() {}
        /** actualServer is set to the actual server where they call went if there was a choice (SlaveOk) */
        virtual bool call( Message &toSend, Message &response, bool assertOk=true , std::string * actualServer = 0 ) = 0;
        virtual void say( Message &toSend, bool isRetry = false , std::string * actualServer = 0 ) = 0;
        virtual void sayPiggyBack( Message &toSend ) = 0;
        /* used by QueryOption_Exhaust.  To use that your subclass must implement this. */
        virtual bool recv( Message& m ) { verify(false); return false; }
        // In general, for lazy queries, we'll need to say, recv, then checkResponse
        virtual void checkResponse( const char* data, int nReturned, bool* retry = NULL, std::string* targetHost = NULL ) {
            if( retry ) *retry = false; if( targetHost ) *targetHost = "";
        }
        virtual bool lazySupported() const = 0;
    };

    /**
       The interface that any db connection should implement
     */
    class MONGO_CLIENT_API DBClientInterface : boost::noncopyable {
    public:
        virtual std::auto_ptr<DBClientCursor> query(const std::string &ns, Query query, int nToReturn = 0, int nToSkip = 0,
                                                    const BSONObj *fieldsToReturn = 0, int queryOptions = 0 , int batchSize = 0 ) = 0;

        virtual void insert( const std::string &ns, BSONObj obj , int flags=0, const WriteConcern* wc=NULL ) = 0;

        virtual void insert( const std::string &ns, const std::vector< BSONObj >& v , int flags=0, const WriteConcern* wc=NULL ) = 0;

        virtual void remove( const std::string &ns, Query query, bool justOne = false, const WriteConcern* wc=NULL ) = 0;

        virtual void remove( const std::string &ns, Query query, int flags, const WriteConcern* wc=NULL ) = 0;

        virtual void update( const std::string &ns,
                             Query query,
                             BSONObj obj,
                             bool upsert = false, bool multi = false, const WriteConcern* wc=NULL ) = 0;

        virtual void update( const std::string &ns, Query query, BSONObj obj, int flags, const WriteConcern* wc=NULL ) = 0;

        virtual ~DBClientInterface() { }

        /**
           @return a single object that matches the query.  if none do, then the object is empty
           @throws AssertionException
        */
        virtual BSONObj findOne(const std::string &ns, const Query& query, const BSONObj *fieldsToReturn = 0, int queryOptions = 0);

        /** query N objects from the database into an array.  makes sense mostly when you want a small number of results.  if a huge number, use
            query() and iterate the cursor.
        */
        void findN(std::vector<BSONObj>& out, const std::string&ns, Query query, int nToReturn, int nToSkip = 0, const BSONObj *fieldsToReturn = 0, int queryOptions = 0, int batchSize = 0);

        /**
         * Saves a document to a collection.
         *
         * If 'toSave' already has an '_id' then an update(upsert) operation is performed and
         * any existing document with that '_id' is overwritten. Otherwise, an insert operation
         * is performed.
         *
         * @param ns The namespace to save the document into.
         * @param toSave The document to save.
         * @param wc The write concern for this operation.
         */
        void save(const StringData& ns, const BSONObj& toSave, const WriteConcern* wc = NULL);

        virtual std::string getServerAddress() const = 0;

        /** don't use this - called automatically by DBClientCursor for you */
        virtual std::auto_ptr<DBClientCursor> getMore( const std::string &ns, long long cursorId, int nToReturn = 0, int options = 0 ) = 0;
    };

    /**
       DB "commands"
       Basically just invocations of connection.$cmd.findOne({...});
    */
    class MONGO_CLIENT_API DBClientWithCommands : public DBClientInterface {
    public:
        /** controls how chatty the client is about network errors & such.  See log.h */
        logger::LogSeverity _logLevel;

        DBClientWithCommands() : _logLevel(logger::LogSeverity::Log()),
                _minWireVersion(0),
                _maxWireVersion(0),
                _cachedAvailableOptions( (enum QueryOptions)0 ),
                _haveCachedAvailableOptions(false) { }

        /** helper function.  run a simple command where the command expression is simply
              { command : 1 }
            @param info -- where to put result object.  may be null if caller doesn't need that info
            @param command -- command name
            @return true if the command returned "ok".
         */
        bool simpleCommand(const std::string &dbname, BSONObj *info, const std::string &command);

        /** Run a database command.  Database commands are represented as BSON objects.  Common database
            commands have prebuilt helper functions -- see below.  If a helper is not available you can
            directly call runCommand.

            @param dbname database name.  Use "admin" for global administrative commands.
            @param cmd  the command object to execute.  For example, { ismaster : 1 }
            @param info the result object the database returns. Typically has { ok : ..., errmsg : ... } fields
                   set.
            @param options see enum QueryOptions - normally not needed to run a command
            @param auth if set, the BSONObj representation will be appended to the command object sent

            @return true if the command returned "ok".
        */
        virtual bool runCommand(const std::string &dbname, const BSONObj& cmd, BSONObj &info,
                                int options=0);

        /**
         * Authenticate a user.
         *
         * The "params" BSONObj should be initialized with some of the fields below.  Which fields
         * are required depends on the mechanism, which is mandatory.
         *
         *     "mechanism": The string name of the sasl mechanism to use.  Mandatory.
         *     "user": The string name of the user to authenticate.  Mandatory.
         *     "db": The database target of the auth command, which identifies the location
         *         of the credential information for the user.  May be "$external" if
         *         credential information is stored outside of the mongo cluster.  Mandatory.
         *     "pwd": The password data.
         *     "digestPassword": Boolean, set to true if the "pwd" is undigested (default).
         *     "serviceName": The GSSAPI service name to use.  Defaults to "mongodb".
         *     "serviceHostname": The GSSAPI hostname to use.  Defaults to the name of the remote
         *          host.
         *
         * Other fields in "params" are silently ignored.
         *
         * Returns normally on success, and throws on error.  Throws a DBException with getCode() ==
         * ErrorCodes::AuthenticationFailed if authentication is rejected.  All other exceptions are
         * tantamount to authentication failure, but may also indicate more serious problems.
         */
        void auth(const BSONObj& params);

        /** Authorize access to a particular database.
            Authentication is separate for each database on the server -- you may authenticate for any
            number of databases on a single connection.
            @param      digestPassword  if password is plain text, set this to true.  otherwise assumed to be pre-digested
            @param[out] authLevel       level of authentication for the given user
            @return true if successful
        */
        bool auth(const std::string &dbname, const std::string &username, const std::string &pwd, std::string& errmsg, bool digestPassword = true);

        /**
         * Logs out the connection for the given database.
         *
         * @param dbname the database to logout from.
         * @param info the result object for the logout command (provided for backwards
         *     compatibility with mongo shell)
         */
        virtual void logout(const std::string& dbname, BSONObj& info);

        /** count number of objects in collection ns that match the query criteria specified
            throws UserAssertion if database returns an error
        */
        virtual unsigned long long count(const std::string &ns, const Query& query = Query(), int options=0, int limit=0, int skip=0 );

        static std::string MONGO_CLIENT_FUNC createPasswordDigest(const std::string &username, const std::string &clearTextPassword);

        /** returns true in isMaster parm if this db is the current master
           of a replica pair.

           pass in info for more details e.g.:
             { "ismaster" : 1.0 , "msg" : "not paired" , "ok" : 1.0  }

           returns true if command invoked successfully.
        */
        virtual bool isMaster(bool& isMaster, BSONObj *info=0);

        /**
           Create a new collection in the database.  Normally, collection creation is automatic.  You would
           use this function if you wish to specify special options on creation.

           If the collection already exists, no action occurs.

           @param ns     fully qualified collection name
           @param size   desired initial extent size for the collection.
                         Must be <= 1000000000 for normal collections.
                         For fixed size (capped) collections, this size is the total/max size of the
                         collection.
           @param capped if true, this is a fixed size collection (where old data rolls out).
           @param max    maximum number of objects if capped (optional).

           @return true if successful.
        */
        bool createCollection(
            const std::string &ns,
            long long size = 0,
            bool capped = false,
            int max = 0,
            BSONObj *info = 0
        );

        /**
         * Creates a new collection in the database. Allows for a user to provide a BSONObj that
         * contains extra options.
         *
         * @param extraOptions Add extra parameters to the create collection command for features
         *  that are version specific or for which default values have flipped between server
         *  releases. Some examples are "usePowerOf2Sizes" and "autoIndexId".
         *
         * @warning Options set in extraOptions which shadow those passed in as parameters will
         *  have indeterminate behavior.
         *
         * @see the form of createCollection with less parameters above.
         *
         * @see http://docs.mongodb.org/manual/reference/command/create/#dbcmd.create for available
         * options.
         */
        bool createCollectionWithOptions(
            const std::string &ns,
            long long size = 0,
            bool capped = false,
            int max = 0,
            const BSONObj& extraOptions = BSONObj(),
            BSONObj *info = 0
        );

        /** Get error result from the last write operation (insert/update/delete) on this connection.
            db doesn't change the command's behavior - it is just for auth checks.
            @return error message text, or empty string if no error.
        */
        std::string getLastError(const std::string& db,
                                 bool fsync = false,
                                 bool j = false,
                                 int w = 0,
                                 int wtimeout = 0);
        /**
         * Same as the form of getLastError that takes a dbname, but just uses the admin DB.
         */
        std::string getLastError(bool fsync = false, bool j = false, int w = 0, int wtimeout = 0);

        /** Get error result from the last write operation (insert/update/delete) on this connection.
            db doesn't change the command's behavior - it is just for auth checks.
            @return full error object.

            If "w" is -1, wait for propagation to majority of nodes.
            If "wtimeout" is 0, the operation will block indefinitely if needed.
        */
        virtual BSONObj getLastErrorDetailed(const std::string& db,
                                             bool fsync = false,
                                             bool j = false,
                                             int w = 0,
                                             int wtimeout = 0);
        /**
         * Same as the form of getLastErrorDetailed that takes a dbname, but just uses the admin DB.
         */
        virtual BSONObj getLastErrorDetailed(bool fsync = false, bool j = false, int w = 0, int wtimeout = 0);

        /** Can be called with the returned value from getLastErrorDetailed to extract an error string.
            If all you need is the string, just call getLastError() instead.
        */
        static std::string MONGO_CLIENT_FUNC getLastErrorString( const BSONObj& res );

        /** Return the last error which has occurred, even if not the very last operation.

           @return { err : <error message>, nPrev : <how_many_ops_back_occurred>, ok : 1 }

           result.err will be null if no error has occurred.
        */
        BSONObj getPrevError();

        /** Reset the previous error state for this connection (accessed via getLastError and
            getPrevError).  Useful when performing several operations at once and then checking
            for an error after attempting all operations.
        */
        bool resetError() { return simpleCommand("admin", 0, "reseterror"); }

        /** Delete the specified collection.
         *  @param info An optional output parameter that receives the result object the database
         *  returns from the drop command.  May be null if the caller doesn't need that info.
         */
        virtual bool dropCollection( const std::string &ns, BSONObj* info = NULL ) {
            std::string db = nsGetDB( ns );
            std::string coll = nsGetCollection( ns );
            uassert( 10011 ,  "no collection name", coll.size() );

            BSONObj temp;
            if ( info == NULL ) {
                info = &temp;
            }

            bool res = runCommand( db.c_str() , BSON( "drop" << coll ) , *info );
            return res;
        }

        /** Perform a repair and compaction of the specified database.  May take a long time to run.  Disk space
           must be available equal to the size of the database while repairing.
        */
        bool repairDatabase(const std::string &dbname, BSONObj *info = 0) {
            return simpleCommand(dbname, info, "repairDatabase");
        }

        /** Copy database from one server or name to another server or name.

           Generally, you should dropDatabase() first as otherwise the copied information will MERGE
           into whatever data is already present in this database.

           For security reasons this function only works when you are authorized to access the "admin" db.  However,
           if you have access to said db, you can copy any database from one place to another.
           TODO: this needs enhancement to be more flexible in terms of security.

           This method provides a way to "rename" a database by copying it to a new db name and
           location.  The copy is "repaired" and compacted.

           fromdb   database name from which to copy.
           todb     database name to copy to.
           fromhost hostname of the database (and optionally, ":port") from which to
                    copy the data.  copies from self if "".

           returns true if successful
        */
        bool copyDatabase(const std::string& fromdb,
                          const std::string& todb,
                          const std::string& fromhost = "",
                          const std::string& mechanism = "DEFAULT",
                          const std::string& username = "",
                          const std::string& password = "",
                          BSONObj *info = 0);

        /** The Mongo database provides built-in performance profiling capabilities.  Uset setDbProfilingLevel()
           to enable.  Profiling information is then written to the system.profile collection, which one can
           then query.
        */
        enum ProfilingLevel {
            ProfileOff = 0,
            ProfileSlow = 1, // log very slow (>100ms) operations
            ProfileAll = 2

        };
        bool setDbProfilingLevel(const std::string &dbname, ProfilingLevel level, BSONObj *info = 0);
        bool getDbProfilingLevel(const std::string &dbname, ProfilingLevel& level, BSONObj *info = 0);


        /** This implicitly converts from char*, string, and BSONObj to be an argument to mapreduce
            You shouldn't need to explicitly construct this
         */
        struct MROutput {
            MROutput(const char* collection) : out(BSON("replace" << collection)) {}
            MROutput(const std::string& collection) : out(BSON("replace" << collection)) {}
            MROutput(const BSONObj& obj) : out(obj) {}

            BSONObj out;
        };
        static MROutput MRInline;

        /** Run a map/reduce job on the server.

            See http://dochub.mongodb.org/core/mapreduce

            ns        namespace (db+collection name) of input data
            jsmapf    javascript map function code
            jsreducef javascript reduce function code.
            query     optional query filter for the input
            output    either a string collection name or an object representing output type
                      if not specified uses inline output type

            returns a result object which contains:
             { result : <collection_name>,
               numObjects : <number_of_objects_scanned>,
               timeMillis : <job_time>,
               ok : <1_if_ok>,
               [, err : <errmsg_if_error>]
             }

             For example one might call:
               result.getField("ok").trueValue()
             on the result to check if ok.
        */
        BSONObj mapreduce(
            const std::string &ns,
            const std::string &jsmapf,
            const std::string &jsreducef,
            Query query = Query(),
            MROutput output = MRInline
        );

        /**
         * Groups documents in a collection by the specified key and performs simple aggregation
         * functions such as computing counts and sums.
         *
         * @note WARNING: Use of the group command is strongly discouraged, it is much better to use
         * the aggregation framework to acheive similar functionality.
         *
         * @see http://docs.mongodb.org/manual/reference/method/db.collection.group
         *
         * @param ns The namespace to group
         * @param key The field or fields to group specified as a projection document: { field: 1 }
         * @param jsreduce An aggregation function that operates on the documents during the group
         * ing operation. The function should take two arguments: the current document and an
         * aggregation result for that group.
         * @param output The output vector.
         * @param initial Initial aggregation result document.
         * @param query Optional selection criteria to determine which documents to process.
         * @param finalize Optional function that runs for each item in the result set before
         * returning the final values in the output vector.
         */
        void group(
            const StringData& ns,
            const StringData& jsreduce,
            std::vector<BSONObj>* output,
            const BSONObj& initial = BSONObj(),
            const Query& query = Query(),
            const BSONObj& key = BSONObj(),
            const StringData& finalize = ""
        );

        /**
         * Does the same thing as 'group' but accepts a key function, 'jskey', that is used to
         * create an object representing the key. This allows for grouping on calculated fields
         * rather on existing fields alone.
         *
         * @see DBClientWithCommands::group
         */
        void groupWithKeyFunction(
            const StringData& ns,
            const StringData& jsreduce,
            std::vector<BSONObj>* output,
            const BSONObj& initial = BSONObj(),
            const Query& query = Query(),
            const StringData& jskey = "",
            const StringData& finalize = ""
        );

        /**
         * Finds the distinct values for a specified field across a single collection and returns
         * the results in an array.
         *
         * @param ns The namespace to query for distinct values.
         * @param field The field for which to return distinct values.
         *  Examples: "name", "address.street"
         * @param query Optional query that specifies a filter for documents from which to retrieve
         * distinct values.
         */
        BSONObj distinct(
            const StringData& ns,
            const StringData& field,
            const Query& query = Query()
        );

        /**
         * Modifies and returns a single document.
         *
         * @note By default, the returned document does not include modifications made on update.
         *
         * @param ns Namespace on which to perform this findAndModify.
         * @param update Update document to be applied.
         * @param query Filter for the update.
         * @param upsert Insert if object does not exist.
         * @param sort Sort for the filter.
         * @param new Return the updated rather than original object.
         * @param fields Fields to return. Specifies inclusion with 1, "{<field1>: 1, ...}"
         */
        BSONObj findAndModify(
            const StringData& ns,
            const BSONObj& query,
            const BSONObj& update,
            bool upsert = false,
            bool returnNew = false,
            const BSONObj& sort = BSONObj(),
            const BSONObj& fields = BSONObj()
        );

        /**
         * Removes and returns a single document.
         *
         * @note By default, the returned document does not include modifications made on update.
         *
         * @param ns Namespace on which to perform this findAndModify.
         * @param query Filter for the update.
         * @param sort Sort for the filter.
         * @param fields Fields to return. Specifies inclusion with 1, "{<field1>: 1, ...}"
         */
        BSONObj findAndRemove(
            const StringData& ns,
            const BSONObj& query,
            const BSONObj& sort = BSONObj(),
            const BSONObj& fields = BSONObj()
        );

        /** Run javascript code on the database server.
           dbname    database SavedContext in which the code runs. The javascript variable 'db' will be assigned
                     to this database when the function is invoked.
           jscode    source code for a javascript function.
           info      the command object which contains any information on the invocation result including
                      the return value and other information.  If an error occurs running the jscode, error
                     information will be in info.  (try "log() << info.toString()")
           retValue  return value from the jscode function.
           args      args to pass to the jscode function.  when invoked, the 'args' variable will be defined
                     for use by the jscode.
           nolock    if true, the server will not take a global write lock when executing the jscode.

           returns true if runs ok.

           See testDbEval() in dbclient.cpp for an example of usage.
        */
        bool eval(const std::string &dbname, const std::string &jscode, BSONObj& info, BSONElement& retValue, BSONObj *args = 0, bool nolock = false);

        /** validate a collection, checking for errors and reporting back statistics.
            this operation is slow and blocking.
         */
        bool validate( const std::string &ns , bool scandata=true ) {
            BSONObj cmd = BSON( "validate" << nsGetCollection( ns ) << "scandata" << scandata );
            BSONObj info;
            return runCommand( nsGetDB( ns ).c_str() , cmd , info );
        }

        /* The following helpers are simply more convenient forms of eval() for certain common cases */

        /* invocation with no return value of interest -- with or without one simple parameter */
        bool eval(const std::string &dbname, const std::string &jscode);
        template< class T >
        bool eval(const std::string &dbname, const std::string &jscode, T parm1) {
            BSONObj info;
            BSONElement retValue;
            BSONObjBuilder b;
            b.append("0", parm1);
            BSONObj args = b.done();
            return eval(dbname, jscode, info, retValue, &args);
        }

        /** eval invocation with one parm to server and one numeric field (either int or double) returned */
        template< class T, class NumType >
        bool eval(const std::string &dbname, const std::string &jscode, T parm1, NumType& ret) {
            BSONObj info;
            BSONElement retValue;
            BSONObjBuilder b;
            b.append("0", parm1);
            BSONObj args = b.done();
            if ( !eval(dbname, jscode, info, retValue, &args) )
                return false;
            ret = (NumType) retValue.number();
            return true;
        }

        /**
           get a list of all the current databases
           uses the { listDatabases : 1 } command.
           throws on error
         */
        std::list<std::string> getDatabaseNames();

        /**
         * Get a list of all the current collections in db.
         */
        std::list<std::string> getCollectionNames( const std::string& db,
                                                   const BSONObj& filter = BSONObj() );
        /**
         * { name : "<short collection name>",
         *   options : { }
         * }
         */
        std::list<BSONObj> getCollectionInfos( const std::string& db,
                                               const BSONObj& filter = BSONObj() );

        /**
         * Returns a DBClientCursor with collection information objects.
         *
         *  Example collection information object:
         *  {
         *      "name" : "mongo_cxx_driver",
         *      "options" : {
         *          "flags" : 1
         *      }
         *  }
         */
        std::auto_ptr<DBClientCursor> enumerateCollections( const std::string& db,
                                                            const BSONObj& filter = BSONObj(),
                                                            int batchSize = 0 );

        bool exists( const std::string& ns );

        /** Create an index on the collection 'ns' as described by the given keys. If you wish
         *  to specify options, see the more flexible overload of 'createIndex' which takes an
         *  IndexSpec object. Failure to construct the index is reported by throwing an
         *  OperationException.
         *
         *  @param ns Namespace on which to create the index
         *  @param keys Document describing keys and index types. You must provide at least one
         * field and its direction.
         */
        void createIndex( const StringData& ns, const BSONObj& keys ) {
            return createIndex( ns, IndexSpec().addKeys(keys) );
        }

        /** Create an index on the collection 'ns' as described by the given
         *  descriptor. Failure to construct the index is reported by throwing an
         *  OperationException.
         *
         *  @param ns Namespace on which to create the index
         *  @param descriptor Configuration object describing the index to create. The
         *  descriptor must describe at least one key and index type.
         */
        virtual void createIndex( const StringData& ns, const IndexSpec& descriptor );

        virtual std::list<BSONObj> getIndexSpecs( const std::string &ns, int options = 0 );

        /**
         * Enumerates all indexes on ns (a db-qualified collection name). Returns a list of the index names.
         */
        virtual std::list<std::string> getIndexNames( const std::string& ns, int options = 0 );

        virtual std::auto_ptr<DBClientCursor> enumerateIndexes( const std::string& ns,
                                                                int options = 0,
                                                                int batchSize = 0 );

        virtual void dropIndex( const std::string& ns , BSONObj keys );
        virtual void dropIndex( const std::string& ns , const std::string& indexName );

        /**
           drops all indexes for the collection
         */
        virtual void dropIndexes( const std::string& ns );

        virtual void reIndex( const std::string& ns );

        static std::string genIndexName( const BSONObj& keys );

        /** Erase / drop an entire database */
        virtual bool dropDatabase(const std::string &dbname, BSONObj *info = 0) {
            bool ret = simpleCommand(dbname, info, "dropDatabase");
            return ret;
        }

        virtual std::string toString() const = 0;

        /**
         * A function type for runCommand hooking; the function takes a pointer
         * to a BSONObjBuilder and returns nothing.  The builder contains a
         * runCommand BSON object.
         * Once such a function is set as the runCommand hook, every time the DBClient
         * processes a runCommand, the hook will be called just prior to sending it to the server. 
         */
        typedef stdx::function<void(BSONObjBuilder*)> RunCommandHookFunc;
        virtual void setRunCommandHook(RunCommandHookFunc func);
        RunCommandHookFunc getRunCommandHook() const {
            return _runCommandHook;
        }

        /** 
         * Similar to above, but for running a function on a command response after a command
         * has been run.
         */
        typedef stdx::function<void(const BSONObj&, const std::string&)> PostRunCommandHookFunc;
        virtual void setPostRunCommandHook(PostRunCommandHookFunc func);
        PostRunCommandHookFunc getPostRunCommandHook() const {
            return _postRunCommandHook;
        }


    protected:
        /** if the result of a command is ok*/
        bool isOk(const BSONObj&);

        /** if the element contains a not master error */
        bool isNotMasterErrorString( const BSONElement& e );

        BSONObj _countCmd(const std::string &ns, const Query& query, int options, int limit, int skip );

        /**
         * Look up the options available on this client.  Caches the answer from
         * _lookupAvailableOptions(), below.
         */
        QueryOptions availableOptions();

        virtual QueryOptions _lookupAvailableOptions();

        virtual void _auth(const BSONObj& params);

        /**
         * Use the MONGODB-CR protocol to authenticate as "username" against the database "dbname",
         * with the given password.  If digestPassword is false, the password is assumed to be
         * pre-digested.  Returns false on failure, and sets "errmsg".
         */
        bool _authMongoCR(const std::string &dbname,
                          const std::string &username,
                          const std::string &pwd,
                          BSONObj *info,
                          bool digestPassword);

        /**
         * Use the MONGODB-X509 protocol to authenticate as "username. The certificate details
         * has already been communicated automatically as part of the connect call.
         * Returns false on failure and set "errmsg".
         */
        bool _authX509(const std::string &dbname,
                       const std::string &username,
                       BSONObj *info);

        /**
         * These functions will be executed by the driver on runCommand calls.
         */
        RunCommandHookFunc _runCommandHook;
        PostRunCommandHookFunc _postRunCommandHook;
        int _minWireVersion;
        int _maxWireVersion;


    private:
        enum QueryOptions _cachedAvailableOptions;
        bool _haveCachedAvailableOptions;

        void _buildGroupObj(
            const StringData& ns,
            const StringData& jsreduce,
            const BSONObj& initial,
            const Query& query,
            const StringData& finalize,
            BSONObjBuilder* groupObj
        );

        void _runGroup(
            const StringData& ns,
            const BSONObj& group,
            const Query& query,
            std::vector<BSONObj>* output
        );

        void _findAndModify(
            const StringData& ns,
            const BSONObj& query,
            const BSONObj& update,
            const BSONObj& sort,
            bool returnNew,
            bool upsert,
            const BSONObj& fields,
            BSONObjBuilder* out
        );

        std::auto_ptr<DBClientCursor> _legacyCollectionInfo(
            const std::string& db,
            const BSONObj& filter,
            int batchSize
        );
    };

    class DBClientWriter;
    class WriteOperation;

    /**
     abstract class that implements the core db operations
     */
    class MONGO_CLIENT_API DBClientBase : public DBClientWithCommands, public DBConnector {
    friend class BulkOperationBuilder;
    protected:
        static AtomicInt64 ConnectionIdSequence;
        long long _connectionId; // unique connection id for this connection
        const boost::scoped_ptr<DBClientWriter> _wireProtocolWriter;
        const boost::scoped_ptr<DBClientWriter> _commandWriter;
        WriteConcern _writeConcern;
        int _maxBsonObjectSize;
        int _maxMessageSizeBytes;
        int _maxWriteBatchSize;
        void _write(
            const std::string& ns,
            const std::vector<WriteOperation*>& writes,
            bool ordered,
            const WriteConcern* writeConcern,
            WriteResult* writeResult
        );
    public:
        static const uint64_t INVALID_SOCK_CREATION_TIME;

        DBClientBase();

        // Required because compiler can't generate a destructor for the _writer
        // as it is an incomplete type.
        virtual ~DBClientBase();

        long long getConnectionId() const { return _connectionId; }

        const WriteConcern& getWriteConcern() const { return _writeConcern; }
        void setWriteConcern( const WriteConcern& w ) { _writeConcern = w; }

        void setWireVersions( int minWireVersion, int maxWireVersion ){
            _minWireVersion = minWireVersion;
            _maxWireVersion = maxWireVersion;
        }

        int getMinWireVersion() { return _minWireVersion; }
        int getMaxWireVersion() { return _maxWireVersion; }
        int getMaxBsonObjectSize() { return _maxBsonObjectSize; }
        int getMaxMessageSizeBytes() { return _maxMessageSizeBytes; }
        int getMaxWriteBatchSize() { return _maxWriteBatchSize; }

        /** send a query to the database.
         @param ns namespace to query, format is <dbname>.<collectname>[.<collectname>]*
         @param query query to perform on the collection.  this is a BSONObj (binary JSON)
         You may format as
           { query: { ... }, orderby: { ... } }
         to specify a sort order.
         @param nToReturn n to return (i.e., limit).  0 = unlimited
         @param nToSkip start with the nth item
         @param fieldsToReturn optional template of which fields to select. if unspecified, returns all fields
         @param queryOptions see options enum at top of this file

         @return    cursor.   0 if error (connection failure)
         @throws AssertionException
        */
        virtual std::auto_ptr<DBClientCursor> query(const std::string &ns, Query query, int nToReturn = 0, int nToSkip = 0,
                                                    const BSONObj *fieldsToReturn = 0, int queryOptions = 0 , int batchSize = 0 );

        /**
         * Returns a list of up to 'numCursors' cursors that can be iterated concurrently.
         *
         * As long as the collection is not modified during scanning, each document appears once
         * in one of the cursors' result sets.
         *
         * @note Warning: One must delete the cursors after use.
         * @note Warning: One must delete any new connections created by the connection factory
         *  after use.
         *
         * @see example usage in dbclient_test.cpp -> DBClientTest/ParallelCollectionScan
         *
         * @param ns The namespace to scan
         * @param numCursors Number of cursors to return. You may get back less than you asked for.
         * @param cursors Output vector to hold cursors created for this scan.
         * @param connectionFactory Function that returns a pointer to a DBClientBase for use by
         *  newly created cursors. The function takes zero parameters but additional parameters
         *  may be bound (if required) using std::bind. See the example listed above for more info.
         */
        virtual void parallelScan(
            const StringData& ns,
            int numCursors,
            std::vector<DBClientCursor*>* cursors,
            stdx::function<DBClientBase* ()> connectionFactory
        );

        virtual std::auto_ptr<DBClientCursor> aggregate(const std::string& ns,
                                                        const BSONObj& pipeline,
                                                        const BSONObj* aggregateOptions = NULL,
                                                        int queryOptions = 0);


        /** Uses QueryOption_Exhaust, when available.

            Exhaust mode sends back all data queries as fast as possible, with no back-and-forth for
            OP_GETMORE.  If you are certain you will exhaust the query, it could be useful.

            Use the DBClientCursorBatchIterator version, below, if you want to do items in large
            blocks, perhaps to avoid granular locking and such.
         */
        virtual unsigned long long query( stdx::function<void(const BSONObj&)> f,
                                          const std::string& ns,
                                          Query query,
                                          const BSONObj *fieldsToReturn = 0,
                                          int queryOptions = 0 );

        virtual unsigned long long query( stdx::function<void(DBClientCursorBatchIterator&)> f,
                                          const std::string& ns,
                                          Query query,
                                          const BSONObj *fieldsToReturn = 0,
                                          int queryOptions = 0 );


        /** don't use this - called automatically by DBClientCursor for you
            @param cursorId id of cursor to retrieve
            @return an handle to a previously allocated cursor
            @throws AssertionException
         */
        virtual std::auto_ptr<DBClientCursor> getMore( const std::string &ns, long long cursorId, int nToReturn = 0, int options = 0 );

        /**
           insert an object into the database
         */
        virtual void insert(
            const std::string &ns,
            BSONObj obj,
            int flags=0,
            const WriteConcern* wc=NULL
        );

        /**
           insert a vector of objects into the database
         */
        virtual void insert(
            const std::string &ns,
            const std::vector< BSONObj >& v,
            int flags=0,
            const WriteConcern* wc=NULL
        );

        /**
           updates objects matching query
         */
        virtual void update(
            const std::string &ns,
            Query query,
            BSONObj obj,
            bool upsert=false,
            bool multi=false,
            const WriteConcern* wc=NULL
        );

        virtual void update(
            const std::string &ns,
            Query query,
            BSONObj obj,
            int flags,
            const WriteConcern* wc=NULL
        );

        /**
         * Initializes an ordered bulk operation by returning an object that can be
         * used to enqueue multiple operations for batch execution.
         *
         * @param ns Namespace on which to apply the operations.
         * @see BulkOperationBuilder
         */
        virtual BulkOperationBuilder initializeUnorderedBulkOp(const std::string& ns);

        /**
         * Initializes an unordered bulk operation by returning an object that can be
         * used to enqueue multiple operations for batch execution.
         *
         * @param ns Namespace on which to apply the operations.
         * @see BulkOperationBuilder
         */
        virtual BulkOperationBuilder initializeOrderedBulkOp(const std::string& ns);

        /**
           remove matching objects from the database
           @param justOne if this true, then once a single match is found will stop
         */
        virtual void remove( const std::string &ns , Query q , bool justOne = 0, const WriteConcern* wc=NULL );

        virtual void remove( const std::string &ns , Query query, int flags, const WriteConcern* wc=NULL );

        virtual bool isFailed() const = 0;

        /**
         * if not checked recently, checks whether the underlying socket/sockets are still valid
         */
        virtual bool isStillConnected() = 0;

        virtual void killCursor( long long cursorID ) = 0;

        virtual bool callRead( Message& toSend , Message& response ) = 0;
        // virtual bool callWrite( Message& toSend , Message& response ) = 0; // TODO: add this if needed

        virtual ConnectionString::ConnectionType type() const = 0;

        virtual double getSoTimeout() const = 0;

        virtual uint64_t getSockCreationMicroSec() const {
            return INVALID_SOCK_CREATION_TIME;
        }

        virtual void reset() {}

    }; // DBClientBase

    class DBClientReplicaSet;

    class MONGO_CLIENT_API ConnectException : public UserException {
    public:
        ConnectException(std::string msg) : UserException(9000,msg) { }
    };

    /**
        A basic connection to the database.
        This is the main entry point for talking to a simple Mongo setup
    */
    class MONGO_CLIENT_API DBClientConnection : public DBClientBase {
    public:
        using DBClientBase::query;

        /**
           @param _autoReconnect if true, automatically reconnect on a connection failure
           @param cp - ignored: do not provide a value for this paramter
           @param timeout tcp timeout in seconds - this is for read/write, not connect.
           Connect timeout is fixed, but short, at 5 seconds.
         */
        DBClientConnection(bool _autoReconnect = false, DBClientReplicaSet* cp = 0, double so_timeout = 0);

        virtual ~DBClientConnection() {
            _numConnections.fetchAndAdd(-1);
        }

        /** Connect to a Mongo database server.

           If autoReconnect is true, you can try to use the DBClientConnection even when
           false was returned -- it will try to connect again.

           @param server server to connect to.
           @param errmsg any relevant error message will appended to the string
           @return false if fails to connect.
        */
        virtual bool connect(const HostAndPort& server, std::string& errmsg);

        /** Compatibility connect now that HostAndPort has an explicit constructor */
        bool connect(const std::string& server, std::string& errmsg) {
            return connect(HostAndPort(server), errmsg);
        }

        /** Connect to a Mongo database server.  Exception throwing version.
            Throws a UserException if cannot connect.

           If autoReconnect is true, you can try to use the DBClientConnection even when
           false was returned -- it will try to connect again.

           @param serverHostname host to connect to.  can include port number ( 127.0.0.1 , 127.0.0.1:5555 )
        */
        void connect(const std::string& serverHostname) {
            std::string errmsg;
            if( !connect(HostAndPort(serverHostname), errmsg) )
                throw ConnectException(std::string("can't connect ") + errmsg);
        }

        /**
         * Logs out the connection for the given database.
         *
         * @param dbname the database to logout from.
         * @param info the result object for the logout command (provided for backwards
         *     compatibility with mongo shell)
         */
        virtual void logout(const std::string& dbname, BSONObj& info);

        virtual std::auto_ptr<DBClientCursor> query(const std::string &ns, Query query=Query(), int nToReturn = 0, int nToSkip = 0,
                                                    const BSONObj *fieldsToReturn = 0, int queryOptions = 0 , int batchSize = 0 );

        virtual unsigned long long query( stdx::function<void(DBClientCursorBatchIterator &)> f,
                                          const std::string& ns,
                                          Query query,
                                          const BSONObj *fieldsToReturn,
                                          int queryOptions );

        virtual bool runCommand(const std::string &dbname,
                                const BSONObj& cmd,
                                BSONObj &info,
                                int options=0);

        /**
           @return true if this connection is currently in a failed state.  When autoreconnect is on,
                   a connection will transition back to an ok state after reconnecting.
         */
        bool isFailed() const { return _failed; }

        bool isStillConnected() { return p ? p->isStillConnected() : true; }

        MessagingPort& port() { verify(p); return *p; }

        std::string toString() const {
            std::stringstream ss;
            ss << _serverString;
            if ( !_serverAddrString.empty() ) ss << " (" << _serverAddrString << ")";
            if ( _failed ) ss << " failed";
            return ss.str();
        }

        std::string getServerAddress() const { return _serverString; }
        const HostAndPort& getServerHostAndPort() const { return _server; }

        virtual void killCursor( long long cursorID );
        virtual bool callRead( Message& toSend , Message& response ) { return call( toSend , response ); }
        virtual void say( Message &toSend, bool isRetry = false , std::string * actualServer = 0 );
        virtual bool recv( Message& m );
        virtual void checkResponse( const char *data, int nReturned, bool* retry = NULL, std::string* host = NULL );
        virtual bool call( Message &toSend, Message &response, bool assertOk = true , std::string * actualServer = 0 );
        virtual ConnectionString::ConnectionType type() const { return ConnectionString::MASTER; }
        void setSoTimeout(double timeout);
        double getSoTimeout() const { return _so_timeout; }

        virtual bool lazySupported() const { return true; }

        static int MONGO_CLIENT_FUNC getNumConnections() {
            return _numConnections.load();
        }

        /**
         * Set the name of the replica set that this connection is associated to.
         * Note: There is no validation on replSetName.
         */
        void setParentReplSetName(const std::string& replSetName);

        static void MONGO_CLIENT_FUNC setLazyKillCursor( bool lazy ) { _lazyKillCursor = lazy; }
        static bool MONGO_CLIENT_FUNC getLazyKillCursor() { return _lazyKillCursor; }

        uint64_t getSockCreationMicroSec() const;

    protected:
        virtual void _auth(const BSONObj& params);
        virtual void sayPiggyBack( Message &toSend );

        boost::scoped_ptr<MessagingPort> p;
        boost::scoped_ptr<SockAddr> server;
        bool _failed;
        const bool autoReconnect;
        Backoff autoReconnectBackoff;
        HostAndPort _server; // remember for reconnects
        std::string _serverString;     // server host and port
        std::string _serverAddrString; // resolved ip of server
        void _checkConnection();

        // throws SocketException if in failed state and not reconnecting or if waiting to reconnect
        void checkConnection() { if( _failed ) _checkConnection(); }

        std::map<std::string, BSONObj> authCache;
        double _so_timeout;
        bool _connect( std::string& errmsg );

        static AtomicInt32 _numConnections;
        static bool _lazyKillCursor; // lazy means we piggy back kill cursors on next op

#ifdef MONGO_SSL
        SSLManagerInterface* sslManager();
#endif

    private:

        /**
         * Checks the BSONElement for the 'not master' keyword and if it does exist,
         * try to inform the replica set monitor that the host this connects to is
         * no longer primary.
         */
        void handleNotMasterResponse(const BSONElement& elemToCheck);

        // Contains the string for the replica set name of the host this is connected to.
        // Should be empty if this connection is not pointing to a replica set member.
        std::string _parentReplSetName;
    };

    /** pings server to check if it's up
     */
    MONGO_CLIENT_API bool MONGO_CLIENT_FUNC serverAlive( const std::string &uri );

    MONGO_CLIENT_API BSONElement MONGO_CLIENT_FUNC getErrField( const BSONObj& result );
    MONGO_CLIENT_API bool MONGO_CLIENT_FUNC hasErrField( const BSONObj& result );

    MONGO_CLIENT_API inline std::ostream& MONGO_CLIENT_FUNC operator<<( std::ostream &s, const Query &q ) {
        return s << q.toString();
    }

} // namespace mongo
