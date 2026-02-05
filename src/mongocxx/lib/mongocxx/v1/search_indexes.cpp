// Copyright 2009-present MongoDB, Inc.
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

#include <mongocxx/v1/search_indexes.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <mongocxx/v1/pipeline.hpp>

#include <mongocxx/v1/aggregate_options.hh>
#include <mongocxx/v1/client_session.hh>
#include <mongocxx/v1/cursor.hh>
#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/read_preference.hh>

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class search_indexes::impl {
   public:
    mongoc_collection_t* _coll;

    ~impl() {
        libmongoc::collection_destroy(_coll);
    }

    impl(impl&& other) noexcept = delete;
    impl& operator=(impl&& other) noexcept = delete;

    impl(impl const& other) : _coll{libmongoc::collection_copy(other._coll)} {}

    impl& operator=(impl const& other) = delete;

    explicit impl(mongoc_collection_t* coll) : _coll{coll} {}

    static impl const& with(search_indexes const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(search_indexes const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(search_indexes& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(search_indexes* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

search_indexes::~search_indexes() {
    delete impl::with(_impl);
}

search_indexes::search_indexes(search_indexes&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

search_indexes& search_indexes::operator=(search_indexes&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

search_indexes::search_indexes(search_indexes const& other) : _impl{new impl{impl::with(other)}} {}

search_indexes& search_indexes::operator=(search_indexes const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

search_indexes::search_indexes(void* impl) : _impl{impl} {}

namespace {

mongoc_read_prefs_t const* get_read_prefs(v1::aggregate_options const& opts) {
    auto const& opt = v1::aggregate_options::internal::read_preference(opts);
    return opt ? v1::read_preference::internal::as_mongoc(*opt) : nullptr;
}

v1::cursor
list_impl(mongoc_collection_t* coll, char const* name, bson_t const* opts, mongoc_read_prefs_t const* read_prefs) {
    v1::pipeline pipeline;

    if (name) {
        pipeline.append_stage(
            scoped_bson{BCON_NEW("$listSearchIndexes", "{", "name", BCON_UTF8(std::string{name}.c_str()), "}")}.view());
    } else {
        pipeline.append_stage(scoped_bson{BCON_NEW("$listSearchIndexes", "{", "}")}.view());
    }

    return v1::cursor::internal::make(
        libmongoc::collection_aggregate(
            coll, MONGOC_QUERY_NONE, scoped_bson_view{pipeline.view_array()}.bson(), opts, read_prefs));
}

bsoncxx::v1::document::value create_impl(mongoc_collection_t* coll, bson_t const* command, bson_t const* opts) {
    scoped_bson reply;
    bson_error_t error = {};

    if (!libmongoc::collection_write_command_with_opts(coll, command, opts, reply.out_ptr(), &error)) {
        v1::throw_exception(error, std::move(reply).value());
    }

    return std::move(reply).value();
}

void append_to(bsoncxx::v1::document::view definition, char const* name, char const* type, scoped_bson& doc) {
    if (name) {
        doc += scoped_bson{BCON_NEW("name", BCON_UTF8(name))};
    }

    doc += scoped_bson{BCON_NEW("definition", BCON_DOCUMENT(scoped_bson_view{definition}.bson()))};

    if (type) {
        doc += scoped_bson{BCON_NEW("type", BCON_UTF8(type))};
    }
}

void append_to(search_indexes::model const& index, scoped_bson& doc) {
    auto const& name_opt = search_indexes::model::internal::name(index);
    auto const& type_opt = search_indexes::model::internal::type(index);

    return append_to(
        index.definition(), name_opt ? name_opt->c_str() : nullptr, type_opt ? type_opt->c_str() : nullptr, doc);
}

std::string create_one_impl(mongoc_collection_t* coll, bson_t const* index, bson_t const* opts) {
    auto const reply = create_impl(
        coll,
        scoped_bson{BCON_NEW(
                        "createSearchIndexes",
                        BCON_UTF8(libmongoc::collection_get_name(coll)),
                        "indexes",
                        "[",
                        BCON_DOCUMENT(index),
                        "]")}
            .bson(),
        opts);

    return std::string{reply["indexesCreated"].get_array().value[0].get_document().value["name"].get_string().value};
}

std::vector<std::string>
create_many_impl(mongoc_collection_t* coll, std::vector<search_indexes::model> const& indexes, bson_t const* opts) {
    struct deleter_type {
        void operator()(bson_array_builder_t* ptr) const noexcept {
            bson_array_builder_destroy(ptr);
        }
    };

    using builder_ptr_type = std::unique_ptr<bson_array_builder_t, deleter_type>;

    auto const builder_owner = builder_ptr_type{bson_array_builder_new(), deleter_type{}};
    auto const builder = builder_owner.get();

    for (auto const& index : indexes) {
        scoped_bson doc;
        append_to(index, doc);
        if (!bson_array_builder_append_document(builder, doc.bson())) {
            throw std::logic_error{"mongocxx::v1::create_many_impl: bson_array_builder_append_document failed"};
        }
    }

    scoped_bson arr;
    if (!bson_array_builder_build(builder, arr.out_ptr())) {
        throw std::logic_error{"mongocxx::v1::create_many_impl: bson_array_builder_build failed"};
    }

    auto const reply = create_impl(
        coll,
        scoped_bson{BCON_NEW(
                        "createSearchIndexes",
                        BCON_UTF8(libmongoc::collection_get_name(coll)),
                        "indexes",
                        BCON_ARRAY(arr.bson()))}
            .bson(),
        opts);

    std::vector<std::string> ret;

    // {"indexesCreated": [{ "name": <name> }, ...]} -> [<name>, ...]
    for (auto const index : reply["indexesCreated"].get_array().value) {
        ret.push_back(std::string{index.get_document().value["name"].get_string().value});
    }

    return ret;
}

void drop_one_impl(mongoc_collection_t* coll, char const* name, bson_t const* opts) {
    scoped_bson const command{
        BCON_NEW("dropSearchIndex", BCON_UTF8(libmongoc::collection_get_name(coll)), "name", BCON_UTF8(name))};

    scoped_bson reply;
    bson_error_t error = {};

    if (!libmongoc::collection_write_command_with_opts(coll, command, opts, reply.out_ptr(), &error)) {
        // https://www.mongodb.com/docs/manual/reference/error-codes/
        static constexpr std::uint32_t serverErrorNamespaceNotFound = 26;

        // Ignore NamespaceNotFound.
        if (error.domain != MONGOC_ERROR_QUERY || error.code != serverErrorNamespaceNotFound) {
            v1::throw_exception(error, std::move(reply).value());
        }
    }
}

void update_one_impl(
    mongoc_collection_t* coll,
    char const* name,
    bsoncxx::v1::document::view definition,
    bson_t const* opts) {
    scoped_bson reply;
    bson_error_t error = {};

    if (!libmongoc::collection_write_command_with_opts(
            coll,
            scoped_bson{BCON_NEW(
                            "updateSearchIndex",
                            BCON_UTF8(libmongoc::collection_get_name(coll)),
                            "name",
                            BCON_UTF8(name),
                            "definition",
                            BCON_DOCUMENT(scoped_bson_view{definition}.bson()))}
                .bson(),
            opts,
            reply.out_ptr(),
            &error)) {
        v1::throw_exception(error, std::move(reply).value());
    }
}

} // namespace

v1::cursor search_indexes::list(v1::aggregate_options const& opts) {
    scoped_bson opts_doc;

    v1::aggregate_options::internal::append_to(opts, opts_doc);

    return list_impl(impl::with(this)->_coll, nullptr, opts_doc.bson(), get_read_prefs(opts));
}

v1::cursor search_indexes::list(v1::client_session const& session, v1::aggregate_options const& opts) {
    scoped_bson opts_doc;

    v1::aggregate_options::internal::append_to(opts, opts_doc);
    v1::client_session::internal::append_to(session, opts_doc);

    return list_impl(impl::with(this)->_coll, nullptr, opts_doc.bson(), get_read_prefs(opts));
}

v1::cursor search_indexes::list(bsoncxx::v1::stdx::string_view name, v1::aggregate_options const& opts) {
    scoped_bson opts_doc;

    v1::aggregate_options::internal::append_to(opts, opts_doc);

    return list_impl(impl::with(this)->_coll, std::string{name}.c_str(), opts_doc.bson(), get_read_prefs(opts));
}

v1::cursor search_indexes::list(
    v1::client_session const& session,
    bsoncxx::v1::stdx::string_view name,
    v1::aggregate_options const& opts) {
    scoped_bson opts_doc;

    v1::aggregate_options::internal::append_to(opts, opts_doc);
    v1::client_session::internal::append_to(session, opts_doc);

    return list_impl(impl::with(this)->_coll, std::string{name}.c_str(), opts_doc.bson(), get_read_prefs(opts));
}

std::string search_indexes::create_one(bsoncxx::v1::document::view definition) {
    scoped_bson doc;

    append_to(definition, nullptr, nullptr, doc);

    return create_one_impl(v1::search_indexes::internal::get_collection(*this), doc.bson(), nullptr);
}

std::string search_indexes::create_one(v1::client_session const& session, bsoncxx::v1::document::view definition) {
    scoped_bson doc;
    append_to(definition, nullptr, nullptr, doc);

    scoped_bson opts_doc;
    v1::client_session::internal::append_to(session, opts_doc);

    return create_one_impl(v1::search_indexes::internal::get_collection(*this), doc.bson(), opts_doc.bson());
}

std::string search_indexes::create_one(bsoncxx::v1::stdx::string_view name, bsoncxx::v1::document::view definition) {
    scoped_bson doc;

    append_to(definition, std::string{name}.c_str(), nullptr, doc);

    return create_one_impl(v1::search_indexes::internal::get_collection(*this), doc.bson(), nullptr);
}

std::string search_indexes::create_one(
    v1::client_session const& session,
    bsoncxx::v1::stdx::string_view name,
    bsoncxx::v1::document::view definition) {
    scoped_bson doc;
    append_to(definition, std::string{name}.c_str(), nullptr, doc);

    scoped_bson opt_doc;
    v1::client_session::internal::append_to(session, opt_doc);

    return create_one_impl(v1::search_indexes::internal::get_collection(*this), doc.bson(), opt_doc.bson());
}

std::string search_indexes::create_one(model const& index) {
    scoped_bson doc;

    append_to(index, doc);

    return create_one_impl(v1::search_indexes::internal::get_collection(*this), doc.bson(), nullptr);
}

std::string search_indexes::create_one(v1::client_session const& session, model const& index) {
    scoped_bson doc;
    append_to(index, doc);

    scoped_bson opts_doc;
    v1::client_session::internal::append_to(session, opts_doc);

    return create_one_impl(v1::search_indexes::internal::get_collection(*this), doc.bson(), opts_doc.bson());
}

std::vector<std::string> search_indexes::create_many(std::vector<model> const& indexes) {
    return create_many_impl(impl::with(this)->_coll, indexes, nullptr);
}

std::vector<std::string> search_indexes::create_many(
    v1::client_session const& session,
    std::vector<model> const& indexes) {
    scoped_bson opts_doc;

    v1::client_session::internal::append_to(session, opts_doc);

    return create_many_impl(impl::with(this)->_coll, indexes, opts_doc.bson());
}

void search_indexes::drop_one(bsoncxx::v1::stdx::string_view name) {
    return drop_one_impl(impl::with(this)->_coll, std::string{name}.c_str(), nullptr);
}

void search_indexes::drop_one(v1::client_session const& session, bsoncxx::v1::stdx::string_view name) {
    scoped_bson opts_doc;

    v1::client_session::internal::append_to(session, opts_doc);

    return drop_one_impl(impl::with(this)->_coll, std::string{name}.c_str(), opts_doc.bson());
}

void search_indexes::update_one(bsoncxx::v1::stdx::string_view name, bsoncxx::v1::document::view definition) {
    return update_one_impl(impl::with(this)->_coll, std::string{name}.c_str(), definition, nullptr);
}

void search_indexes::update_one(
    v1::client_session const& session,
    bsoncxx::v1::stdx::string_view name,
    bsoncxx::v1::document::view definition) {
    scoped_bson opts_doc;

    v1::client_session::internal::append_to(session, opts_doc);

    return update_one_impl(impl::with(this)->_coll, std::string{name}.c_str(), definition, opts_doc.bson());
}

class search_indexes::model::impl {
   public:
    bsoncxx::v1::document::value _definition;
    bsoncxx::v1::stdx::optional<std::string> _name;
    bsoncxx::v1::stdx::optional<std::string> _type;

    impl(std::string name, bsoncxx::v1::document::value definition)
        : _definition{std::move(definition)}, _name{std::move(name)} {}

    explicit impl(bsoncxx::v1::document::value definition) : _definition{std::move(definition)} {}

    static impl const& with(model const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(model const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(model& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(model* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

search_indexes::model::~model() {
    delete impl::with(_impl);
}

search_indexes::model::model(model&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

search_indexes::model& search_indexes::model::operator=(model&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

search_indexes::model::model(model const& other) : _impl{new impl{impl::with(other)}} {}

search_indexes::model& search_indexes::model::operator=(model const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

search_indexes::model::model(std::string name, bsoncxx::v1::document::value definition)
    : _impl{new impl{std::move(name), std::move(definition)}} {}

search_indexes::model::model(bsoncxx::v1::document::value definition) : _impl{new impl{std::move(definition)}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> search_indexes::model::name() const {
    return impl::with(this)->_name;
}

bsoncxx::v1::document::view search_indexes::model::definition() const {
    return impl::with(this)->_definition;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> search_indexes::model::type() const {
    return impl::with(this)->_type;
}

search_indexes::model& search_indexes::model::type(std::string type) {
    impl::with(this)->_type = std::move(type);
    return *this;
}

search_indexes search_indexes::internal::make(mongoc_collection_t* coll) {
    return {new impl{coll}};
}

bool search_indexes::internal::is_moved_from(search_indexes const& self) {
    return self._impl == nullptr;
}

mongoc_collection_t const* search_indexes::internal::get_collection(search_indexes const& self) {
    return impl::with(self)._coll;
}

mongoc_collection_t* search_indexes::internal::get_collection(search_indexes& self) {
    return impl::with(self)._coll;
}

bsoncxx::v1::stdx::optional<std::string> const& search_indexes::model::internal::name(model const& self) {
    return impl::with(self)._name;
}

bsoncxx::v1::document::value const& search_indexes::model::internal::definition(model const& self) {
    return impl::with(self)._definition;
}

bsoncxx::v1::stdx::optional<std::string> const& search_indexes::model::internal::type(model const& self) {
    return impl::with(self)._type;
}

bsoncxx::v1::stdx::optional<std::string>& search_indexes::model::internal::name(model& self) {
    return impl::with(self)._name;
}

bsoncxx::v1::document::value& search_indexes::model::internal::definition(model& self) {
    return impl::with(self)._definition;
}

bsoncxx::v1::stdx::optional<std::string>& search_indexes::model::internal::type(model& self) {
    return impl::with(self)._type;
}

} // namespace v1
} // namespace mongocxx
