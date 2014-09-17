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

#include <iosfwd>
#include <string>

#include "mongo/bson/util/builder.h"
#include "mongo/platform/hash_namespace.h"

namespace mongo {
    class Status;
    class StringData;
    template <typename T> class StatusWith;

    /**
     * Name of a process on the network.
     *
     * Composed of some name component, followed optionally by a colon and a numeric port.  The name
     * might be an IPv4 or IPv6 address or a relative or fully qualified host name, or an absolute
     * path to a unix socket.
     */
    struct MONGO_CLIENT_API HostAndPort {

        /**
         * Parses "text" to produce a HostAndPort.  Returns either that or an error
         * status describing the parse failure.
         */
        static StatusWith<HostAndPort> MONGO_CLIENT_FUNC parse(const StringData& text);

        /**
         * Construct an empty/invalid HostAndPort.
         */
        HostAndPort();

        /**
         * Constructs a HostAndPort by parsing "text" of the form hostname[:portnumber]
         * Throws an AssertionException if bad config std::string or bad port #.
         */
        explicit HostAndPort(const StringData& text);

        /**
         * Constructs a HostAndPort with the hostname "h" and port "p".
         *
         * If "p" is -1, port() returns ServerGlobalParams::DefaultDBPort.
         */
        HostAndPort(const std::string& h, int p);

        /**
         * (Re-)initializes this HostAndPort by parsing "s".  Returns
         * Status::OK on success.  The state of this HostAndPort is unspecified
         * after initialize() returns a non-OK status, though it is safe to
         * assign to it or re-initialize it.
         */
        Status initialize(const StringData& s);

        bool operator<(const HostAndPort& r) const;
        bool operator==(const HostAndPort& r) const;
        bool operator!=(const HostAndPort& r) const { return !(*this == r); }

        /**
         * Returns true if the hostname looks localhost-y.
         *
         * TODO: Make a more rigorous implementation, perhaps elsewhere in
         * the networking library.
         */
        bool isLocalHost() const;

        /**
         * Returns a string representation of "host:port".
         */
        std::string toString() const;

        /**
         * Like toString(), above, but writes to "ss", instead.
         */
        void append( StringBuilder& ss ) const;

        /**
         * Returns true if this object represents no valid HostAndPort.
         */
        bool empty() const;

        const std::string& host() const {
            return _host;
        }
        int port() const;

        bool hasPort() const {
            return _port >= 0;
        }

    private:
        std::string _host;
        int _port; // -1 indicates unspecified
    };

    MONGO_CLIENT_API std::ostream& MONGO_CLIENT_FUNC operator<<(std::ostream& os, const HostAndPort& hp);

}  // namespace mongo

MONGO_HASH_NAMESPACE_START

template <>
struct hash<mongo::HostAndPort> {
    size_t operator()(const mongo::HostAndPort& host) const;
};

MONGO_HASH_NAMESPACE_END
