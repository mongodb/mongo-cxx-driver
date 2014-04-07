#include <iostream>

#include "mongo/unittest/integration.h"
#include "mongo/client/init.h"

ParameterStruct TestParams;

int main(int argc, char **argv) {
    if ( argc != 1 ) {
        if ( argc != 3 ) {
            std::cout << "need to pass port as second param" << std::endl;
            return EXIT_FAILURE;
        }
        TestParams.port = argv[ 2 ];
    }

    mongo::Status status = mongo::client::initialize();
    if (!status.isOK())
        ::abort();

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
