#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "driver/base/instance.hpp"

int main(int argc, char** argv) {
    mongo::driver::base::instance instance;

    int result = Catch::Session().run(argc, argv);
}
