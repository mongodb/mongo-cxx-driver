#pragma once

#include <vector>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/options/search_index_view.hpp>
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

    impl(const impl& i) = default;

    impl(impl&& i) = default;

    ~impl() = default;

    impl& operator=(const impl& i) = default;

    cursor list(const std::string name, const bsoncxx::document::view& aggregation_opts) {
        pipeline pipeline{};
        pipeline.append_stage(
            make_document(kvp("$listSearchIndexes", make_document(kvp("name", name)))));
        return list(pipeline, aggregation_opts);
    }

    cursor list(const bsoncxx::document::view& aggregation_opts) {
        pipeline pipeline{};
        pipeline.append_stage(make_document(kvp("$listSearchIndexes", make_document())));
        return list(pipeline, aggregation_opts);
    }

    cursor list(const pipeline& pipeline, const bsoncxx::document::view& aggregation_opts) {
        libbson::scoped_bson_t opts_bson{aggregation_opts};
        libbson::scoped_bson_t stages(bsoncxx::document::view(pipeline.view_array()));
        bson_error_t error;
        const mongoc_read_prefs_t* rp_ptr = NULL;

        return libmongoc::collection_aggregate(
            _coll, static_cast<::mongoc_query_flags_t>(0), stages.bson(), opts_bson.bson(), rp_ptr);
    }

    bsoncxx::stdx::optional<std::string> create_one(const search_index_model& model,
                                                    const options::search_index_view& options) {
        bsoncxx::document::value result =
            create_many(std::vector<search_index_model>{model}, options);
        bsoncxx::document::view result_view = result.view();

        return bsoncxx::stdx::make_optional(result_view["indexesCreated"]
                                                .get_array()
                                                .value.begin()
                                                ->get_document()
                                                .value["name"]
                                                .get_string()
                                                .value.to_string());
    }

    bsoncxx::document::value create_many(const std::vector<search_index_model>& search_indexes,
                                         const options::search_index_view& options) {
        using namespace bsoncxx;

        builder::basic::array search_index_arr;

        for (auto&& model : search_indexes) {
            builder::basic::document search_index_doc;
            // model may or may not have a name attached to it (even though the server will)
            const stdx::optional<std::string> name = model.get_name();
            const bsoncxx::document::view& definition = model.get_definition();

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

        libbson::scoped_bson_t command_bson{command};
        libbson::scoped_bson_t opts_bson{opts_doc.view()};

        auto result = libmongoc::collection_write_command_with_opts(
            _coll, command_bson.bson(), opts_bson.bson(), reply.bson_for_init(), &error);

        if (!result) {
            throw_exception<operation_exception>(reply.steal(), error);
        }

        return reply.steal();
    }

    void drop_one(const std::string name, const options::search_index_view& options) {
        bsoncxx::builder::basic::document opts_doc;

        bsoncxx::document::value command = make_document(
            kvp("dropSearchIndex", libmongoc::collection_get_name(_coll)), kvp("name", name));

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

    void update_one(const std::string name,
                    const bsoncxx::document::view_or_value& definition,
                    const options::search_index_view& options) {
        bsoncxx::builder::basic::document opts_doc;
        bsoncxx::document::value command =
            make_document(kvp("updateSearchIndex", libmongoc::collection_get_name(_coll)),
                          kvp("name", name),
                          kvp("definition", definition.view()));

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
