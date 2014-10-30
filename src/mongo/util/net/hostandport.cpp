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

#include "mongo/platform/basic.h"

#include "mongo/util/net/hostandport.h"

#include <boost/functional/hash.hpp>

#include "mongo/base/parse_number.h"
#include "mongo/base/status.h"
#include "mongo/base/status_with.h"
#include "mongo/base/string_data.h"
#include "mongo/bson/util/builder.h"
#include "mongo/client/options.h"
#include "mongo/db/jsobj.h"
#include "mongo/util/mongoutils/str.h"
#include "mongo/util/net/sock.h"
#include "mongo/util/assert_util.h"

namespace mongo {

    StatusWith<HostAndPort> HostAndPort::parse(const StringData& text) {
        HostAndPort result;
        Status status = result.initialize(text);
        if (!status.isOK()) {
            return StatusWith<HostAndPort>(status);
        }
        return StatusWith<HostAndPort>(result);
    }

    HostAndPort::HostAndPort() : _port(-1) {}

    HostAndPort::HostAndPort(const StringData& text) {
        uassertStatusOK(initialize(text));
    }

    HostAndPort::HostAndPort(const std::string& h, int p) : _host(h), _port(p) {}

    bool HostAndPort::operator<(const HostAndPort& r) const {
        const int cmp = host().compare(r.host());
        if (cmp)
            return cmp < 0;
        return port() < r.port();
    }

    bool HostAndPort::operator==(const HostAndPort& r) const {
        return host() == r.host() && port() == r.port();
    }

    int HostAndPort::port() const {
        if (hasPort())
            return _port;
        return client::Options::kDbServer;
    }

    bool HostAndPort::isLocalHost() const {
        return (  _host == "localhost"
               || str::startsWith(_host.c_str(), "127.")
               || _host == "::1"
               || _host == "anonymous unix socket"
               || _host.c_str()[0] == '/' // unix socket
               );
    }

    std::string HostAndPort::toString() const {
        StringBuilder ss;
        append( ss );
        return ss.str();
    }

    void HostAndPort::append(StringBuilder& ss) const {
        // wrap ipv6 addresses in []s for roundtrip-ability
        if (host().find(':') != std::string::npos) {
            ss << '[' << host() << ']';
        }
        else {
            ss << host();
        }
        ss << ':' << port();
    }

    bool HostAndPort::empty() const {
        return _host.empty() && _port < 0;
    }

    Status HostAndPort::initialize(const StringData& s) {
        size_t colonPos = s.rfind(':');
        StringData hostPart = s.substr(0, colonPos);

        // handle ipv6 hostPart (which we require to be wrapped in []s)
        const size_t openBracketPos = s.find('[');
        const size_t closeBracketPos = s.find(']');
        if (openBracketPos != std::string::npos) {
            if (openBracketPos != 0) {
                return Status(ErrorCodes::FailedToParse,
                              str::stream() << "'[' present, but not first character in "
                                            << s.toString());
            }
            if (closeBracketPos == std::string::npos) {
                return Status(ErrorCodes::FailedToParse,
                              str::stream() << "ipv6 address is missing closing ']' in hostname in "
                                            << s.toString());
            }

            hostPart = s.substr(openBracketPos+1, closeBracketPos-openBracketPos-1);
            // prevent accidental assignment of port to the value of the final portion of hostPart
            if (colonPos < closeBracketPos) {
                colonPos = std::string::npos;
            }
            else if (colonPos != closeBracketPos+1) {
                return Status(ErrorCodes::FailedToParse,
                              str::stream() << "Extraneous characters between ']' and pre-port ':'"
                                            << " in " << s.toString());
            }
        }
        else if (closeBracketPos != std::string::npos) {
            return Status(ErrorCodes::FailedToParse,
                          str::stream() << "']' present without '[' in " << s.toString());
        }
        else if (s.find(':') != colonPos) {
            return Status(ErrorCodes::FailedToParse,
                          str::stream() << "More than one ':' detected. If this is an ipv6 address,"
                                        << " it needs to be surrounded by '[' and ']'; "
                                        << s.toString());
        }

        if (hostPart.empty()) {
            return Status(ErrorCodes::FailedToParse, str::stream() <<
                          "Empty host component parsing HostAndPort from \"" <<
                          escape(s.toString()) << "\"");
        }

        int port;
        if (colonPos != std::string::npos) {
            const StringData portPart = s.substr(colonPos + 1);
            Status status = parseNumberFromStringWithBase(portPart, 10, &port);
            if (!status.isOK()) {
                return status;
            }
            if (port <= 0) {
                return Status(ErrorCodes::FailedToParse, str::stream() << "Port number " << port <<
                              " out of range parsing HostAndPort from \"" << escape(s.toString()) <<
                              "\"");
            }
        }
        else {
            port = -1;
        }
        _host = hostPart.toString();
        _port = port;
        return Status::OK();
    }

    std::ostream& operator<<(std::ostream& os, const HostAndPort& hp) {
        return os << hp.toString();
    }

}  // namespace mongo

MONGO_HASH_NAMESPACE_START
size_t hash<mongo::HostAndPort>::operator()(const mongo::HostAndPort& host) const {
    hash<int> intHasher;
    size_t hash = intHasher(host.port());
    boost::hash_combine(hash, host.host());
    return hash;
}
MONGO_HASH_NAMESPACE_END
