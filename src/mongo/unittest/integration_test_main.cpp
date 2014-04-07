#include <iostream>

#include "mongo/unittest/integration_test.h"
#include "mongo/client/init.h"

using mongo::unittest::IntegrationTestParameters;

IntegrationTestParameters params;

int main(int argc, char **argv) {
    if ( argc != 1 ) {
        if ( argc != 3 ) {
            std::cout << "need to pass port as second param" << std::endl;
            return EXIT_FAILURE;
        }
        params.port = argv[2];
    } else {
        params.port = "27107";
    }

    mongo::Status status = mongo::client::initialize();
    if (!status.isOK())
        ::abort();

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
