workspace(name = "bazelregistry_mongo_cxx_driver")

local_repository(
    name = "bazelregistry_mongo_c_driver",
    path = "../mongo-c-driver",
)

load("@bazelregistry_mongo_c_driver//:config.bzl", "mongo_c_driver_common_config", "bson_config", "mongoc_config")

mongo_c_driver_common_config()
bson_config()
mongoc_config()

load("//:config.bzl", "bsoncxx_config", "mongocxx_config")
bsoncxx_config()
mongocxx_config()
