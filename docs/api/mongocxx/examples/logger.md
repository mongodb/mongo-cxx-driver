# Basic Usage

@snippet api/mongocxx/examples/logger/basic_usage.cpp Example

# Temporarily Replace the Logger

Use @ref mongocxx::v1::logger_guard to install a log message handler for a scope
and automatically restore the previously-active handler on exit. Guards nest,
restoring each prior handler in turn.

@snippet api/mongocxx/examples/logger/logger_guard.cpp Example

# Convert a Log Level to a String

@snippet api/mongocxx/examples/logger/to_string.cpp Example
