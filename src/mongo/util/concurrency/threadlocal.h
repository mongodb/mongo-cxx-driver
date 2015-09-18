#pragma once

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

#include "mongo/client/undef_macros.h"
#include <boost/thread/tss.hpp>
#include "mongo/client/redef_macros.h"


namespace mongo {

/* thread local "value" rather than a pointer
   good for things which have copy constructors (and the copy constructor is fast enough)
   e.g.
     ThreadLocalValue<int> myint;
*/
template <class T>
class ThreadLocalValue {
public:
    ThreadLocalValue(T def = 0) : _default(def) {}

    T get() const {
        T* val = _val.get();
        if (val)
            return *val;
        return _default;
    }

    void set(const T& i) {
        T* v = _val.get();
        if (v) {
            *v = i;
            return;
        }
        v = new T(i);
        _val.reset(v);
    }

    T& getRef() {
        T* v = _val.get();
        if (v) {
            return *v;
        }
        v = new T(_default);
        _val.reset(v);
        return *v;
    }

private:
    boost::thread_specific_ptr<T> _val;
    const T _default;
};

/* TSP
   These macros use intrinsics which are faster than boost::thread_specific_ptr.
   However the intrinsics don't free up objects on thread closure. Thus we use
   a combination here, with the assumption that reset's are infrequent, so that
   get's are fast.
*/
#if defined(MONGO_HAVE___THREAD) || defined(MONGO_HAVE___DECLSPEC_THREAD)

template <class T>
struct TSP {
    boost::thread_specific_ptr<T> tsp;

public:
    T* get() const;
    void reset(T* v);
    T* getMake() {
        T* t = get();
        if (t == 0)
            reset(t = new T());
        return t;
    }
};

#if defined(MONGO_HAVE___DECLSPEC_THREAD)

#define TSP_DECLARE(T, p) extern TSP<T> p;

#define TSP_DEFINE(T, p)        \
    __declspec(thread) T* _##p; \
    TSP<T> p;                   \
    template <>                 \
    T* TSP<T>::get() const {    \
        return _##p;            \
    }                           \
    void TSP<T>::reset(T* v) {  \
        tsp.reset(v);           \
        _##p = v;               \
    }
#else

#define TSP_DECLARE(T, p)           \
    extern __thread T* _##p;        \
    template <>                     \
    inline T* TSP<T>::get() const { \
        return _##p;                \
    }                               \
    extern TSP<T> p;

#define TSP_DEFINE(T, p)       \
    __thread T* _##p;          \
    template <>                \
    void TSP<T>::reset(T* v) { \
        tsp.reset(v);          \
        _##p = v;              \
    }                          \
    TSP<T> p;
#endif

#elif defined(_POSIX_THREADS) && (_POSIX_THREADS >= 0)
template <class T>
struct TSP {
    pthread_key_t _key;

public:
    TSP() {
        verify(pthread_key_create(&_key, TSP::dodelete) == 0);
    }

    ~TSP() {
        pthread_key_delete(_key);
    }

    static void dodelete(void* x) {
        T* t = reinterpret_cast<T*>(x);
        delete t;
    }

    T* get() const {
        return reinterpret_cast<T*>(pthread_getspecific(_key));
    }

    void reset(T* v) {
        T* old = get();
        delete old;
        verify(pthread_setspecific(_key, v) == 0);
    }

    T* getMake() {
        T* t = get();
        if (t == 0) {
            t = new T();
            reset(t);
        }
        return t;
    }
};

#define TSP_DECLARE(T, p) extern TSP<T> p;

#define TSP_DEFINE(T, p) TSP<T> p;

#else

template <class T>
struct TSP {
    boost::thread_specific_ptr<T> tsp;

public:
    T* get() const {
        return tsp.get();
    }
    void reset(T* v) {
        tsp.reset(v);
    }
    T* getMake() {
        T* t = get();
        if (t == 0)
            reset(t = new T());
        return t;
    }
};

#define TSP_DECLARE(T, p) extern TSP<T> p;

#define TSP_DEFINE(T, p) TSP<T> p;

#endif
}
