file (GLOB bsoncxx-builder_headers *.hpp)
file (GLOB bsoncxx-builder_sources *.cpp)

add_library(bsoncxx-builder OBJECT ${bsoncxx-builder_sources})
