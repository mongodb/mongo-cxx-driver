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
    SECTION("has NAME disengaged") {                 \
        REQUIRE(!OBJECT.NAME());                     \
    }                                                \
                                                     \
    SECTION("has a method to set the upsert") {      \
        OBJECT.NAME(VALUE);                          \
        REQUIRE(OBJECT.NAME().value() == VALUE);     \
    }

#define CHECK_OPTIONAL_ARGUMENT_WITHOUT_EQUALITY(OBJECT, NAME, VALUE) \
    SECTION("has NAME disengaged") {                                  \
        REQUIRE(!OBJECT.NAME());                                      \
    }                                                                 \
                                                                      \
    SECTION("has a method to set the upsert") {                       \
        OBJECT.NAME(VALUE);                                           \
        REQUIRE(OBJECT.NAME());                                       \
    }

#define MOCK_POOL                                                                              \
    auto client_pool_new = libmongoc::client_pool_new.create_instance();                       \
    client_pool_new->interpose([](const mongoc_uri_t*) { return nullptr; }).forever();         \
    auto client_pool_destroy = libmongoc::client_pool_destroy.create_instance();               \
    client_pool_destroy->interpose([&](::mongoc_client_pool_t*) {}).forever();                 \
    auto client_pool_pop = libmongoc::client_pool_pop.create_instance();                       \
    client_pool_pop->interpose([](::mongoc_client_pool_t*) { return nullptr; }).forever();     \
    auto client_pool_push = libmongoc::client_pool_push.create_instance();                     \
    client_pool_push->interpose([](::mongoc_client_pool_t*, ::mongoc_client_t*) {}).forever(); \
    auto client_pool_try_pop = libmongoc::client_pool_try_pop.create_instance();               \
    client_pool_try_pop->interpose([](::mongoc_client_pool_t*) { return nullptr; }).forever(); \
    auto client_pool_set_ssl_opts = libmongoc::client_pool_set_ssl_opts.create_instance();     \
    client_pool_set_ssl_opts->interpose([](::mongoc_client_pool_t*, const ::mongoc_ssl_opt_t*) {});

#define MOCK_CLIENT                                                                             \
    auto client_new = libmongoc::client_new_from_uri.create_instance();                         \
    client_new->interpose([](const mongoc_uri_t*) { return nullptr; }).forever();               \
    auto client_destroy = libmongoc::client_destroy.create_instance();                          \
    client_destroy->interpose([](mongoc_client_t*) {}).forever();                               \
    auto client_set_preference = libmongoc::client_set_read_prefs.create_instance();            \
    client_set_preference->interpose([](mongoc_client_t*, const mongoc_read_prefs_t*) {})       \
        .forever();                                                                             \
    auto client_get_preference = libmongoc::client_get_read_prefs.create_instance();            \
    client_get_preference->interpose([](const mongoc_client_t*) { return nullptr; }).forever(); \
    auto client_set_concern = libmongoc::client_set_write_concern.create_instance();            \
    client_set_concern->interpose([](mongoc_client_t*, const mongoc_write_concern_t*) {})       \
        .forever();                                                                             \
    auto client_get_concern = libmongoc::client_get_write_concern.create_instance();            \
    client_get_concern->interpose([](const mongoc_client_t*) { return nullptr; }).forever();

#define MOCK_DATABASE                                                                            \
    auto get_database = libmongoc::client_get_database.create_instance();                        \
    get_database->interpose([&](mongoc_client_t*, const char*) { return nullptr; });             \
    auto database_set_preference = libmongoc::database_set_read_prefs.create_instance();         \
    database_set_preference->interpose([](mongoc_database_t*, const mongoc_read_prefs_t*) {})    \
        .forever();                                                                              \
    auto database_get_preference = libmongoc::database_get_read_prefs.create_instance();         \
    database_get_preference->interpose([](const mongoc_database_t*) { return nullptr; })         \
        .forever();                                                                              \
    auto database_set_concern = libmongoc::database_set_write_concern.create_instance();         \
    database_set_concern->interpose([](mongoc_database_t*, const mongoc_write_concern_t*) {})    \
        .forever();                                                                              \
    auto database_get_concern = libmongoc::database_get_write_concern.create_instance();         \
    database_get_concern->interpose([](const mongoc_database_t*) { return nullptr; }).forever(); \
    auto database_destroy = libmongoc::database_destroy.create_instance();                       \
    database_destroy->interpose([](mongoc_database_t*) {}).forever();                            \
    auto database_get_collection = libmongoc::database_get_collection.create_instance();         \
    database_get_collection->interpose([](mongoc_database_t*, const char*) { return nullptr; })  \
        .forever();                                                                              \
    auto database_command = libmongoc::database_command.create_instance();                       \
    auto database_command_simple = libmongoc::database_command_simple.create_instance();

