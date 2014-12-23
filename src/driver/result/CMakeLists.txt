file (GLOB mongocxx-driver-result_headers *.hpp)
file (GLOB mongocxx-driver-result_sources *.cpp)

add_library(mongocxx-driver-result OBJECT ${mongocxx-driver-result_sources})

