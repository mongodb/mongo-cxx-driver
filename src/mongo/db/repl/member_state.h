/*
 *    Copyright (C) 2010 10gen Inc.
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

/** replica set member */

#pragma once

#include <string>

namespace mongo {
namespace repl {


    /*
        RS_STARTUP    serving still starting up, or still trying to initiate the set
        RS_PRIMARY    this server thinks it is primary
        RS_SECONDARY  this server thinks it is a secondary (slave mode)
        RS_RECOVERING recovering/resyncing; after recovery usually auto-transitions to secondary
        RS_STARTUP2   loaded config, still determining who is primary

        State -> integer mappings are reserved forever.  Do not change them or delete them, except
        to update RS_MAX when introducing new states.
    */
    struct MemberState {
        enum MS {
            RS_STARTUP = 0,
            RS_PRIMARY = 1,
            RS_SECONDARY = 2,
            RS_RECOVERING = 3,
            RS_STARTUP2 = 5,
            RS_UNKNOWN = 6, /* remote node not yet reached */
            RS_ARBITER = 7,
            RS_DOWN = 8, /* node not reachable for a report */
            RS_ROLLBACK = 9,
            RS_REMOVED = 10, /* node removed from replica set */
            RS_MAX = 10
        } s;

        MemberState(MS ms = RS_UNKNOWN) : s(ms) { }
        explicit MemberState(int ms) : s((MS) ms) { }

        bool startup() const { return s == RS_STARTUP; }
        bool primary() const { return s == RS_PRIMARY; }
        bool secondary() const { return s == RS_SECONDARY; }
        bool recovering() const { return s == RS_RECOVERING; }
        bool startup2() const { return s == RS_STARTUP2; }
        bool rollback() const { return s == RS_ROLLBACK; }
        bool readable() const { return s == RS_PRIMARY || s == RS_SECONDARY; }
        bool removed() const { return s == RS_REMOVED; }
        bool arbiter() const { return s == RS_ARBITER; }

        std::string toString() const;

        bool operator==(const MemberState& r) const { return s == r.s; }
        bool operator!=(const MemberState& r) const { return s != r.s; }
    };

    inline std::string MemberState::toString() const {
        switch ( s ) {
        case RS_STARTUP: return "STARTUP";
        case RS_PRIMARY: return "PRIMARY";
        case RS_SECONDARY: return "SECONDARY";
        case RS_RECOVERING: return "RECOVERING";
        case RS_STARTUP2: return "STARTUP2";
        case RS_ARBITER: return "ARBITER";
        case RS_DOWN: return "DOWN";
        case RS_ROLLBACK: return "ROLLBACK";
        case RS_UNKNOWN: return "UNKNOWN";
        case RS_REMOVED: return "REMOVED";
        }
        return "";
    }

} // namespace repl
} // namespace mongo
