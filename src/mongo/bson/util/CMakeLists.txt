file (GLOB bsoncxx-util_headers *.hpp)
file (GLOB bsoncxx-util_sources *.cpp)

add_library(bsoncxx-util OBJECT ${bsoncxx-util_sources})
