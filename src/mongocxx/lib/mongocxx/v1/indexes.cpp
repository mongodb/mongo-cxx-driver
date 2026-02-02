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

#include <mongocxx/v1/indexes.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <mongocxx/v1/write_concern.hpp>

#include <mongocxx/v1/client_session.hh>
#include <mongocxx/v1/cursor.hh>
#include <mongocxx/v1/exception.hh>

#include <chrono>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/immortal.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

using code = mongocxx::v1::indexes::errc;

class indexes::impl {
   public:
    mongoc_collection_t* _coll;
    mongoc_client_t* _client;

    impl(mongoc_collection_t* coll, mongoc_client_t* client) : _coll{coll}, _client{client} {}

    static impl const& with(indexes const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(indexes const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(indexes& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(indexes* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

indexes::~indexes() {
    delete impl::with(_impl);
}

indexes::indexes(indexes&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

indexes& indexes::operator=(indexes&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

indexes::indexes(indexes const& other) : _impl{new impl{impl::with(other)}} {}

indexes& indexes::operator=(indexes const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

namespace {

std::string keys_to_index_string(bsoncxx::v1::document::view keys) {
    struct deleter_type {
        void operator()(char* ptr) const noexcept {
            bson_free(ptr);
        }
    };

    using ptr_type = std::unique_ptr<char, deleter_type>;

    return ptr_type{libmongoc::collection_keys_to_index_string(scoped_bson_view{keys})}.get();
}

void append_to(indexes::model const& index, scoped_bson& doc) {
    auto const& model_opts = index.options();
    auto const& keys = index.keys();

    if (!model_opts["name"]) {
        doc += scoped_bson{BCON_NEW("name", BCON_UTF8(keys_to_index_string(keys).c_str()))};
    }

    doc += scoped_bson{BCON_NEW("key", BCON_DOCUMENT(scoped_bson_view{keys}.bson()))};
    doc += model_opts;
}

void append_to(indexes::options const& opts, scoped_bson& doc) {
    if (auto const& opt = opts.max_time()) {
        doc += scoped_bson{BCON_NEW("maxTimeMS", BCON_INT64(std::int64_t{opt->count()}))};
    }

    if (auto const& opt = indexes::options::internal::write_concern(opts)) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{opt->to_document()}.bson()))};
    }
}

v1::cursor list_impl(mongoc_collection_t* coll, bson_t const* opts) {
    return v1::cursor::internal::make(libmongoc::collection_find_indexes_with_opts(coll, opts));
}

bsoncxx::v1::document::value create_impl(
    mongoc_collection_t* coll,
    bson_t const* indexes,
    bson_t const* opts,
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> commit_quorum) {
    scoped_bson command{
        BCON_NEW("createIndexes", BCON_UTF8(libmongoc::collection_get_name(coll)), "indexes", BCON_ARRAY(indexes))};

    if (commit_quorum) {
        command += *commit_quorum;
    }

    scoped_bson reply;
    bson_error_t error = {};

    if (!libmongoc::collection_write_command_with_opts(coll, command.bson(), opts, reply.out_ptr(), &error)) {
        v1::throw_exception(error, std::move(reply).value());
    }

    return std::move(reply).value();
}

bsoncxx::v1::stdx::optional<std::string> create_one_impl(
    mongoc_collection_t* coll,
    indexes::model const& index,
    bson_t const* opts,
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> commit_quorum) {
    auto const reply_owner = [&] {
        scoped_bson doc;
        append_to(index, doc);
        return create_impl(coll, scoped_bson{BCON_NEW("0", BCON_DOCUMENT(doc.bson()))}.bson(), opts, commit_quorum);
    }();

    auto const reply = [&] {
        // SERVER-78611: sharded clusters may place fields in a raw response document instead of in
        // the top-level document.
        if (auto const raw = reply_owner.view()["raw"]) {
            // There should only be a single field in the raw response with the shard connection
            // string as the key. e.g.:
            //   {
            //     'raw': {
            //       'shard01/localhost:27018,27019,27020': {
            //         ... # Raw response fields.
            //       }
            //     }
            //   }
            // Using a for loop for convenience.
            for (auto const& shard_response : raw.get_document().view()) {
                return shard_response.get_document().view();
            }
        }

        return reply_owner.view();
    }();

    if (auto const note = reply["note"]) {
        if (note.get_string().value == "all indexes already exist") {
            return {};
        }
    }

    if (auto const name = index.options()["name"]) {
        return std::string{name.get_string().value};
    }

    return keys_to_index_string(index.keys());
}

bsoncxx::v1::document::value create_many_impl(
    mongoc_collection_t* coll,
    std::vector<indexes::model> const& indexes,
    bson_t const* opts,
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> commit_quorum) {
    struct builder_deleter_type {
        void operator()(bson_array_builder_t* ptr) const noexcept {
            bson_array_builder_destroy(ptr);
        }
    };

    using builder_ptr_type = std::unique_ptr<bson_array_builder_t, builder_deleter_type>;

    auto const builder_owner = builder_ptr_type{bson_array_builder_new(), builder_deleter_type{}};
    auto const builder = builder_owner.get();

    for (auto const& model : indexes) {
        scoped_bson doc;

        append_to(model, doc);

        bson_array_builder_append_document(builder, doc.bson());
    }

    scoped_bson arr;
    if (!bson_array_builder_build(builder, arr.out_ptr())) {
        throw std::logic_error{"mongocxx::v1::create_many_impl: bson_array_builder_build failed"};
    }
    return create_impl(coll, arr.bson(), opts, commit_quorum);
}

void drop_impl(mongoc_collection_t* coll, bson_t const* command, bson_t const* opts) {
    scoped_bson reply;
    bson_error_t error = {};

    if (!libmongoc::collection_write_command_with_opts(coll, command, opts, reply.out_ptr(), &error)) {
        v1::throw_exception(error, std::move(reply).value());
    }
}

void drop_one_impl(mongoc_collection_t* coll, bsoncxx::v1::stdx::string_view name, bson_t const* opts) {
    if (name == "*") {
        throw v1::exception::internal::make(code::invalid_name);
    }

    drop_impl(
        coll,
        scoped_bson{BCON_NEW(
                        "dropIndexes",
                        BCON_UTF8(libmongoc::collection_get_name(coll)),
                        "index",
                        BCON_UTF8(std::string{name}.c_str()))}
            .bson(),
        opts);
}

void drop_all_impl(mongoc_collection_t* coll, bson_t const* opts) {
    drop_impl(
        coll,
        scoped_bson{BCON_NEW("dropIndexes", BCON_UTF8(libmongoc::collection_get_name(coll)), "index", "*")}.bson(),
        opts);
}

} // namespace

v1::cursor indexes::list() {
    return list_impl(impl::with(this)->_coll, nullptr);
}

v1::cursor indexes::list(v1::client_session const& session) {
    scoped_bson doc;

    v1::client_session::internal::append_to(session, doc);

    return list_impl(impl::with(this)->_coll, doc.bson());
}

bsoncxx::v1::stdx::optional<std::string> indexes::create_one(model const& index, options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    return create_one_impl(impl::with(this)->_coll, index, doc.bson(), opts.commit_quorum());
}

bsoncxx::v1::stdx::optional<std::string>
indexes::create_one(v1::client_session const& session, model const& index, options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    v1::client_session::internal::append_to(session, doc);

    return create_one_impl(impl::with(this)->_coll, index, doc.bson(), opts.commit_quorum());
}

bsoncxx::v1::document::value indexes::create_many(std::vector<model> const& indexes, options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    return create_many_impl(impl::with(this)->_coll, indexes, doc.bson(), opts.commit_quorum());
}

bsoncxx::v1::document::value
indexes::create_many(v1::client_session const& session, std::vector<model> const& indexes, options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    v1::client_session::internal::append_to(session, doc);

    return create_many_impl(impl::with(this)->_coll, indexes, doc.bson(), opts.commit_quorum());
}

void indexes::drop_one(bsoncxx::v1::stdx::string_view index, options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    drop_one_impl(impl::with(this)->_coll, index, doc.bson());
}

void indexes::drop_one(v1::client_session const& session, bsoncxx::v1::stdx::string_view index, options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    v1::client_session::internal::append_to(session, doc);

    drop_one_impl(impl::with(this)->_coll, index, doc.bson());
}

void indexes::drop_one(model const& index, options const& opts) {
    auto const name = index.options()["name"];

    if (name && name.type_id() == bsoncxx::v1::types::id::k_string) {
        this->drop_one(name.get_string().value, opts);
    } else {
        this->drop_one(keys_to_index_string(index.keys()), opts);
    }
}

void indexes::drop_one(v1::client_session const& session, model const& index, options const& opts) {
    auto const name = index.options()["name"];

    if (name && name.type_id() == bsoncxx::v1::types::id::k_string) {
        this->drop_one(session, name.get_string().value, opts);
    } else {
        this->drop_one(session, keys_to_index_string(index.keys()), opts);
    }
}

void indexes::drop_all(options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    drop_all_impl(impl::with(this)->_coll, doc.bson());
}

void indexes::drop_all(v1::client_session const& session, options const& opts) {
    scoped_bson doc;

    append_to(opts, doc);

    v1::client_session::internal::append_to(session, doc);

    drop_all_impl(impl::with(this)->_coll, doc.bson());
}

std::error_category const& indexes::error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "mongocxx::v1::indexes";
        }

        std::string message(int v) const noexcept override {
            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::invalid_name:
                    return "\"*\" is not a permitted index name";
                default:
                    return std::string(this->name()) + ':' + std::to_string(v);
            }
        }

        bool equivalent(int v, std::error_condition const& ec) const noexcept override {
            if (ec.category() == v1::source_error_category()) {
                using condition = v1::source_errc;

                auto const source = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::invalid_name:
                        return source == condition::mongocxx;

                    case code::zero:
                    default:
                        return false;
                }
            }

            if (ec.category() == v1::type_error_category()) {
                using condition = v1::type_errc;

                auto const type = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::invalid_name:
                        return type == condition::invalid_argument;

                    case code::zero:
                    default:
                        return false;
                }
            }

