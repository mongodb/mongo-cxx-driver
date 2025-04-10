# As a Regular Exception

@warning The @ref mongocxx::server_error_category error category is overloaded ([CXX-834](https://jira.mongodb.org/browse/CXX-834)). The error code value may belong to the server, mongoc, or libmongocrypt depending on the context. Use error code values with caution.

@snippet examples/api/mongocxx/examples/operation_exceptions/regular.cpp Example

# As an Operation Exception

@note Using @ref mongocxx::exception for error handling is recommended for forward compatibility. ([CXX-2377](https://jira.mongodb.org/browse/CXX-2377))

@snippet examples/api/mongocxx/examples/operation_exceptions/operation.cpp Example
