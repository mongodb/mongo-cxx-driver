// assert_util.cpp

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

#define MONGO_LOG_DEFAULT_COMPONENT ::mongo::logger::LogComponent::kDefault

#include "mongo/platform/basic.h"

#include "mongo/util/assert_util.h"

using namespace std;

#ifndef _WIN32
#include <cxxabi.h>
#include <sys/file.h>
#endif

#include "mongo/bson/bsonobjbuilder.h"
#include "mongo/util/debug_util.h"
#include "mongo/util/log.h"

namespace mongo {

string DBException::toString() const {
    stringstream ss;
    ss << getCode() << " " << what();
    return ss.str();
}

ErrorCodes::Error DBException::convertExceptionCode(int exCode) {
    if (exCode == 0)
        return ErrorCodes::UnknownError;
    return static_cast<ErrorCodes::Error>(exCode);
}

void ExceptionInfo::append(BSONObjBuilder& b, const char* m, const char* c) const {
    if (msg.empty())
        b.append(m, "unknown assertion");
    else
        b.append(m, msg);

    if (code)
        b.append(c, code);
}

/* "warning" assert -- safe to continue, so we don't throw exception. */
NOINLINE_DECL void wasserted(const char* expr, const char* file, unsigned line) {
    static bool rateLimited;
    static time_t lastWhen;
    static unsigned lastLine;
    if (lastLine == line && time(0) - lastWhen < 5) {
        if (!rateLimited) {
            rateLimited = true;
            log() << "rate limiting wassert" << endl;
        }
        return;
    }
    lastWhen = time(0);
    lastLine = line;

    log() << "warning assertion failure " << expr << ' ' << file << ' ' << dec << line << endl;
    logContext();
}

NOINLINE_DECL void verifyFailed(const char* expr, const char* file, unsigned line) {
    log() << "Assertion failure " << expr << ' ' << file << ' ' << dec << line << endl;
    logContext();
    stringstream temp;
    temp << "assertion " << file << ":" << line;
    AssertionException e(temp.str(), 0);
    throw e;
}

NOINLINE_DECL void invariantFailed(const char* expr, const char* file, unsigned line) {
    log() << "Invariant failure " << expr << ' ' << file << ' ' << dec << line << endl;
    logContext();
    log() << "\n\n***aborting after invariant() failure\n\n" << endl;
    abort();
}

NOINLINE_DECL void invariantOKFailed(const char* expr,
                                     const Status& status,
                                     const char* file,
                                     unsigned line) {
    log() << "Invariant failure: " << expr << " resulted in status " << status << " at " << file
          << ' ' << dec << line;
    logContext();
    log() << "\n\n***aborting after invariant() failure\n\n" << endl;
    abort();
}

NOINLINE_DECL void fassertFailed(int msgid) {
    log() << "Fatal Assertion " << msgid << endl;
    logContext();
    log() << "\n\n***aborting after fassert() failure\n\n" << endl;
    abort();
}

MONGO_COMPILER_NORETURN void fassertFailedWithStatus(int msgid, const Status& status) {
    log() << "Fatal assertion " << msgid << " " << status;
    logContext();
    log() << "\n\n***aborting after fassert() failure\n\n" << endl;
    abort();
}

MONGO_COMPILER_NORETURN void fassertFailedWithStatusNoTrace(int msgid, const Status& status) {
    log() << "Fatal assertion " << msgid << " " << status;
    logContext();
    log() << "\n\n***aborting after fassert() failure\n\n" << endl;
    abort();
}

void uasserted(int msgid, const string& msg) {
    uasserted(msgid, msg.c_str());
}

void UserException::appendPrefix(stringstream& ss) const {
    ss << "userassert:";
}
void MsgAssertionException::appendPrefix(stringstream& ss) const {
    ss << "massert:";
}

NOINLINE_DECL void uasserted(int msgid, const char* msg) {
    LOG(1) << "User Assertion: " << msgid << ":" << msg << endl;
    throw UserException(msgid, msg);
}

void msgasserted(int msgid, const string& msg) {
    msgasserted(msgid, msg.c_str());
}

NOINLINE_DECL void msgasserted(int msgid, const char* msg) {
    log() << "Assertion: " << msgid << ":" << msg << endl;
    logContext();
    throw MsgAssertionException(msgid, msg);
}

NOINLINE_DECL void msgassertedNoTrace(int msgid, const char* msg) {
    log() << "Assertion: " << msgid << ":" << msg << endl;
    throw MsgAssertionException(msgid, msg);
}

void msgassertedNoTrace(int msgid, const std::string& msg) {
    msgassertedNoTrace(msgid, msg.c_str());
}

std::string causedBy(const char* e) {
    return std::string(" :: caused by :: ") + e;
}

std::string causedBy(const DBException& e) {
    return causedBy(e.toString());
}

std::string causedBy(const std::exception& e) {
    return causedBy(e.what());
}

std::string causedBy(const std::string& e) {
    return causedBy(e.c_str());
}

std::string causedBy(const std::string* e) {
    return (e && *e != "") ? causedBy(*e) : "";
}

std::string causedBy(const Status& e) {
    return causedBy(e.reason());
}

string errnoWithPrefix(const char* prefix) {
    stringstream ss;
    if (prefix)
        ss << prefix << ": ";
    ss << errnoWithDescription();
    return ss.str();
}

string demangleName(const type_info& typeinfo) {
#ifdef _WIN32
    return typeinfo.name();
#else
    int status;

    char* niceName = abi::__cxa_demangle(typeinfo.name(), 0, 0, &status);
    if (!niceName)
        return typeinfo.name();

    string s = niceName;
    free(niceName);
    return s;
#endif
}

string ExceptionInfo::toString() const {
    stringstream ss;
    ss << "exception: " << code << " " << msg;
    return ss.str();
}

NOINLINE_DECL ErrorMsg::ErrorMsg(const char* msg, char ch) {
    int l = strlen(msg);
    verify(l < 128);
    memcpy(buf, msg, l);
    char* p = buf + l;
    p[0] = ch;
    p[1] = 0;
}

NOINLINE_DECL ErrorMsg::ErrorMsg(const char* msg, unsigned val) {
    int l = strlen(msg);
    verify(l < 128);
    memcpy(buf, msg, l);
    char* p = buf + l;
    sprintf(p, "%u", val);
}
}
