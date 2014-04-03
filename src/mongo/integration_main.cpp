#include <iostream>

#include "mongo/client/init.h"
#include "mongo/integration/integration.h"

ParameterStruct TestParams;

int main(int argc, char **argv) {
    if ( argc != 1 ) {
        if ( argc != 3 ) {
            std::cout << "need to pass port as second param" << std::endl;
            return EXIT_FAILURE;
        }
        TestParams.port = argv[ 2 ];
    }
    ::testing::InitGoogleTest(&argc, argv);
    mongo::Status status = mongo::client::initialize();
    return RUN_ALL_TESTS();
}
