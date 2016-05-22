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
