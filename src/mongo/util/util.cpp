// @file util.cpp

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

#include <iomanip>

#include "mongo/platform/atomic_word.h"
#include "mongo/util/concurrency/mutex.h"
#include "mongo/util/goodies.h"
#include "mongo/util/mongoutils/str.h"
#include "mongo/util/time_support.h"
#include "mongo/util/timer.h"

namespace mongo {

    using std::ostream;
    using std::setfill;
    using std::setw;
    using std::string;
    using std::stringstream;

    string hexdump(const char *data, unsigned len) {
        verify( len < 1000000 );
        const unsigned char *p = (const unsigned char *) data;
        stringstream ss;
        ss << std::hex << setw(2) << setfill('0');
        for( unsigned i = 0; i < len; i++ ) {
            ss << static_cast<unsigned>(p[i]) << ' ';
        }
        string s = ss.str();
        return s;
    }

    bool isPrime(int n) {
        int z = 2;
        while ( 1 ) {
            if ( z*z > n )
                break;
            if ( n % z == 0 )
                return false;
            z++;
        }
        return true;
    }

    int nextPrime(int n) {
        n |= 1; // 2 goes to 3...don't care...
        while ( !isPrime(n) )
            n += 2;
        return n;
    }

    ostream& operator<<( ostream &s, const ThreadSafeString &o ) {
        s << o.toString();
        return s;
    }

    bool StaticObserver::_destroyingStatics = false;

} // namespace mongo
