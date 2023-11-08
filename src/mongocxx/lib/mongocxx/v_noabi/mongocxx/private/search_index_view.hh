#pragma once

#include <vector>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <mongocxx/private/append_aggregate_options.hh>
#include <mongocxx/private/client_session.hh>
#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
inline namespace v_noabi {
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

struct collection_deleter {
    void operator()(mongoc_collection_t* ptr) noexcept {
        libmongoc::collection_destroy(ptr);
    }
};

using collection_ptr = std::unique_ptr<mongoc_collection_t, collection_deleter>;

// `copy_and_apply_default_rw_concerns` copies the mongoc_collection_t and applies a default
// readConcern and writeConcern. Used to prevent sending a readConcern or writeConcern.
static collection_ptr copy_and_apply_default_rw_concerns(mongoc_collection_t* coll) {
    auto* wc_default = libmongoc::write_concern_new();
    auto* rc_default = libmongoc::read_concern_new();
    auto coll_copy = libmongoc::collection_copy(coll);

    mongoc_collection_set_read_concern(coll_copy, rc_default);
    mongoc_collection_set_write_concern(coll_copy, wc_default);

    libmongoc::read_concern_destroy(rc_default);
    libmongoc::write_concern_destroy(wc_default);

    return collection_ptr(coll_copy);
}

class search_index_view::impl {
   public:
    impl(mongoc_collection_t* collection, mongoc_client_t* client)
        : _coll{collection}, _client{client} {}

    cursor list(const client_session* session,
                bsoncxx::string::view_or_value name,
                const options::aggregate& options) {
        pipeline pipeline{};
        pipeline.append_stage(
            make_document(kvp("$listSearchIndexes", make_document(kvp("name", name.view())))));
        return list(session, pipeline, options);
    }

    cursor list(const client_session* session, const options::aggregate& options) {
        pipeline pipeline{};
        pipeline.append_stage(make_document(kvp("$listSearchIndexes", make_document())));
        return list(session, pipeline, options);
    }

    cursor list(const client_session* session,
                const pipeline& pipeline,
                const options::aggregate& options) {
        bsoncxx::builder::basic::document opts_doc;
        libbson::scoped_bson_t stages(bsoncxx::document::view(pipeline.view_array()));

        append_aggregate_options(opts_doc, options);

        if (session) {
            opts_doc.append(bsoncxx::builder::concatenate_doc{session->_get_impl().to_document()});
        }

        const mongoc_read_prefs_t* const rp_ptr =
            options.read_preference() ? options.read_preference()->_impl->read_preference_t
                                      : nullptr;

        libbson::scoped_bson_t opts_bson(opts_doc.view());

        auto coll_copy = copy_and_apply_default_rw_concerns(_coll);
        return libmongoc::collection_aggregate(
            coll_copy.get(), mongoc_query_flags_t(), stages.bson(), opts_bson.bson(), rp_ptr);
    }

    std::string create_one(const client_session* session, const search_index_model& model) {
        const auto result = create_many(session, std::vector<search_index_model>{model});
        return bsoncxx::string::to_string(result["indexesCreated"]
                                              .get_array()
                                              .value.begin()
                                              ->get_document()
                                              .value["name"]
                                              .get_string()
                                              .value);
    }

    bsoncxx::document::value create_many(const client_session* session,
                                         const std::vector<search_index_model>& search_indexes) {
        using namespace bsoncxx;

        builder::basic::array search_index_arr;

        for (auto&& model : search_indexes) {
            builder::basic::document search_index_doc;
            // model may or may not have a name attached to it. The server will create the name if
            // it is not set.
            const stdx::optional<bsoncxx::string::view_or_value> name = model.name();
            const bsoncxx::document::view& definition = model.definition();

            if (name) {
                search_index_doc.append(kvp("name", name.value()));
            }
            search_index_doc.append(kvp("definition", definition));
            search_index_arr.append(search_index_doc.view());
        }

        document::view_or_value command =
            make_document(kvp("createSearchIndexes", libmongoc::collection_get_name(_coll)),
                          kvp("indexes", search_index_arr.view()));

        libbson::scoped_bson_t reply;
        bson_error_t error;

        builder::basic::document opts_doc;

        if (session) {
            opts_doc.append(bsoncxx::builder::concatenate_doc{session->_get_impl().to_document()});
        }

        libbson::scoped_bson_t command_bson{command};
        libbson::scoped_bson_t opts_bson{opts_doc.view()};

        auto coll_copy = copy_and_apply_default_rw_concerns(_coll);
        auto result = libmongoc::collection_write_command_with_opts(
            coll_copy.get(), command_bson.bson(), opts_bson.bson(), reply.bson_for_init(), &error);

        if (!result) {
            throw_exception<operation_exception>(reply.steal(), error);
        }

        return reply.steal();
    }

    void drop_one(const client_session* session, bsoncxx::string::view_or_value name) {
        bsoncxx::builder::basic::document opts_doc;

        bsoncxx::document::value command =
            make_document(kvp("dropSearchIndex", libmongoc::collection_get_name(_coll)),
                          kvp("name", name.view()));

        if (session) {
            opts_doc.append(bsoncxx::builder::concatenate_doc{session->_get_impl().to_document()});
        }

        libbson::scoped_bson_t reply;
        libbson::scoped_bson_t command_bson{command.view()};
        libbson::scoped_bson_t opts_bson{opts_doc.view()};
        bson_error_t error;

        auto coll_copy = copy_and_apply_default_rw_concerns(_coll);
        bool result = libmongoc::collection_write_command_with_opts(
            coll_copy.get(), command_bson.bson(), opts_bson.bson(), reply.bson_for_init(), &error);

        const uint32_t serverErrorNamespaceNotFound = 26;
        if (error.domain == MONGOC_ERROR_QUERY && error.code == serverErrorNamespaceNotFound) {
            // Ignore NamespaceNotFound error.
            // NamespaceNotFound server error code is documented in server code:
            // https://github.com/mongodb/mongo/blob/07e852967e936adbc255518ebaa9c116937becc4/src/mongo/base/error_codes.yml#L64
            return;
        }

        if (!result) {
            throw_exception<operation_exception>(reply.steal(), error);
        }
    }

    void update_one(const client_session* session,
                    bsoncxx::string::view_or_value name,
                    bsoncxx::document::view_or_value definition) {
        bsoncxx::builder::basic::document opts_doc;
        bsoncxx::document::value command =
            make_document(kvp("updateSearchIndex", libmongoc::collection_get_name(_coll)),
                          kvp("name", name.view()),
                          kvp("definition", definition.view()));

        if (session) {
            opts_doc.append(bsoncxx::builder::concatenate_doc{session->_get_impl().to_document()});
        }

        libbson::scoped_bson_t reply;
        libbson::scoped_bson_t command_bson{command.view()};
        libbson::scoped_bson_t opts_bson{opts_doc.view()};
        bson_error_t error;

        auto coll_copy = copy_and_apply_default_rw_concerns(_coll);
        bool result = libmongoc::collection_write_command_with_opts(
            coll_copy.get(), command_bson.bson(), opts_bson.bson(), reply.bson_for_init(), &error);

        if (!result) {
            throw_exception<operation_exception>(reply.steal(), error);
        }
    }

    mongoc_collection_t* _coll;
    mongoc_client_t* _client;
};
}  // namespace v_noabi
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
