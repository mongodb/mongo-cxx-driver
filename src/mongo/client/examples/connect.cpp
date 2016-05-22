// clientTest.cpp

/*    Copyright 2016 10gen Inc.
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

// It is the responsibility of the mongo client consumer to ensure that any necessary windows
// headers have already been included before including the driver facade headers.
#if defined(_WIN32)
#include <winsock2.h>
#include <windows.h>
#endif

#include <iostream>
#include <cstdlib>

// this header should be first to ensure that it includes cleanly in any context
#include "mongo/client/dbclient.h"
#include "mongo/stdx/functional.h"

namespace {

class OstreamAppender : public mongo::logger::MessageLogDomain::EventAppender {
public:
    explicit OstreamAppender(std::ostream& stream) : _stream(stream) {}

    virtual mongo::Status append(
        const mongo::logger::MessageLogDomain::EventAppender::Event& event) {
        _stream << event.getDate() << " " << event.getSeverity() << " " << event.getComponent()
                << " " << event.getContextName() << ": " << event.getMessage() << "\n";
        return mongo::Status::OK();
    }

private:
    std::ostream& _stream;
};

mongo::client::Options::LogAppenderPtr makeOstreamAppender(std::ostream& stream) {
    return mongo::client::Options::LogAppenderPtr(new OstreamAppender(stream));
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " MONGODB_URI" << std::endl;
        return EXIT_FAILURE;
    }

    const std::string uri(argv[1]);

    // The legacy C++ driver only has global state for SSL, and will error if
    // the URI has an SSL state that differs from the global state. Adjust the
    // global state to reflect the provided URI.
    mongo::client::Options options;
    if (uri.find("ssl=true") != std::string::npos) {
        options.setSSLMode(mongo::client::Options::kSSLRequired);
    }

    // Enable logging at an extremely verbose debug level.
    mongo::client::Options::LogAppenderFactory factory =
        mongo::stdx::bind(&makeOstreamAppender, boost::ref(std::cout));
    options.setLogAppenderFactory(factory);
    options.setMinLoggedSeverity(mongo::logger::LogSeverity::Debug(5));

    mongo::client::GlobalInstance instance(options);
    if (!instance.initialized()) {
        std::cout << "failed to initialize the client driver: " << instance.status() << std::endl;
        return EXIT_FAILURE;
    }

    std::string errmsg;
    mongo::ConnectionString connectionString = mongo::ConnectionString::parse(uri, errmsg);

    if (!connectionString.isValid()) {
        std::cout << "Error parsing connection string " << uri << ": " << errmsg << std::endl;
        return EXIT_FAILURE;
    }

    const boost::scoped_ptr<mongo::DBClientBase> connection(connectionString.connect(errmsg));
    if (!connection) {
        std::cout << "failed to connect: " << errmsg << "\n";
        return EXIT_FAILURE;
    }

    bool isMaster = false;
    mongo::BSONObj isMasterResult;

    const bool isMasterCmdResult = connection->isMaster(isMaster, &isMasterResult);

    if (!isMasterCmdResult) {
        std::cout << "Error running isMaster command" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "master: " << (isMaster ? "true" : "false") << "\n";
    std::cout << "details: " << mongo::tojson(isMasterResult, mongo::Strict, true) << "\n";

    return EXIT_SUCCESS;
}
