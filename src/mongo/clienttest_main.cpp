#include <gtest/gtest.h>
#include <mongo/client/init.h>

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    mongo::Status status = mongo::client::initialize();
    return RUN_ALL_TESTS();
}
