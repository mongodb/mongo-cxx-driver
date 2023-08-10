#include <iostream>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

int main(void) {
    mongocxx::instance inst{};
    std::cout << "mongo-cxx-driver version: " << MONGOCXX_VERSION_STRING << std::endl;
    std::cout << "THE RED HAT UBI IMAGE WORKS!" << std::endl;
}