            return false;
        }
    };

    static bsoncxx::immortal<type> const instance;

    return instance.value();
}

indexes::indexes(void* impl) : _impl(impl) {}

class indexes::model::impl {
   public:
    bsoncxx::v1::document::value _keys;
    bsoncxx::v1::document::value _options;

    impl(bsoncxx::v1::document::value keys, bsoncxx::v1::document::value options)
        : _keys{std::move(keys)}, _options{std::move(options)} {}

    explicit impl(bsoncxx::v1::document::value keys) : _keys{std::move(keys)} {}

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

indexes::model::~model() {
    delete impl::with(_impl);
}

indexes::model::model(model&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

indexes::model& indexes::model::operator=(model&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

indexes::model::model(model const& other) : _impl{new impl{impl::with(other)}} {}

indexes::model& indexes::model::operator=(model const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

indexes::model::model(bsoncxx::v1::document::value keys, bsoncxx::v1::document::value options)
    : _impl{new impl{std::move(keys), std::move(options)}} {}

indexes::model::model(bsoncxx::v1::document::value keys) : _impl{new impl{std::move(keys)}} {}

bsoncxx::v1::document::view indexes::model::keys() const {
    return impl::with(this)->_keys;
}

bsoncxx::v1::document::view indexes::model::options() const {
    return impl::with(this)->_options;
}

class indexes::options::impl {
   public:
    // CreateIndexOptions
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> _max_time;
    bsoncxx::v1::stdx::optional<v1::write_concern> _write_concern;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _commit_quorum;

    // IndexOptions
    bsoncxx::v1::stdx::optional<bool> _background;
    bsoncxx::v1::stdx::optional<bool> _unique;
    bsoncxx::v1::stdx::optional<bool> _hidden;
    bsoncxx::v1::stdx::optional<std::string> _name;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<bool> _sparse;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _storage_engine;
    bsoncxx::v1::stdx::optional<std::chrono::seconds> _expire_after;
    bsoncxx::v1::stdx::optional<std::int32_t> _version;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _weights;
    bsoncxx::v1::stdx::optional<std::string> _default_language;
    bsoncxx::v1::stdx::optional<std::string> _language_override;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _partial_filter_expression;
    bsoncxx::v1::stdx::optional<std::uint8_t> _twod_sphere_version;
    bsoncxx::v1::stdx::optional<std::uint8_t> _twod_bits_precision;
    bsoncxx::v1::stdx::optional<double> _twod_location_min;
    bsoncxx::v1::stdx::optional<double> _twod_location_max;
    bsoncxx::v1::stdx::optional<double> _haystack_bucket_size;

    static impl const& with(options const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(options const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(options& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(options* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

indexes::options::~options() {
    delete impl::with(_impl);
}

indexes::options::options(options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

indexes::options& indexes::options::operator=(options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

indexes::options::options(options const& other) : _impl{new impl{impl::with(other)}} {}

indexes::options& indexes::options::operator=(options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

indexes::options::options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

indexes::options& indexes::options::max_time(std::chrono::milliseconds max_time) {
    impl::with(this)->_max_time = max_time;
    return *this;
}

bsoncxx::v1::stdx::optional<std::chrono::milliseconds> indexes::options::max_time() const {
    return impl::with(this)->_max_time;
}

indexes::options& indexes::options::write_concern(v1::write_concern write_concern) {
    impl::with(this)->_write_concern = std::move(write_concern);
    return *this;
}
bsoncxx::v1::stdx::optional<v1::write_concern> indexes::options::write_concern() const {
    return impl::with(this)->_write_concern;
}

indexes::options& indexes::options::commit_quorum(std::int32_t commit_quorum) {
    impl::with(this)->_commit_quorum = scoped_bson{BCON_NEW("commitQuorum", BCON_INT32(commit_quorum))}.value();
    return *this;
}

indexes::options& indexes::options::commit_quorum(bsoncxx::v1::stdx::string_view commit_quorum) {
    impl::with(this)->_commit_quorum =
        scoped_bson{BCON_NEW("commitQuorum", BCON_UTF8(std::string{commit_quorum}.c_str()))}.value();
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> const indexes::options::commit_quorum() const {
    return impl::with(this)->_commit_quorum;
}

indexes::options& indexes::options::background(bool background) {
    impl::with(this)->_background = background;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> indexes::options::background() const {
    return impl::with(this)->_background;
}

indexes::options& indexes::options::unique(bool unique) {
    impl::with(this)->_unique = unique;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> indexes::options::unique() const {
    return impl::with(this)->_unique;
}

indexes::options& indexes::options::hidden(bool hidden) {
    impl::with(this)->_hidden = hidden;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> indexes::options::hidden() const {
    return impl::with(this)->_hidden;
}

indexes::options& indexes::options::name(std::string name) {
    impl::with(this)->_name = std::move(name);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> indexes::options::name() const {
    return impl::with(this)->_name;
}

indexes::options& indexes::options::collation(bsoncxx::v1::document::value collation) {
    impl::with(this)->_collation = std::move(collation);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> indexes::options::collation() const {
    return impl::with(this)->_collation;
}

indexes::options& indexes::options::sparse(bool sparse) {
    impl::with(this)->_sparse = sparse;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> indexes::options::sparse() const {
    return impl::with(this)->_sparse;
}

indexes::options& indexes::options::storage_engine(bsoncxx::v1::document::value storage_engine) {
    impl::with(this)->_storage_engine = std::move(storage_engine);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> indexes::options::storage_engine() const {
    return impl::with(this)->_storage_engine;
}

indexes::options& indexes::options::expire_after(std::chrono::seconds seconds) {
    impl::with(this)->_expire_after = seconds;
    return *this;
}

bsoncxx::v1::stdx::optional<std::chrono::seconds> indexes::options::expire_after() const {
    return impl::with(this)->_expire_after;
}

indexes::options& indexes::options::version(std::int32_t v) {
    impl::with(this)->_version = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> indexes::options::version() const {
    return impl::with(this)->_version;
}

indexes::options& indexes::options::weights(bsoncxx::v1::document::value weights) {
    impl::with(this)->_weights = std::move(weights);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> indexes::options::weights() const {
    return impl::with(this)->_weights;
}

indexes::options& indexes::options::default_language(std::string default_language) {
    impl::with(this)->_default_language = std::move(default_language);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> indexes::options::default_language() const {
    return impl::with(this)->_default_language;
}

indexes::options& indexes::options::language_override(std::string language_override) {
    impl::with(this)->_language_override = std::move(language_override);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> indexes::options::language_override() const {
    return impl::with(this)->_language_override;
}

indexes::options& indexes::options::partial_filter_expression(bsoncxx::v1::document::value partial_filter_expression) {
    impl::with(this)->_partial_filter_expression = std::move(partial_filter_expression);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> indexes::options::partial_filter_expression() const {
    return impl::with(this)->_partial_filter_expression;
}

indexes::options& indexes::options::twod_sphere_version(std::uint8_t twod_sphere_version) {
    impl::with(this)->_twod_sphere_version = twod_sphere_version;
    return *this;
}

bsoncxx::v1::stdx::optional<std::uint8_t> indexes::options::twod_sphere_version() const {
    return impl::with(this)->_twod_sphere_version;
}

indexes::options& indexes::options::twod_bits_precision(std::uint8_t twod_bits_precision) {
    impl::with(this)->_twod_bits_precision = twod_bits_precision;
    return *this;
}

bsoncxx::v1::stdx::optional<std::uint8_t> indexes::options::twod_bits_precision() const {
    return impl::with(this)->_twod_bits_precision;
}

indexes::options& indexes::options::twod_location_min(double twod_location_min) {
    impl::with(this)->_twod_location_min = twod_location_min;
    return *this;
}

bsoncxx::v1::stdx::optional<double> indexes::options::twod_location_min() const {
    return impl::with(this)->_twod_location_min;
}

indexes::options& indexes::options::twod_location_max(double twod_location_max) {
    impl::with(this)->_twod_location_max = twod_location_max;
    return *this;
}

bsoncxx::v1::stdx::optional<double> indexes::options::twod_location_max() const {
    return impl::with(this)->_twod_location_max;
}

indexes::options::operator bsoncxx::v1::document::value() const {
    scoped_bson ret;

    if (auto const& opt = impl::with(this)->_name) {
        ret += scoped_bson{BCON_NEW("name", BCON_UTF8(opt->c_str()))};
    }

    if (auto const& opt = impl::with(this)->_background) {
        ret += scoped_bson{BCON_NEW("background", BCON_BOOL(*opt))};
    }

    if (auto const& opt = impl::with(this)->_unique) {
        ret += scoped_bson{BCON_NEW("unique", BCON_BOOL(*opt))};
    }

    if (auto const& opt = impl::with(this)->_hidden) {
        ret += scoped_bson{BCON_NEW("hidden", BCON_BOOL(*opt))};
    }

    if (auto const& opt = impl::with(this)->_partial_filter_expression) {
        ret += scoped_bson{BCON_NEW("partialFilterExpression", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = impl::with(this)->_sparse) {
        ret += scoped_bson{BCON_NEW("sparse", BCON_BOOL(*opt))};
    }

    if (auto const& opt = impl::with(this)->_expire_after) {
        ret += scoped_bson{BCON_NEW("expireAfterSeconds", BCON_INT64(opt->count()))};
    }

    if (auto const& opt = impl::with(this)->_weights) {
        ret += scoped_bson{BCON_NEW("weights", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = impl::with(this)->_default_language) {
        ret += scoped_bson{BCON_NEW("default_language", BCON_UTF8(opt->c_str()))};
    }

    if (auto const& opt = impl::with(this)->_language_override) {
        ret += scoped_bson{BCON_NEW("language_override", BCON_UTF8(opt->c_str()))};
    }

    if (auto const& opt = impl::with(this)->_twod_sphere_version) {
        ret += scoped_bson{BCON_NEW("2dsphereIndexVersion", BCON_INT32(std::int32_t{*opt}))};
    }

    if (auto const& opt = impl::with(this)->_twod_bits_precision) {
        ret += scoped_bson{BCON_NEW("bits", BCON_INT32(*opt))};
    }

    if (auto const& opt = impl::with(this)->_twod_location_min) {
        ret += scoped_bson{BCON_NEW("min", BCON_DOUBLE(*opt))};
    }

    if (auto const& opt = impl::with(this)->_twod_location_max) {
        ret += scoped_bson{BCON_NEW("max", BCON_DOUBLE(*opt))};
    }

    if (auto const& opt = impl::with(this)->_haystack_bucket_size) {
        ret += scoped_bson{BCON_NEW("bucketSize", BCON_DOUBLE(*opt))};
    }

    if (auto const& opt = impl::with(this)->_collation) {
        ret += scoped_bson{BCON_NEW("collation", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = impl::with(this)->_storage_engine) {
        ret += scoped_bson{BCON_NEW("storageEngine", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    return std::move(ret).value();
}

indexes indexes::internal::make(mongoc_collection_t* coll, mongoc_client_t* client) {
    return {new impl{coll, client}};
}

mongoc_collection_t const* indexes::internal::get_collection(indexes const& self) {
    return impl::with(self)._coll;
}

mongoc_client_t const* indexes::internal::get_client(indexes const& self) {
    return impl::with(self)._client;
}

mongoc_collection_t* indexes::internal::get_collection(indexes& self) {
    return impl::with(self)._coll;
}

mongoc_client_t* indexes::internal::get_client(indexes& self) {
    return impl::with(self)._client;
}

bsoncxx::v1::stdx::optional<v1::write_concern> const& indexes::options::internal::write_concern(options const& self) {
    return impl::with(self)._write_concern;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& indexes::options::internal::commit_quorum(
    options const& self) {
    return impl::with(self)._commit_quorum;
}

bsoncxx::v1::stdx::optional<v1::write_concern>& indexes::options::internal::write_concern(options& self) {
    return impl::with(self)._write_concern;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& indexes::options::internal::commit_quorum(options& self) {
    return impl::with(self)._commit_quorum;
}

bsoncxx::v1::stdx::optional<std::string>& indexes::options::internal::name(options& self) {
    return impl::with(self)._name;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& indexes::options::internal::collation(options& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& indexes::options::internal::storage_engine(options& self) {
    return impl::with(self)._storage_engine;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& indexes::options::internal::weights(options& self) {
    return impl::with(self)._weights;
}

bsoncxx::v1::stdx::optional<std::string>& indexes::options::internal::default_language(options& self) {
    return impl::with(self)._default_language;
}

bsoncxx::v1::stdx::optional<std::string>& indexes::options::internal::language_override(options& self) {
    return impl::with(self)._language_override;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& indexes::options::internal::partial_filter_expression(
    options& self) {
    return impl::with(self)._partial_filter_expression;
}

} // namespace v1
} // namespace mongocxx
