// Copyright 2014 MongoDB Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#define CHECK_OPTIONAL_ARGUMENT(OBJECT, NAME, VALUE) \
    SECTION("has NAME disengaged") { \
        REQUIRE(!OBJECT.NAME()); \
    }\
\
    SECTION("has a method to set the upsert") { \
        OBJECT.NAME(VALUE); \
        REQUIRE(OBJECT.NAME().value() == VALUE); \
    }

#define CHECK_OPTIONAL_ARGUMENT_WITHOUT_EQUALITY(OBJECT, NAME, VALUE) \
    SECTION("has NAME disengaged") { \
        REQUIRE(!OBJECT.NAME()); \
    }\
\
    SECTION("has a method to set the upsert") { \
        OBJECT.NAME(VALUE); \
        REQUIRE(OBJECT.NAME()); \
    }

#define MOCK_CLIENT \
    auto client_new = libmongoc::client_new_from_uri.create_instance(); \
    client_new->interpose([](const mongoc_uri_t*){return nullptr;}).forever(); \
    auto client_destroy = libmongoc::client_destroy.create_instance(); \
    client_destroy->interpose([](mongoc_client_t*){}).forever(); \
    auto client_set_preference = libmongoc::client_set_read_prefs.create_instance(); \
    client_set_preference->interpose([](mongoc_client_t*, const mongoc_read_prefs_t*){}).forever(); \
    auto client_get_preference = libmongoc::client_get_read_prefs.create_instance(); \
    client_get_preference->interpose([](const mongoc_client_t*){return nullptr;}).forever(); \
    auto client_set_concern = libmongoc::client_set_write_concern.create_instance(); \
    client_set_concern->interpose([](mongoc_client_t*, const mongoc_write_concern_t*){}).forever(); \
    auto client_get_concern = libmongoc::client_get_write_concern.create_instance(); \
    client_get_concern->interpose([](const mongoc_client_t*){return nullptr;}).forever();

#define MOCK_DATABASE \
    auto get_database = libmongoc::client_get_database.create_instance(); \
    get_database->interpose([&](mongoc_client_t*, const char*) { \
        return nullptr; \
    }); \
    auto database_set_preference = libmongoc::database_set_read_prefs.create_instance(); \
    database_set_preference->interpose([](mongoc_database_t*, \
                                        const mongoc_read_prefs_t*){}).forever(); \
    auto database_get_preference = libmongoc::database_get_read_prefs.create_instance(); \
    database_get_preference->interpose([](const mongoc_database_t*){return nullptr;}).forever(); \
    auto database_set_concern = libmongoc::database_set_write_concern.create_instance(); \
    database_set_concern->interpose([](mongoc_database_t*, \
                                       const mongoc_write_concern_t*){}).forever(); \
    auto database_get_concern = libmongoc::database_get_write_concern.create_instance(); \
    database_get_concern->interpose([](const mongoc_database_t*){return nullptr;}).forever(); \
    auto database_destroy = libmongoc::database_destroy.create_instance(); \
    database_destroy->interpose([](mongoc_database_t*) {}).forever(); \
    auto database_get_collection = libmongoc::database_get_collection.create_instance();\
    database_get_collection->interpose([](mongoc_database_t*, const char*) { \
        return nullptr; \
    }).forever();

#define MOCK_COLLECTION \
    auto collection_set_preference = libmongoc::collection_set_read_prefs.create_instance(); \
    collection_set_preference->interpose([](mongoc_collection_t*, \
                                        const mongoc_read_prefs_t*) {}).forever(); \
    auto collection_get_preference = libmongoc::collection_get_read_prefs.create_instance(); \
    collection_get_preference->interpose([](const mongoc_collection_t*) {return nullptr;}).forever(); \
    auto collection_set_concern = libmongoc::collection_set_write_concern.create_instance(); \
    collection_set_concern->interpose([](mongoc_collection_t*,\
                                     const mongoc_write_concern_t*) {}).forever(); \
    auto collection_destroy = libmongoc::collection_destroy.create_instance();\
    collection_destroy->interpose([](mongoc_collection_t*) {});

#define MOCK_CONCERN \
    auto concern_copy = libmongoc::write_concern_copy.create_instance(); \
    concern_copy->interpose([](const mongoc_write_concern_t*){return nullptr;}).forever();
