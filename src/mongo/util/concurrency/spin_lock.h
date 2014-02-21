// spin_lock.h

/*    Copyright 2014 MongoDB Inc.
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

#ifdef _WIN32
#include "mongo/platform/windows_basic.h"
#endif

#include "mutex.h"

namespace mongo {

    /**
     * The spinlock currently requires late GCC support routines to be efficient.
     * Other platforms default to a mutex implemenation.
     */
    class SpinLock : boost::noncopyable {
    public:
        SpinLock();
        ~SpinLock();

        static bool isfast(); // true if a real spinlock on this platform

    private:
#if defined(_WIN32)
        CRITICAL_SECTION _cs;
    public:
        void lock() {EnterCriticalSection(&_cs); }
        void unlock() { LeaveCriticalSection(&_cs); }
#elif defined(__USE_XOPEN2K)
        pthread_spinlock_t _lock;
        void _lk();
    public:
        void unlock() { pthread_spin_unlock(&_lock); }
        void lock() {
            if ( MONGO_likely( pthread_spin_trylock( &_lock ) == 0 ) )
                return;
            _lk(); 
        }
#elif defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4)
        volatile bool _locked;
    public:
        void unlock() {__sync_lock_release(&_locked); }
        void lock();
#else
        // default to a mutex if not implemented
        SimpleMutex _mutex;
    public:
        void unlock() { _mutex.unlock(); }
        void lock() { _mutex.lock(); }
#endif
    };
    
    class scoped_spinlock : boost::noncopyable {
    public:
        scoped_spinlock( SpinLock& l ) : _l(l) {
            _l.lock();
        }
        ~scoped_spinlock() {
            _l.unlock();}
    private:
        SpinLock& _l;
    };

}  // namespace mongo
