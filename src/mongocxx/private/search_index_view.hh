#pragma once

#include <vector>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/string/view_or_value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/private/append_aggregate_options.hh>
#include <mongocxx/private/client_session.hh>
#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

class search_index_view::impl {
   public:
    impl(mongoc_collection_t* collection, mongoc_client_t* client)
        : _coll{collection}, _client{client} {}

    ~impl() = default;
    impl(impl&& i) = delete;
    impl& operator=(impl&& i) = delete;
    impl(const impl& i) = delete;
    impl& operator=(const impl& i) = delete;

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

        return libmongoc::collection_aggregate(
            _coll, mongoc_query_flags_t(), stages.bson(), opts_bson.bson(), rp_ptr);
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

        auto result = libmongoc::collection_write_command_with_opts(
            _coll, command_bson.bson(), opts_bson.bson(), reply.bson_for_init(), &error);

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

        bool result = libmongoc::collection_write_command_with_opts(
            _coll, command_bson.bson(), opts_bson.bson(), reply.bson_for_init(), &error);

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

        bool result = libmongoc::collection_write_command_with_opts(
            _coll, command_bson.bson(), opts_bson.bson(), reply.bson_for_init(), &error);

        if (!result) {
            throw_exception<operation_exception>(reply.steal(), error);
        }
    }

    mongoc_collection_t* _coll;
    mongoc_client_t* _client;
};
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
