/* @file value.h
   concurrency helpers DiagStr, Guarded
*/

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

#include "spin_lock.h"

namespace mongo {

    /** declare that a variable that is "guarded" by a mutex.

        The decl documents the rule.  For example "counta and countb are guarded by xyzMutex":

          Guarded<int, xyzMutex> counta;
          Guarded<int, xyzMutex> countb;

        Upon use, specify the scoped_lock object.  This makes it hard for someone 
        later to forget to be in the lock.  Check is made that it is the right lock in _DEBUG
        builds at runtime.
    */
    template <typename T, SimpleMutex& BY>
    class Guarded {
        T _val;
    public:
        T& ref(const SimpleMutex::scoped_lock& lk) {
            dassert( &lk.m() == &BY );
            return _val;
        }
    };

    // todo: rename this to ThreadSafeString or something
    /** there is now one mutex per DiagStr.  If you have hundreds or millions of
        DiagStrs you'll need to do something different.
    */
    class DiagStr {
        mutable SpinLock m;
        string _s;
    public:
        DiagStr(const DiagStr& r) : _s(r.get()) { }
        DiagStr(const string& r) : _s(r) { }
        DiagStr() { }
        bool empty() const { 
            scoped_spinlock lk(m);
            return _s.empty();
        }
        string get() const { 
            scoped_spinlock lk(m);
            return _s;
        }
        void set(const char *s) {
            scoped_spinlock lk(m);
            _s = s;
        }
        void set(const string& s) { 
            scoped_spinlock lk(m);
            _s = s;
        }
        operator string() const { return get(); }
        void operator=(const string& s) { set(s); }
        void operator=(const DiagStr& rhs) { 
            set( rhs.get() );
        }

        // == is not defined.  use get() == ... instead.  done this way so one thinks about if composing multiple operations
        bool operator==(const string& s) const; 
    };

}
