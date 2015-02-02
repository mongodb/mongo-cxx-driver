file (GLOB_RECURSE mongocxx-bson_headers *.hpp)
file (GLOB_RECURSE mongocxx-bson_sources *.cpp)

add_library(mongocxx-bson ${mongocxx-bson_sources})
