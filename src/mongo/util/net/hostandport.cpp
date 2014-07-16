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

#include "mongo/base/parse_number.h"
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

    void HostAndPort::append( StringBuilder& ss ) const {
        ss << host() << ':' << port();
    }

    bool HostAndPort::empty() const {
        return _host.empty() && _port < 0;
    }

    Status HostAndPort::initialize(const StringData& s) {
        const size_t colonPos = s.rfind(':');
        const StringData hostPart = s.substr(0, colonPos);
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
