// @file timer.h

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

#pragma once

#include "mongo/client/export_macros.h"

namespace mongo {

/**
 * Time tracking object.
 *
 * Should be of reasonably high performance, though the implementations are platform-specific.
 * Each platform provides a distinct implementation of the now() method, and sets the
 * _countsPerSecond static field to the constant number of ticks per second that now() counts
 * in.  The maximum span measurable by the counter and convertible to microseconds is about 10
 * trillion ticks.  As long as there are fewer than 100 ticks per nanosecond, timer durations of
 * 2.5 years will be supported.  Since a typical tick duration will be under 10 per nanosecond,
 * if not below 1 per nanosecond, this should not be an issue.
 */
class Timer /*copyable*/ {
public:
    static const long long millisPerSecond = 1000;
    static const long long microsPerSecond = 1000 * millisPerSecond;
    static const long long nanosPerSecond = 1000 * microsPerSecond;

    Timer() {
        reset();
    }
    int seconds() const {
        return (int)(micros() / 1000000);
    }
    int millis() const {
        return (int)(micros() / 1000);
    }
    int minutes() const {
        return seconds() / 60;
    }


    /** Get the time interval and reset at the same time.
     *  @return time in milliseconds.
     */
    inline int millisReset() {
        const long long nextNow = now();
        const long long deltaMicros = static_cast<long long>((nextNow - _old) * _microsPerCount);

        _old = nextNow;
        return static_cast<int>(deltaMicros / 1000);
    }

    inline long long micros() const {
        return static_cast<long long>((now() - _old) * _microsPerCount);
    }

    inline void reset() {
        _old = now();
    }

    inline static void setCountsPerSecond(long long countsPerSecond) {
        _countsPerSecond = countsPerSecond;
        _microsPerCount = static_cast<double>(microsPerSecond) / _countsPerSecond;
    }

    inline static long long getCountsPerSecond() {
        return _countsPerSecond;
    }

private:
    /**
     * Internally, the timer counts platform-dependent ticks of some sort, and
     * must then convert those ticks to microseconds and their ilk.  This field
     * stores the frequency of the platform-dependent counter.
     */
    static long long _countsPerSecond;

    // Derived value from _countsPerSecond. This represents the conversion ratio
    // from clock ticks to microseconds.
    static double _microsPerCount;

    long long now() const;

    long long _old;
};
}  // namespace mongo
