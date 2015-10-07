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

#include "mongo/client/dbclientcursor.h"

#include "mongo/db/dbmessage.h"
#include "mongo/db/namespace_string.h"
#include "mongo/util/debug_util.h"
#include "mongo/client/dbclientcursorshim.h"
#include "mongo/util/log.h"

namespace mongo {

using std::auto_ptr;
using std::endl;
using std::string;
using std::vector;

DBClientCursor::DBClientCursor(DBClientBase* client,
                               const std::string& _ns,
                               BSONObj _query,
                               int _nToReturn,
                               int _nToSkip,
                               const BSONObj* _fieldsToReturn,
                               int queryOptions,
                               int bs)
    : _client(client),
      ns(_ns),
      query(_query),
      nToReturn((queryOptions & QueryOption_CursorTailable) ? 0 : _nToReturn),
      nToSkip(_nToSkip),
      nReturned(0),
      fieldsToReturn(_fieldsToReturn),
      opts(queryOptions),
      batchSize(bs == 1 ? 2 : bs),
      resultFlags(0),
      cursorId(),
      _ownCursor(true),
      wasError(false) {
    _finishConsInit();
}

DBClientCursor::DBClientCursor(DBClientBase* client,
                               const std::string& _ns,
                               long long _cursorId,
                               int _nToReturn,
                               int options,
                               int bs)
    : _client(client),
      ns(_ns),
      nToReturn((options & QueryOption_CursorTailable) ? 0 : _nToReturn),
      nToSkip(0),
      nReturned(0),
      fieldsToReturn(0),
      opts(options),
      batchSize(bs == 1 ? 2 : bs),
      resultFlags(0),
      cursorId(_cursorId),
      _ownCursor(true),
      wasError(false) {
    _finishConsInit();
}

void assembleRequest(const string& ns,
                     BSONObj query,
                     int nToReturn,
                     int nToSkip,
                     const BSONObj* fieldsToReturn,
                     int queryOptions,
                     Message& toSend);

void DBClientCursor::_finishConsInit() {
    _originalHost = _client->getServerAddress();
}

int DBClientCursor::nextBatchSize() {
    if (nToReturn == 0)
        return batchSize;
    if (batchSize == 0)
        return nToReturn;
    return std::min(batchSize, nToReturn);
}

void DBClientCursor::_assembleInit(Message& toSend) {
    if (!cursorId) {
        assembleRequest(ns, query, nextBatchSize(), nToSkip, fieldsToReturn, opts, toSend);
    } else {
        BufBuilder b;
        b.appendNum(opts);
        b.appendStr(ns);
        b.appendNum(nextBatchSize());
        b.appendNum(cursorId);
        toSend.setData(dbGetMore, b.buf(), b.len());
    }
}

bool DBClientCursor::init() {
    Message toSend;
    _assembleInit(toSend);

    if (!_client->call(toSend, *batch.m, false, &_originalHost)) {
        // log msg temp?
        log() << "DBClientCursor::init call() failed" << endl;
        return false;
    }
    if (batch.m->empty()) {
        // log msg temp?
        log() << "DBClientCursor::init message from call() was empty" << endl;
        return false;
    }
    dataReceived();
    return true;
}

void DBClientCursor::initLazy(bool isRetry) {
    massert(15875,
            "DBClientCursor::initLazy called on a client that doesn't support lazy",
            _client->lazySupported());
    if (DBClientWithCommands::RunCommandHookFunc hook = _client->getRunCommandHook()) {
        if (NamespaceString(ns).isCommand()) {
            BSONObjBuilder bob;
            bob.appendElements(query);
            hook(&bob);
            query = bob.obj();
        }
    }

    Message toSend;
    _assembleInit(toSend);
    _client->say(toSend, isRetry, &_originalHost);
}

bool DBClientCursor::initLazyFinish(bool& retry) {
    bool recvd = _client->recv(*batch.m);

    // If we get a bad response, return false
    if (!recvd || batch.m->empty()) {
        if (!recvd)
            log() << "DBClientCursor::init lazy say() failed" << endl;
        if (batch.m->empty())
            log() << "DBClientCursor::init message from say() was empty" << endl;

        _client->checkResponse(NULL, -1, &retry, &_lazyHost);

        return false;
    }

    dataReceived(retry, _lazyHost);

    if (DBClientWithCommands::PostRunCommandHookFunc hook = _client->getPostRunCommandHook()) {
        if (NamespaceString(ns).isCommand()) {
            BSONObj cmdResponse = peekFirst();
            hook(cmdResponse, _lazyHost);
        }
    }

    return !retry;
}

bool DBClientCursor::initCommand() {
    BSONObj res;

    bool ok = _client->runCommand(nsGetDB(ns), query, res, opts);
    replyToQuery(0, *batch.m, res);
    dataReceived();

    return ok;
}

void DBClientCursor::requestMore() {
    verify(cursorId && batch.pos == batch.nReturned);

    BufBuilder b;
    b.appendNum(opts);
    b.appendStr(ns);
    b.appendNum(nextBatchSize());
    b.appendNum(cursorId);

    Message toSend;
    toSend.setData(dbGetMore, b.buf(), b.len());
    auto_ptr<Message> response(new Message());

    _client->call(toSend, *response);
    this->batch.m = response;
    dataReceived();
}

/** with QueryOption_Exhaust, the server just blasts data at us (marked at end with cursorid==0). */
void DBClientCursor::exhaustReceiveMore() {
    verify(cursorId && batch.pos == batch.nReturned);
    verify(!nToReturn);
    auto_ptr<Message> response(new Message());
    if (!_client->recv(*response)) {
        uasserted(16465, "recv failed while exhausting cursor");
    }
    batch.m = response;
    dataReceived();
}

void DBClientCursor::dataReceived(bool& retry, string& host) {
    QueryResult::View qr = batch.m->singleData().view2ptr();
    resultFlags = qr.getResultFlags();

    if (qr.getResultFlags() & ResultFlag_ErrSet) {
        wasError = true;
    }

    if (qr.getResultFlags() & ResultFlag_CursorNotFound) {
        // cursor id no longer valid at the server.
        verify(qr.getCursorId() == 0);
        cursorId = 0;  // 0 indicates no longer valid (dead)
        if (!(opts & QueryOption_CursorTailable))
            throw UserException(
                13127, "getMore: cursor didn't exist on server, possible restart or timeout?");
    }

    if (cursorId == 0 || !(opts & QueryOption_CursorTailable)) {
        // only set initially: we don't want to kill it on end of data
        // if it's a tailable cursor
        cursorId = qr.getCursorId();
    }

    batch.nReturned = qr.getNReturned();
    batch.pos = 0;
    batch.data = qr.data();

    _client->checkResponse(batch.data, batch.nReturned, &retry, &host);  // watches for "not master"

    /* this assert would fire the way we currently work:
        verify( nReturned || cursorId == 0 );
    */
}

/** If true, safe to call next().  Requests more from server if necessary. */
bool DBClientCursor::rawMore() {
    DEV _assertIfNull();

    if (nToReturn && nReturned >= nToReturn)
        return false;

    if (batch.pos < batch.nReturned)
        return true;

    if (cursorId == 0)
        return false;

    requestMore();
    return batch.pos < batch.nReturned;
}

bool DBClientCursor::more() {
    DEV _assertIfNull();

    if (!_putBack.empty())
        return true;

    if (shim.get())
        return shim->more();

    return rawMore();
}

BSONObj DBClientCursor::rawNext() {
    DEV _assertIfNull();

    uassert(13422, "DBClientCursor next() called but more() is false", batch.pos < batch.nReturned);

    batch.pos++;
    BSONObj o(batch.data);
    batch.data += o.objsize();
    /* todo would be good to make data null at end of batch for safety */
    return o;
}

BSONObj DBClientCursor::next() {
    DEV _assertIfNull();

    nReturned++;

    if (!_putBack.empty()) {
        BSONObj ret = _putBack.top();
        _putBack.pop();
        return ret;
    }

    if (shim.get())
        return shim->next();

    return rawNext();
}

BSONObj DBClientCursor::nextSafe() {
    BSONObj o = next();
    if (this->wasError && strcmp(o.firstElementFieldName(), "$err") == 0) {
        std::string s = "nextSafe(): " + o.toString();
        LOG(5) << s;
        uasserted(13106, s);
    }
    return o;
}

void DBClientCursor::peek(vector<BSONObj>& v, int atMost) {
    int m = atMost;

    /*
    for( stack<BSONObj>::iterator i = _putBack.begin(); i != _putBack.end(); i++ ) {
        if( m == 0 )
            return;
        v.push_back(*i);
        m--;
        n++;
    }
    */

    int p = batch.pos;
    const char* d = batch.data;
    while (m && p < batch.nReturned) {
        BSONObj o(d);
        d += o.objsize();
        p++;
        m--;
        v.push_back(o);
    }
}

BSONObj DBClientCursor::peekFirst() {
    vector<BSONObj> v;
    peek(v, 1);

    if (v.size() > 0)
        return v[0];
    else
        return BSONObj();
}

bool DBClientCursor::peekError(BSONObj* error) {
    if (!wasError)
        return false;

    vector<BSONObj> v;
    peek(v, 1);

    verify(v.size() == 1);
    verify(hasErrField(v[0]));

    if (error)
        *error = v[0].getOwned();
    return true;
}

DBClientCursor::~DBClientCursor() {
    kill();
}

void DBClientCursor::kill() {
    DESTRUCTOR_GUARD(

        if (cursorId && _ownCursor) {
            BufBuilder b;
            b.appendNum((int)0);  // reserved
            b.appendNum((int)1);  // number
            b.appendNum(cursorId);

            Message m;
            m.setData(dbKillCursors, b.buf(), b.len());

            // Kill the cursor the same way the connection itself would.  Usually, non-lazily
            if (DBClientConnection::getLazyKillCursor())
                _client->sayPiggyBack(m);
            else
                _client->say(m);
        }

        );

    // Mark this cursor as dead since we can't do any getMores.
    cursorId = 0;
}


}  // namespace mongo
