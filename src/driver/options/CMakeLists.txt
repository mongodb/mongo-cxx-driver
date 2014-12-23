file (GLOB mongocxx-driver-options_headers *.hpp)
file (GLOB mongocxx-driver-options_sources *.cpp)

add_library(mongocxx-driver-options OBJECT ${mongocxx-driver-options_sources})
