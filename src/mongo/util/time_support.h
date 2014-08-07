// @file time_support.h

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

#include <iosfwd>
#include <ctime>
#include <string>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/version.hpp>

#include "mongo/base/status_with.h"
#include "mongo/client/export_macros.h"

namespace mongo {

    typedef boost::posix_time::milliseconds Milliseconds;
    typedef boost::posix_time::seconds Seconds;

    void time_t_to_Struct(time_t t, struct tm * buf , bool local = false );
    std::string time_t_to_String(time_t t);
    std::string time_t_to_String_short(time_t t);

    struct MONGO_CLIENT_API Date_t {
        // TODO: make signed (and look for related TODO's)
        unsigned long long millis;
        Date_t(): millis(0) {}
        Date_t(unsigned long long m): millis(m) {}
        operator unsigned long long&() { return millis; }
        operator const unsigned long long&() const { return millis; }
        void toTm (tm *buf);
        std::string toString() const;
        time_t toTimeT() const;
        int64_t asInt64() const {
            return static_cast<int64_t>(millis);
        }
        bool isFormatable() const;
    };

    // uses ISO 8601 dates without trailing Z
    // colonsOk should be false when creating filenames
    MONGO_CLIENT_API std::string MONGO_CLIENT_FUNC terseCurrentTime(bool colonsOk=true);

    /**
     * Formats "time" according to the ISO 8601 extended form standard, including date,
     * and time, in the UTC timezone.
     *
     * Sample format: "2013-07-23T18:42:14Z"
     */
    MONGO_CLIENT_API std::string MONGO_CLIENT_FUNC timeToISOString(time_t time);

    /**
     * Formats "date" according to the ISO 8601 extended form standard, including date,
     * and time with milliseconds decimal component, in the UTC timezone.
     *
     * Sample format: "2013-07-23T18:42:14.072Z"
     */
    MONGO_CLIENT_API std::string MONGO_CLIENT_FUNC dateToISOStringUTC(Date_t date);

    /**
     * Formats "date" according to the ISO 8601 extended form standard, including date,
     * and time with milliseconds decimal component, in the local timezone.
     *
     * Sample format: "2013-07-23T18:42:14.072-05:00"
     */
    MONGO_CLIENT_API std::string MONGO_CLIENT_FUNC dateToISOStringLocal(Date_t date);

    /**
     * Formats "date" in fixed width in the local time zone.
     *
     * Sample format: "Wed Oct 31 13:34:47.996"
     */
    MONGO_CLIENT_API std::string MONGO_CLIENT_FUNC dateToCtimeString(Date_t date);

    /**
     * Parses a Date_t from an ISO 8601 string representation.
     *
     * Sample formats: "2013-07-23T18:42:14.072-05:00"
     *                 "2013-07-23T18:42:14.072Z"
     *
     * Local times are currently not supported.
     */
    MONGO_CLIENT_API StatusWith<Date_t> MONGO_CLIENT_FUNC dateFromISOString(const StringData& dateString);

    /**
     * Like dateToISOStringUTC, except outputs to a std::ostream.
     */
    MONGO_CLIENT_API void MONGO_CLIENT_FUNC outputDateAsISOStringUTC(std::ostream& os, Date_t date);

    /**
     * Like dateToISOStringLocal, except outputs to a std::ostream.
     */
    MONGO_CLIENT_API void MONGO_CLIENT_FUNC outputDateAsISOStringLocal(std::ostream& os, Date_t date);

    /**
     * Like dateToCtimeString, except outputs to a std::ostream.
     */
    MONGO_CLIENT_API void MONGO_CLIENT_FUNC outputDateAsCtime(std::ostream& os, Date_t date);

    boost::gregorian::date currentDate();

    // parses time of day in "hh:mm" format assuming 'hh' is 00-23
    bool toPointInTime( const std::string& str , boost::posix_time::ptime* timeOfDay );

    MONGO_CLIENT_API void MONGO_CLIENT_FUNC sleepsecs(int s);
    MONGO_CLIENT_API void MONGO_CLIENT_FUNC sleepmillis(long long ms);
    MONGO_CLIENT_API void MONGO_CLIENT_FUNC sleepmicros(long long micros);

    class Backoff {
    public:

        Backoff( int maxSleepMillis, int resetAfter ) :
            _maxSleepMillis( maxSleepMillis ),
            _resetAfterMillis( maxSleepMillis + resetAfter ), // Don't reset < the max sleep
            _lastSleepMillis( 0 ),
            _lastErrorTimeMillis( 0 )
        {}

        void nextSleepMillis();

        /**
         * testing-only function. used in dbtests/basictests.cpp
         */
        int getNextSleepMillis(int lastSleepMillis, unsigned long long currTimeMillis,
                               unsigned long long lastErrorTimeMillis) const;

    private:

        // Parameters
        int _maxSleepMillis;
        int _resetAfterMillis;

        // Last sleep information
        int _lastSleepMillis;
        unsigned long long _lastErrorTimeMillis;
    };

    // DO NOT TOUCH except for testing
    void jsTimeVirtualSkew( long long skew );

    void jsTimeVirtualThreadSkew( long long skew );
    long long getJSTimeVirtualThreadSkew();

    /** Date_t is milliseconds since epoch */
    MONGO_CLIENT_API Date_t MONGO_CLIENT_FUNC jsTime();

    /** warning this will wrap */
    unsigned curTimeMicros();
    unsigned long long curTimeMicros64();
    unsigned long long curTimeMillis64();

    // these are so that if you use one of them compilation will fail
    char *asctime(const struct tm *tm);
    char *ctime(const time_t *timep);
    struct tm *gmtime(const time_t *timep);
    struct tm *localtime(const time_t *timep);

#if (BOOST_VERSION >= 105000)
#define MONGO_BOOST_TIME_UTC boost::TIME_UTC_
#else
#define MONGO_BOOST_TIME_UTC boost::TIME_UTC
#endif

}  // namespace mongo