#define MOCK_COLLECTION                                                                           \
    auto collection_set_preference = libmongoc::collection_set_read_prefs.create_instance();      \
    collection_set_preference->interpose([](mongoc_collection_t*, const mongoc_read_prefs_t*) {}) \
        .forever();                                                                               \
    auto collection_get_preference = libmongoc::collection_get_read_prefs.create_instance();      \
    collection_get_preference->interpose([](const mongoc_collection_t*) { return nullptr; })      \
        .forever();                                                                               \
    auto collection_set_concern = libmongoc::collection_set_write_concern.create_instance();      \
    collection_set_concern->interpose([](mongoc_collection_t*, const mongoc_write_concern_t*) {}) \
        .forever();                                                                               \
    auto collection_destroy = libmongoc::collection_destroy.create_instance();                    \
    collection_destroy->interpose([](mongoc_collection_t*) {});                                   \
    auto collection_drop = libmongoc::collection_drop.create_instance();                          \
    auto collection_count = libmongoc::collection_count.create_instance();                        \
    auto collection_create_index = libmongoc::collection_create_index.create_instance();          \
    auto collection_find = libmongoc::collection_find.create_instance();                          \
    auto collection_aggregate = libmongoc::collection_aggregate.create_instance();

#define MOCK_CURSOR                                                    \
    auto cursor_destroy = libmongoc::cursor_destroy.create_instance(); \
    cursor_destroy->interpose([&](mongoc_cursor_t*) {});

#define MOCK_BULK                                                                                \
    auto bulk_operation_new = libmongoc::bulk_operation_new.create_instance();                   \
    auto bulk_operation_insert = libmongoc::bulk_operation_insert.create_instance();             \
    auto bulk_operation_remove_one = libmongoc::bulk_operation_remove_one.create_instance();     \
    auto bulk_operation_update_one = libmongoc::bulk_operation_update_one.create_instance();     \
    auto bulk_operation_replace_one = libmongoc::bulk_operation_replace_one.create_instance();   \
    auto bulk_operation_update = libmongoc::bulk_operation_update.create_instance();             \
    auto bulk_operation_remove = libmongoc::bulk_operation_remove.create_instance();             \
    auto bulk_operation_set_client = libmongoc::bulk_operation_set_client.create_instance();     \
    auto bulk_operation_set_database = libmongoc::bulk_operation_set_database.create_instance(); \
    auto bulk_operation_set_collection =                                                         \
        libmongoc::bulk_operation_set_collection.create_instance();                              \
    auto bulk_operation_set_write_concern =                                                      \
        libmongoc::bulk_operation_set_write_concern.create_instance();                           \
    auto bulk_operation_execute = libmongoc::bulk_operation_execute.create_instance();           \
    auto bulk_operation_destroy = libmongoc::bulk_operation_destroy.create_instance();           \
    bool bulk_operation_new_called = false;                                                      \
    bool bulk_operation_op_called = false;                                                       \
    bool bulk_operation_set_client_called = false;                                               \
    bool bulk_operation_set_database_called = false;                                             \
    bool bulk_operation_set_collection_called = false;                                           \
    bool bulk_operation_set_write_concern_called = false;                                        \
    bool bulk_operation_execute_called = false;                                                  \
    bool bulk_operation_destroy_called = false;

#define MOCK_CONCERN                                                     \
    auto concern_copy = libmongoc::write_concern_copy.create_instance(); \
    concern_copy->interpose([](const mongoc_write_concern_t*) { return nullptr; }).forever();
