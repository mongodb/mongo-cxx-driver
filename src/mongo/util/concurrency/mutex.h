// @file mutex.h

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

#include <boost/noncopyable.hpp>

#include "mongo/util/time_support.h"

namespace mongo {

    inline boost::xtime incxtimemillis( long long s ) {
        boost::xtime xt;
        boost::xtime_get(&xt, MONGO_BOOST_TIME_UTC);
        xt.sec += (int)( s / 1000 );
        xt.nsec += (int)(( s % 1000 ) * 1000000);
        if ( xt.nsec >= 1000000000 ) {
            xt.nsec -= 1000000000;
            xt.sec++;
        }
        return xt;
    }

    // If you create a local static instance of this class, that instance will be destroyed
    // before all global static objects are destroyed, so _destroyingStatics will be set
    // to true before the global static variables are destroyed.
    class StaticObserver : boost::noncopyable {
    public:
        static bool _destroyingStatics;
        ~StaticObserver() { _destroyingStatics = true; }
    };

}
