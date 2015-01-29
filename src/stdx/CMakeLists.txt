file (GLOB mongocxx-stdx_headers *.hpp)
file (GLOB mongocxx-stdx_sources *.cpp)

add_library(mongocxx-stdx STATIC ${mongocxx-stdx_sources})
