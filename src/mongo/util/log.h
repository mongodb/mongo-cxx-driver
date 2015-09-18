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

// #pragma once is not used in this header.
// This header attempts to enforce the rule that no logging should be done in
// an inline function defined in a header.
// To enforce this "no logging in header" rule, we use #include guards with a validating #else
// clause.
// Also, this header relies on a preprocessor macro to determine the default component for the
// unconditional logging functions severe(), error(), warning() and log(). Disallowing multiple
// inclusion of log.h will ensure that the default component will be set correctly.

#if defined(MONGO_UTIL_LOG_H_)
#error \
    "mongo/util/log.h cannot be included multiple times. " \
       "This may occur when log.h is included in a header. " \
       "Please check your #include's."
#else  // MONGO_UTIL_LOG_H_
#define MONGO_UTIL_LOG_H_

#include "mongo/base/status.h"
#include "mongo/bson/util/builder.h"
#include "mongo/logger/log_component.h"
#include "mongo/logger/logger.h"
#include "mongo/logger/logstream_builder.h"
#include "mongo/logger/tee.h"

// Provide log component in global scope so that MONGO_LOG will always have a valid component.
// Global log component will be kDefault unless overridden by MONGO_LOG_DEFAULT_COMPONENT.
#if defined(MONGO_LOG_DEFAULT_COMPONENT)
const ::mongo::logger::LogComponent MongoLogDefaultComponent_component =
    MONGO_LOG_DEFAULT_COMPONENT;
#else
#error \
    "mongo/util/log.h requires MONGO_LOG_DEFAULT_COMPONENT to be defined. " \
       "Please see http://www.mongodb.org/about/contributors/reference/server-logging-rules/ "
#endif  // MONGO_LOG_DEFAULT_COMPONENT

namespace mongo {

namespace logger {
typedef void (*ExtraLogContextFn)(BufBuilder& builder);
Status registerExtraLogContextFn(ExtraLogContextFn contextFn);

}  // namespace logger

namespace {

using logger::LogstreamBuilder;
using logger::LabeledLevel;
using logger::Tee;

/**
 * Returns a LogstreamBuilder for logging a message with LogSeverity::Severe().
 */
inline LogstreamBuilder severe() {
    return LogstreamBuilder(logger::globalLogDomain(),
                            std::string(),
                            logger::LogSeverity::Severe(),
                            ::MongoLogDefaultComponent_component);
}

inline LogstreamBuilder severe(logger::LogComponent component) {
    return LogstreamBuilder(
        logger::globalLogDomain(), std::string(), logger::LogSeverity::Severe(), component);
}

/**
 * Returns a LogstreamBuilder for logging a message with LogSeverity::Error().
 */
inline LogstreamBuilder error() {
    return LogstreamBuilder(logger::globalLogDomain(),
                            std::string(),
                            logger::LogSeverity::Error(),
                            ::MongoLogDefaultComponent_component);
}

inline LogstreamBuilder error(logger::LogComponent component) {
    return LogstreamBuilder(
        logger::globalLogDomain(), std::string(), logger::LogSeverity::Error(), component);
}

/**
 * Returns a LogstreamBuilder for logging a message with LogSeverity::Warning().
 */
inline LogstreamBuilder warning() {
    return LogstreamBuilder(logger::globalLogDomain(),
                            std::string(),
                            logger::LogSeverity::Warning(),
                            ::MongoLogDefaultComponent_component);
}

inline LogstreamBuilder warning(logger::LogComponent component) {
    return LogstreamBuilder(
        logger::globalLogDomain(), std::string(), logger::LogSeverity::Warning(), component);
}

/**
 * Returns a LogstreamBuilder for logging a message with LogSeverity::Log().
 */
inline LogstreamBuilder log() {
    return LogstreamBuilder(logger::globalLogDomain(),
                            std::string(),
                            logger::LogSeverity::Log(),
                            ::MongoLogDefaultComponent_component);
}

inline LogstreamBuilder log(logger::LogComponent component) {
    return LogstreamBuilder(
        logger::globalLogDomain(), std::string(), logger::LogSeverity::Log(), component);
}

inline LogstreamBuilder log(logger::LogComponent::Value componentValue) {
    return LogstreamBuilder(
        logger::globalLogDomain(), std::string(), logger::LogSeverity::Log(), componentValue);
}

/**
 * Runs the same logic as log()/warning()/error(), without actually outputting a stream.
 */
inline bool shouldLog(logger::LogSeverity severity) {
    return logger::globalLogDomain()->shouldLog(::MongoLogDefaultComponent_component, severity);
}

}  // namespace

// MONGO_LOG uses log component from MongoLogDefaultComponent from current or global namespace.
#define MONGO_LOG(DLEVEL)                                                              \
    if (!(::mongo::logger::globalLogDomain())                                          \
             ->shouldLog(MongoLogDefaultComponent_component,                           \
                         ::mongo::LogstreamBuilder::severityCast(DLEVEL))) {           \
    } else                                                                             \
    ::mongo::logger::LogstreamBuilder(::mongo::logger::globalLogDomain(),              \
                                      std::string(),                                   \
                                      ::mongo::LogstreamBuilder::severityCast(DLEVEL), \
                                      MongoLogDefaultComponent_component)

#define LOG MONGO_LOG

#define MONGO_LOG_COMPONENT(DLEVEL, COMPONENT1)                                            \
    if (!(::mongo::logger::globalLogDomain())                                              \
             ->shouldLog((COMPONENT1), ::mongo::LogstreamBuilder::severityCast(DLEVEL))) { \
    } else                                                                                 \
    ::mongo::logger::LogstreamBuilder(::mongo::logger::globalLogDomain(),                  \
                                      std::string(),                                       \
                                      ::mongo::LogstreamBuilder::severityCast(DLEVEL),     \
                                      (COMPONENT1))

#define MONGO_LOG_COMPONENT2(DLEVEL, COMPONENT1, COMPONENT2)                                     \
    if (!(::mongo::logger::globalLogDomain())                                                    \
             ->shouldLog(                                                                        \
                 (COMPONENT1), (COMPONENT2), ::mongo::LogstreamBuilder::severityCast(DLEVEL))) { \
    } else                                                                                       \
    ::mongo::logger::LogstreamBuilder(::mongo::logger::globalLogDomain(),                        \
                                      std::string(),                                             \
                                      ::mongo::LogstreamBuilder::severityCast(DLEVEL),           \
                                      (COMPONENT1))

#define MONGO_LOG_COMPONENT3(DLEVEL, COMPONENT1, COMPONENT2, COMPONENT3)               \
    if (!(::mongo::logger::globalLogDomain())                                          \
             ->shouldLog((COMPONENT1),                                                 \
                         (COMPONENT2),                                                 \
                         (COMPONENT3),                                                 \
                         ::mongo::LogstreamBuilder::severityCast(DLEVEL))) {           \
    } else                                                                             \
    ::mongo::logger::LogstreamBuilder(::mongo::logger::globalLogDomain(),              \
                                      std::string(),                                   \
                                      ::mongo::LogstreamBuilder::severityCast(DLEVEL), \
                                      (COMPONENT1))


/** output the error # and error message with prefix.
    handy for use as parm in uassert/massert.
    */
std::string errnoWithPrefix(const char* prefix);

std::string errnoWithDescription(int errorcode = -1);

/**
 * Write the optional "msg".
 */
void logContext(const char* msg = NULL);

}  // namespace mongo

#endif  // MONGO_UTIL_LOG_H_
