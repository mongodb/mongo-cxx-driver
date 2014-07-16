// @file log.h

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

#include "mongo/base/status.h"
#include "mongo/bson/util/builder.h"
#include "mongo/logger/log_component.h"
#include "mongo/logger/logger.h"
#include "mongo/logger/logstream_builder.h"
#include "mongo/logger/tee.h"
#include "mongo/util/concurrency/thread_name.h"

namespace mongo {

namespace logger {
    typedef void (*ExtraLogContextFn)(BufBuilder& builder);
    Status registerExtraLogContextFn(ExtraLogContextFn contextFn);

}  // namespace logger

    using logger::LogstreamBuilder;
    using logger::LabeledLevel;
    using logger::Tee;

    /**
     * Returns a LogstreamBuilder for logging a message with LogSeverity::Severe().
     */
    inline LogstreamBuilder severe() {
        return LogstreamBuilder(logger::globalLogDomain(),
                                getThreadName(),
                                logger::LogSeverity::Severe());
    }

    /**
     * Returns a LogstreamBuilder for logging a message with LogSeverity::Error().
     */
    inline LogstreamBuilder error() {
        return LogstreamBuilder(logger::globalLogDomain(),
                                getThreadName(),
                                logger::LogSeverity::Error());
    }

    /**
     * Returns a LogstreamBuilder for logging a message with LogSeverity::Warning().
     */
    inline LogstreamBuilder warning() {
        return LogstreamBuilder(logger::globalLogDomain(),
                                getThreadName(),
                                logger::LogSeverity::Warning());
    }

    /**
     * Returns a LogstreamBuilder for logging a message with LogSeverity::Log().
     */
    inline LogstreamBuilder log() {
        return LogstreamBuilder(logger::globalLogDomain(),
                                getThreadName(),
                                logger::LogSeverity::Log());
    }


// MONGO_LOG uses log component from MongoLogDefaultComponent from current or global namespace.
#define MONGO_LOG(DLEVEL) \
    if (!(::mongo::logger::globalLogDomain())->shouldLog(MongoLogDefaultComponent_component, ::mongo::LogstreamBuilder::severityCast(DLEVEL))) {} \
    else LogstreamBuilder(::mongo::logger::globalLogDomain(), getThreadName(), ::mongo::LogstreamBuilder::severityCast(DLEVEL))

#define LOG MONGO_LOG

#define MONGO_LOG_COMPONENT(DLEVEL, COMPONENT1) \
    if (!(::mongo::logger::globalLogDomain())->shouldLog((COMPONENT1), ::mongo::LogstreamBuilder::severityCast(DLEVEL))) {} \
    else LogstreamBuilder(::mongo::logger::globalLogDomain(), getThreadName(), ::mongo::LogstreamBuilder::severityCast(DLEVEL))

#define MONGO_LOG_COMPONENT2(DLEVEL, COMPONENT1, COMPONENT2) \
    if (!(::mongo::logger::globalLogDomain())->shouldLog((COMPONENT1), (COMPONENT2), ::mongo::LogstreamBuilder::severityCast(DLEVEL))) {} \
    else LogstreamBuilder(::mongo::logger::globalLogDomain(), getThreadName(), ::mongo::LogstreamBuilder::severityCast(DLEVEL))

#define MONGO_LOG_COMPONENT3(DLEVEL, COMPONENT1, COMPONENT2, COMPONENT3) \
    if (!(::mongo::logger::globalLogDomain())->shouldLog((COMPONENT1), (COMPONENT2), (COMPONENT3), ::mongo::LogstreamBuilder::severityCast(DLEVEL))) {} \
    else LogstreamBuilder(::mongo::logger::globalLogDomain(), getThreadName(), ::mongo::LogstreamBuilder::severityCast(DLEVEL))


    /** output the error # and error message with prefix.
        handy for use as parm in uassert/massert.
        */
    std::string errnoWithPrefix( const char * prefix );

    std::string errnoWithDescription(int errorcode = -1);

    /**
     * Write the optional "msg".
     */
    void logContext(const char *msg = NULL);

} // namespace mongo

/**
 * Defines default log component for MONGO_LOG.
 * Use this macro inside an implementation namespace or code block where debug messages
 * are logged using MONGO_LOG().
 *
 * Note: Do not use more than once inside any namespace/code block.
 *       Using static function instead of enum to support use inside function code block.
 */
#define MONGO_LOG_DEFAULT_COMPONENT_FILE(COMPONENT) \
    static const ::mongo::logger::LogComponent MongoLogDefaultComponent_component = (COMPONENT);

/**
 * MONGO_LOG_DEFAULT_COMPONENT for local code block.
 */
#define MONGO_LOG_DEFAULT_COMPONENT_LOCAL(COMPONENT) \
    const ::mongo::logger::LogComponent MongoLogDefaultComponent_component = (COMPONENT);

// Provide log component in global scope so that MONGO_LOG will always have a valid component.
const ::mongo::logger::LogComponent MongoLogDefaultComponent_component =
    ::mongo::logger::LogComponent::kDefault;
