#include <gtest/gtest.h>
#include <mongo/client/init.h>

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    const mongo::Status status = mongo::client::initialize();
    if (!status.isOK())
        ::abort();

    return RUN_ALL_TESTS();
}
