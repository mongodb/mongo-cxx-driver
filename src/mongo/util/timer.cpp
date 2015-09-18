// @file mongo/util/timer.cpp

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

#include "mongo/util/timer.h"

#include <ctime>
#include <limits>
#if defined(MONGO_HAVE_HEADER_UNISTD_H)
#include <unistd.h>
#endif

#include "mongo/util/assert_util.h"
#include "mongo/util/time_support.h"

namespace mongo {

// Set default value to reflect "generic" timer implementation.
// Define Timer::_countsPerSecond before static initializer "atstartuputil" to ensure correct
// relative sequencing regardless of how _countsPerSecond is initialized (static or dynamic).
long long Timer::_countsPerSecond = Timer::microsPerSecond;
double Timer::_microsPerCount = 1.0f;

namespace {

// TODO: SERVER-5112, better startup-time initialization of C++ modules.
struct AtStartup {
    AtStartup();
} atstartuputil;

// "Generic" implementation for Timer::now().
long long _timerNowGeneric() {
    return curTimeMicros64();
}

// Function pointer to Timer::now() implementation.
// Overridden in AtStartup() with better implementation where available.
long long (*_timerNow)() = &_timerNowGeneric;

#if defined(_WIN32)

/**
 * Windows-specific implementation of the
 * Timer class.  Windows selects the best available timer, in its estimation, for
 * measuring time at high resolution.  This may be the HPET of the TSC on x86 systems,
 * but is promised to be synchronized across processors, barring BIOS errors.
 */
long long timerNowWindows() {
    LARGE_INTEGER i;
    fassert(16161, QueryPerformanceCounter(&i));
    return i.QuadPart;
}

AtStartup::AtStartup() {
    LARGE_INTEGER x;
    bool ok = QueryPerformanceFrequency(&x);
    verify(ok);
    Timer::setCountsPerSecond(x.QuadPart);
    _timerNow = &timerNowWindows;
}

#elif defined(MONGO_HAVE_POSIX_MONOTONIC_CLOCK)

/**
 * Implementation for timer on systems that support the
 * POSIX clock API and CLOCK_MONOTONIC clock.
 */
long long timerNowPosixMonotonicClock() {
    timespec the_time;
    long long result;

    fassert(16160, !clock_gettime(CLOCK_MONOTONIC, &the_time));

    // Safe for 292 years after the clock epoch, even if we switch to a signed time value.
    // On Linux, the monotonic clock's epoch is the UNIX epoch.
    result = static_cast<long long>(the_time.tv_sec);
    result *= Timer::nanosPerSecond;
    result += static_cast<long long>(the_time.tv_nsec);
    return result;
}

AtStartup::AtStartup() {
    // If the monotonic clock is not available at runtime (sysconf() returns 0 or -1),
    // do not override the generic implementation or modify Timer::_countsPerSecond.
    if (sysconf(_SC_MONOTONIC_CLOCK) <= 0) {
        return;
    }

    Timer::setCountsPerSecond(Timer::nanosPerSecond);
    _timerNow = &timerNowPosixMonotonicClock;

    // Make sure that the current time relative to the (unspecified) epoch isn't already too
    // big to represent as a 64-bit count of nanoseconds.
    long long maxSecs = std::numeric_limits<long long>::max() / Timer::nanosPerSecond;
    timespec the_time;
    fassert(16162, !clock_gettime(CLOCK_MONOTONIC, &the_time));
    fassert(16163, static_cast<long long>(the_time.tv_sec) < maxSecs);
}
#else
AtStartup::AtStartup() {}
#endif

}  // namespace

long long Timer::now() const {
    return _timerNow();
}

}  // namespace mongo
