file (GLOB mongocxx-driver-model_headers *.hpp)
file (GLOB mongocxx-driver-model_sources *.cpp)

add_library(mongocxx-driver-model OBJECT ${mongocxx-driver-model_sources})
