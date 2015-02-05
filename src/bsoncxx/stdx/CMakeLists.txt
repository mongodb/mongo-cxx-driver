file (GLOB bsoncxx-stdx_headers *.hpp)
file (GLOB bsoncxx-stdx_sources *.cpp)

add_library(bsoncxx-stdx OBJECT ${bsoncxx-stdx_sources})
