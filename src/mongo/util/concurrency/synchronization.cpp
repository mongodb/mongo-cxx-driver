// synchronization.cpp

/*    Copyright 2010 10gen Inc.
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

#include "mongo/util/concurrency/synchronization.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/locks.hpp>

#include "mongo/util/assert_util.h"

namespace mongo {

    Notification::Notification() : _mutex() {
        lookFor = 1;
        cur = 0;
    }

    void Notification::waitToBeNotified() {
        boost::unique_lock<boost::mutex> lock( _mutex );
        while ( lookFor != cur )
            _condition.wait( lock );
        lookFor++;
    }

    void Notification::notifyOne() {
        boost::lock_guard<boost::mutex> lock( _mutex );
        verify( cur != lookFor );
        cur++;
        _condition.notify_one();
    }

    /* --- NotifyAll --- */

    NotifyAll::NotifyAll() : _mutex() {
        _lastDone = 0;
        _lastReturned = 0;
        _nWaiting = 0;
    }

    NotifyAll::When NotifyAll::now() { 
        boost::lock_guard<boost::mutex> lock( _mutex );
        return ++_lastReturned;
    }

    void NotifyAll::waitFor(When e) {
        boost::unique_lock<boost::mutex> lock( _mutex );
        ++_nWaiting;
        while( _lastDone < e ) {
            _condition.wait( lock );
        }
    }

    void NotifyAll::awaitBeyondNow() { 
        boost::unique_lock<boost::mutex> lock( _mutex );
        ++_nWaiting;
        When e = ++_lastReturned;
        while( _lastDone <= e ) {
            _condition.wait( lock );
        }
    }

    void NotifyAll::notifyAll(When e) {
        boost::lock_guard<boost::mutex> lock( _mutex );
        _lastDone = e;
        _nWaiting = 0;
        _condition.notify_all();
    }

} // namespace mongo
