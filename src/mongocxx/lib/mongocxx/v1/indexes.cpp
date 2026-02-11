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

#include <bsoncxx/v1/types/value.hh>

#include <mongocxx/v1/client_session.hh>
#include <mongocxx/v1/cursor.hh>
#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/write_concern.hh>

#include <chrono>
#include <cstdint>
#include <limits>
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

void append_comment(bsoncxx::v1::types::value const& opt, scoped_bson& doc) {
    scoped_bson v;

    if (!BSON_APPEND_VALUE(v.out_ptr(), "comment", &bsoncxx::v1::types::value::internal::get_bson_value(opt))) {
        throw std::logic_error{"mongocxx::v1::append_comment: BSON_APPEND_VALUE failed"};
    }

    doc += v;
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

void append_to(indexes::list_options const& opts, scoped_bson& doc) {
    if (auto const opt = opts.batch_size()) {
        doc += scoped_bson{BCON_NEW("batchSize", BCON_INT32(*opt))};
    }

    if (auto const& opt = indexes::list_options::internal::comment(opts)) {
        append_comment(*opt, doc);
    }
}

class create_index_options_impl {
   public:
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _comment;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _commit_quorum;
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> _max_time;
    bsoncxx::v1::stdx::optional<v1::write_concern> _write_concern;

    static create_index_options_impl* with(void* ptr) {
        return static_cast<create_index_options_impl*>(ptr);
    }
};

template <typename CreateIndexOptions>
void append_create_to(CreateIndexOptions const& opts, scoped_bson& doc) {
    if (auto const& opt = CreateIndexOptions::internal::comment(opts)) {
        append_comment(*opt, doc);
    }

    if (auto const& opt = opts.max_time()) {
        doc += scoped_bson{BCON_NEW("maxTimeMS", BCON_INT64(std::int64_t{opt->count()}))};
    }

    if (auto const& opt = CreateIndexOptions::internal::write_concern(opts)) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{opt->to_document()}.bson()))};
    }
}

class drop_index_options_impl {
   public:
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _comment;
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> _max_time;
    bsoncxx::v1::stdx::optional<v1::write_concern> _write_concern;

    static drop_index_options_impl* with(void* ptr) {
        return static_cast<drop_index_options_impl*>(ptr);
    }
};

template <typename DropIndexOptions>
void append_drop_to(DropIndexOptions const& opts, scoped_bson& doc) {
    if (auto const& opt = DropIndexOptions::internal::comment(opts)) {
        append_comment(*opt, doc);
    }

    if (auto const& opt = opts.max_time()) {
        doc += scoped_bson{BCON_NEW("maxTimeMS", BCON_INT64(std::int64_t{opt->count()}))};
    }

    if (auto const& opt = DropIndexOptions::internal::write_concern(opts)) {
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

} // namespace

v1::cursor indexes::list(list_options const& list_opts) {
    scoped_bson doc;

    append_to(list_opts, doc);

    return list_impl(impl::with(this)->_coll, doc.bson());
}

v1::cursor indexes::list(v1::client_session const& session, list_options const& list_opts) {
    scoped_bson doc;

    append_to(list_opts, doc);

    v1::client_session::internal::append_to(session, doc);

    return list_impl(impl::with(this)->_coll, doc.bson());
}

bsoncxx::v1::stdx::optional<std::string> indexes::create_one(
    model const& index,
    create_one_options const& create_opts) {
    scoped_bson doc;

    append_create_to(create_opts, doc);

    return create_one_impl(impl::with(this)->_coll, index, doc.bson(), create_opts.commit_quorum());
}

bsoncxx::v1::stdx::optional<std::string>
indexes::create_one(v1::client_session const& session, model const& index, create_one_options const& create_opts) {
    scoped_bson doc;

    append_create_to(create_opts, doc);

    v1::client_session::internal::append_to(session, doc);

    return create_one_impl(impl::with(this)->_coll, index, doc.bson(), create_opts.commit_quorum());
}

bsoncxx::v1::document::value indexes::create_many(
    std::vector<model> const& indexes,
    create_many_options const& create_opts) {
    scoped_bson doc;

    append_create_to(create_opts, doc);

    return create_many_impl(impl::with(this)->_coll, indexes, doc.bson(), create_opts.commit_quorum());
}

bsoncxx::v1::document::value indexes::create_many(
    v1::client_session const& session,
    std::vector<model> const& indexes,
    create_many_options const& create_opts) {
    scoped_bson doc;

    append_create_to(create_opts, doc);

    v1::client_session::internal::append_to(session, doc);

    return create_many_impl(impl::with(this)->_coll, indexes, doc.bson(), create_opts.commit_quorum());
}

void indexes::drop_one(bsoncxx::v1::stdx::string_view index, drop_one_options const& drop_opts) {
    scoped_bson doc;

    append_drop_to(drop_opts, doc);

    internal::drop_one_impl(impl::with(this)->_coll, index, doc.bson());
}

void indexes::drop_one(
    v1::client_session const& session,
    bsoncxx::v1::stdx::string_view index,
    drop_one_options const& drop_opts) {
    scoped_bson doc;

    append_drop_to(drop_opts, doc);

    v1::client_session::internal::append_to(session, doc);

    internal::drop_one_impl(impl::with(this)->_coll, index, doc.bson());
}

void indexes::drop_one(model const& index, drop_one_options const& drop_opts) {
    auto const name = index.options()["name"];

    if (name && name.type_id() == bsoncxx::v1::types::id::k_string) {
        this->drop_one(name.get_string().value, drop_opts);
    } else {
        this->drop_one(keys_to_index_string(index.keys()), drop_opts);
    }
}

void indexes::drop_one(v1::client_session const& session, model const& index, drop_one_options const& drop_opts) {
    auto const name = index.options()["name"];

    if (name && name.type_id() == bsoncxx::v1::types::id::k_string) {
        this->drop_one(session, name.get_string().value, drop_opts);
    } else {
        this->drop_one(session, keys_to_index_string(index.keys()), drop_opts);
    }
}

void indexes::drop_all(drop_all_options const& drop_opts) {
    scoped_bson doc;

    append_drop_to(drop_opts, doc);

    internal::drop_all_impl(impl::with(this)->_coll, doc.bson());
}

void indexes::drop_all(v1::client_session const& session, drop_all_options const& drop_opts) {
    scoped_bson doc;

    append_drop_to(drop_opts, doc);

    v1::client_session::internal::append_to(session, doc);

    internal::drop_all_impl(impl::with(this)->_coll, doc.bson());
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
                case code::expired_after_i32:
                    return "the \"expiredAfterSeconds\" field must be representable as an `std::int32_t`";
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
                    case code::expired_after_i32:
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
                    case code::expired_after_i32:
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
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _partial_filter_expression;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _storage_engine;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _weights;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _wildcard_projection;
    bsoncxx::v1::stdx::optional<std::string> _default_language;
    bsoncxx::v1::stdx::optional<std::string> _language_override;
    bsoncxx::v1::stdx::optional<std::string> _name;
    bsoncxx::v1::stdx::optional<double> _twod_location_max;
    bsoncxx::v1::stdx::optional<double> _twod_location_min;
    bsoncxx::v1::stdx::optional<std::chrono::seconds> _expire_after;
    bsoncxx::v1::stdx::optional<std::int32_t> _text_index_version;
    bsoncxx::v1::stdx::optional<std::int32_t> _version;
    bsoncxx::v1::stdx::optional<std::uint8_t> _twod_bits_precision;
    bsoncxx::v1::stdx::optional<std::uint8_t> _twod_sphere_version;
    bsoncxx::v1::stdx::optional<bool> _background;
    bsoncxx::v1::stdx::optional<bool> _hidden;
    bsoncxx::v1::stdx::optional<bool> _sparse;
    bsoncxx::v1::stdx::optional<bool> _unique;

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

indexes::options& indexes::options::background(bool v) {
    impl::with(this)->_background = v;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> indexes::options::background() const {
    return impl::with(this)->_background;
}

indexes::options& indexes::options::collation(bsoncxx::v1::document::value v) {
    impl::with(this)->_collation = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> indexes::options::collation() const {
    return impl::with(this)->_collation;
}

indexes::options& indexes::options::default_language(std::string v) {
    impl::with(this)->_default_language = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> indexes::options::default_language() const {
    return impl::with(this)->_default_language;
}

indexes::options& indexes::options::expire_after(std::chrono::seconds v) {
    impl::with(this)->_expire_after = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::chrono::seconds> indexes::options::expire_after() const {
    return impl::with(this)->_expire_after;
}

indexes::options& indexes::options::hidden(bool v) {
    impl::with(this)->_hidden = v;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> indexes::options::hidden() const {
    return impl::with(this)->_hidden;
}

indexes::options& indexes::options::language_override(std::string v) {
    impl::with(this)->_language_override = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> indexes::options::language_override() const {
    return impl::with(this)->_language_override;
}

indexes::options& indexes::options::name(std::string v) {
    impl::with(this)->_name = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> indexes::options::name() const {
    return impl::with(this)->_name;
}

indexes::options& indexes::options::partial_filter_expression(bsoncxx::v1::document::value v) {
    impl::with(this)->_partial_filter_expression = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> indexes::options::partial_filter_expression() const {
    return impl::with(this)->_partial_filter_expression;
}

indexes::options& indexes::options::sparse(bool v) {
    impl::with(this)->_sparse = v;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> indexes::options::sparse() const {
    return impl::with(this)->_sparse;
}

indexes::options& indexes::options::storage_engine(bsoncxx::v1::document::value v) {
    impl::with(this)->_storage_engine = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> indexes::options::storage_engine() const {
    return impl::with(this)->_storage_engine;
}

indexes::options& indexes::options::text_index_version(std::int32_t v) {
    impl::with(this)->_text_index_version = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> indexes::options::text_index_version() const {
    return impl::with(this)->_text_index_version;
}

indexes::options& indexes::options::twod_bits_precision(std::uint8_t v) {
    impl::with(this)->_twod_bits_precision = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::uint8_t> indexes::options::twod_bits_precision() const {
    return impl::with(this)->_twod_bits_precision;
}

indexes::options& indexes::options::twod_location_max(double v) {
    impl::with(this)->_twod_location_max = v;
    return *this;
}

bsoncxx::v1::stdx::optional<double> indexes::options::twod_location_max() const {
    return impl::with(this)->_twod_location_max;
}

indexes::options& indexes::options::twod_location_min(double v) {
    impl::with(this)->_twod_location_min = v;
    return *this;
}

bsoncxx::v1::stdx::optional<double> indexes::options::twod_location_min() const {
    return impl::with(this)->_twod_location_min;
}

indexes::options& indexes::options::twod_sphere_version(std::uint8_t v) {
    impl::with(this)->_twod_sphere_version = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::uint8_t> indexes::options::twod_sphere_version() const {
    return impl::with(this)->_twod_sphere_version;
}

indexes::options& indexes::options::unique(bool v) {
    impl::with(this)->_unique = v;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> indexes::options::unique() const {
    return impl::with(this)->_unique;
}

indexes::options& indexes::options::version(std::int32_t v) {
    impl::with(this)->_version = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> indexes::options::version() const {
    return impl::with(this)->_version;
}

indexes::options& indexes::options::wildcard_projection(bsoncxx::v1::document::value v) {
    impl::with(this)->_wildcard_projection = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> indexes::options::wildcard_projection() const {
    return impl::with(this)->_wildcard_projection;
}

indexes::options& indexes::options::weights(bsoncxx::v1::document::value v) {
    impl::with(this)->_weights = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> indexes::options::weights() const {
    return impl::with(this)->_weights;
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
        auto const v = opt->count();

        if (v < std::numeric_limits<std::int32_t>::min() || v > std::numeric_limits<std::int32_t>::max()) {
            throw v1::exception::internal::make(code::expired_after_i32);
        }

        ret += scoped_bson{BCON_NEW("expireAfterSeconds", BCON_INT32(static_cast<std::int32_t>(v)))};
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

    if (auto const& opt = impl::with(this)->_collation) {
        ret += scoped_bson{BCON_NEW("collation", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = impl::with(this)->_storage_engine) {
        ret += scoped_bson{BCON_NEW("storageEngine", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = impl::with(this)->_wildcard_projection) {
        ret += scoped_bson{BCON_NEW("wildcardProjection", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    return std::move(ret).value();
}

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

indexes::create_one_options::~create_one_options() {
    delete create_index_options_impl::with(_impl);
}

indexes::create_one_options::create_one_options(create_one_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

indexes::create_one_options& indexes::create_one_options::operator=(create_one_options&& other) noexcept {
    if (this != &other) {
        delete create_index_options_impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

indexes::create_one_options::create_one_options(create_one_options const& other)
    : _impl{new create_index_options_impl{*create_index_options_impl::with(other._impl)}} {}

indexes::create_one_options& indexes::create_one_options::operator=(create_one_options const& other) {
    if (this != &other) {
        delete create_index_options_impl::with(
            exchange(_impl, new create_index_options_impl{*create_index_options_impl::with(other._impl)}));
    }

    return *this;
}

indexes::create_one_options::create_one_options() : _impl{new create_index_options_impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

indexes::create_one_options& indexes::create_one_options::comment(bsoncxx::v1::types::value v) {
    create_index_options_impl::with(_impl)->_comment = std::move(v);
    return *this;
}

indexes::create_one_options& indexes::create_one_options::commit_quorum(bsoncxx::v1::document::value v) {
    create_index_options_impl::with(_impl)->_commit_quorum = std::move(v);
    return *this;
}

indexes::create_one_options& indexes::create_one_options::max_time(std::chrono::milliseconds v) {
    create_index_options_impl::with(_impl)->_max_time = v;
    return *this;
}

indexes::create_one_options& indexes::create_one_options::write_concern(v1::write_concern v) {
    create_index_options_impl::with(_impl)->_write_concern = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> indexes::create_one_options::comment() const {
    return create_index_options_impl::with(_impl)->_comment;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> indexes::create_one_options::commit_quorum() const {
    return create_index_options_impl::with(_impl)->_commit_quorum;
}

bsoncxx::v1::stdx::optional<std::chrono::milliseconds> indexes::create_one_options::max_time() const {
    return create_index_options_impl::with(_impl)->_max_time;
}

bsoncxx::v1::stdx::optional<v1::write_concern> indexes::create_one_options::write_concern() const {
    return create_index_options_impl::with(_impl)->_write_concern;
}

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

indexes::create_many_options::~create_many_options() {
    delete create_index_options_impl::with(_impl);
}

indexes::create_many_options::create_many_options(create_many_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

indexes::create_many_options& indexes::create_many_options::operator=(create_many_options&& other) noexcept {
    if (this != &other) {
        delete create_index_options_impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

indexes::create_many_options::create_many_options(create_many_options const& other)
    : _impl{new create_index_options_impl{*create_index_options_impl::with(other._impl)}} {}

indexes::create_many_options& indexes::create_many_options::operator=(create_many_options const& other) {
    if (this != &other) {
        delete create_index_options_impl::with(
            exchange(_impl, new create_index_options_impl{*create_index_options_impl::with(other._impl)}));
    }

    return *this;
}

indexes::create_many_options::create_many_options() : _impl{new create_index_options_impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

indexes::create_many_options& indexes::create_many_options::comment(bsoncxx::v1::types::value v) {
    create_index_options_impl::with(_impl)->_comment = std::move(v);
    return *this;
}

indexes::create_many_options& indexes::create_many_options::commit_quorum(bsoncxx::v1::document::value v) {
    create_index_options_impl::with(_impl)->_commit_quorum = std::move(v);
    return *this;
}

indexes::create_many_options& indexes::create_many_options::max_time(std::chrono::milliseconds v) {
    create_index_options_impl::with(_impl)->_max_time = v;
    return *this;
}

indexes::create_many_options& indexes::create_many_options::write_concern(v1::write_concern v) {
    create_index_options_impl::with(_impl)->_write_concern = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> indexes::create_many_options::comment() const {
    return create_index_options_impl::with(_impl)->_comment;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> indexes::create_many_options::commit_quorum() const {
    return create_index_options_impl::with(_impl)->_commit_quorum;
}

bsoncxx::v1::stdx::optional<std::chrono::milliseconds> indexes::create_many_options::max_time() const {
    return create_index_options_impl::with(_impl)->_max_time;
}

bsoncxx::v1::stdx::optional<v1::write_concern> indexes::create_many_options::write_concern() const {
    return create_index_options_impl::with(_impl)->_write_concern;
}

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

indexes::drop_one_options::~drop_one_options() {
    delete drop_index_options_impl::with(_impl);
}

indexes::drop_one_options::drop_one_options(drop_one_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

indexes::drop_one_options& indexes::drop_one_options::operator=(drop_one_options&& other) noexcept {
    if (this != &other) {
        delete drop_index_options_impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

indexes::drop_one_options::drop_one_options(drop_one_options const& other)
    : _impl{new drop_index_options_impl{*drop_index_options_impl::with(other._impl)}} {}

indexes::drop_one_options& indexes::drop_one_options::operator=(drop_one_options const& other) {
    if (this != &other) {
        delete drop_index_options_impl::with(
            exchange(_impl, new drop_index_options_impl{*drop_index_options_impl::with(other._impl)}));
    }

    return *this;
}

indexes::drop_one_options::drop_one_options() : _impl{new drop_index_options_impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

indexes::drop_one_options& indexes::drop_one_options::comment(bsoncxx::v1::types::value v) {
    drop_index_options_impl::with(_impl)->_comment = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> indexes::drop_one_options::comment() const {
    return drop_index_options_impl::with(_impl)->_comment;
}

indexes::drop_one_options& indexes::drop_one_options::max_time(std::chrono::milliseconds v) {
    drop_index_options_impl::with(_impl)->_max_time = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::chrono::milliseconds> indexes::drop_one_options::max_time() const {
    return drop_index_options_impl::with(_impl)->_max_time;
}

indexes::drop_one_options& indexes::drop_one_options::write_concern(v1::write_concern v) {
    drop_index_options_impl::with(_impl)->_write_concern = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::write_concern> indexes::drop_one_options::write_concern() const {
    return drop_index_options_impl::with(_impl)->_write_concern;
}

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

indexes::drop_all_options::~drop_all_options() {
    delete drop_index_options_impl::with(_impl);
}

indexes::drop_all_options::drop_all_options(drop_all_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

indexes::drop_all_options& indexes::drop_all_options::operator=(drop_all_options&& other) noexcept {
    if (this != &other) {
        delete drop_index_options_impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

indexes::drop_all_options::drop_all_options(drop_all_options const& other)
    : _impl{new drop_index_options_impl{*drop_index_options_impl::with(other._impl)}} {}

indexes::drop_all_options& indexes::drop_all_options::operator=(drop_all_options const& other) {
    if (this != &other) {
        delete drop_index_options_impl::with(
            exchange(_impl, new drop_index_options_impl{*drop_index_options_impl::with(other._impl)}));
    }

    return *this;
}

indexes::drop_all_options::drop_all_options() : _impl{new drop_index_options_impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

indexes::drop_all_options& indexes::drop_all_options::comment(bsoncxx::v1::types::value v) {
    drop_index_options_impl::with(_impl)->_comment = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> indexes::drop_all_options::comment() const {
    return drop_index_options_impl::with(_impl)->_comment;
}

indexes::drop_all_options& indexes::drop_all_options::max_time(std::chrono::milliseconds v) {
    drop_index_options_impl::with(_impl)->_max_time = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::chrono::milliseconds> indexes::drop_all_options::max_time() const {
    return drop_index_options_impl::with(_impl)->_max_time;
}

indexes::drop_all_options& indexes::drop_all_options::write_concern(v1::write_concern v) {
    drop_index_options_impl::with(_impl)->_write_concern = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::write_concern> indexes::drop_all_options::write_concern() const {
    return drop_index_options_impl::with(_impl)->_write_concern;
}

class indexes::list_options::impl {
   public:
    bsoncxx::v1::stdx::optional<std::int32_t> _batch_size;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _comment;

    static impl const& with(list_options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(list_options const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(list_options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(list_options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

indexes::list_options::~list_options() {
    delete impl::with(this);
}

indexes::list_options::list_options(list_options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

indexes::list_options& indexes::list_options::operator=(list_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

indexes::list_options::list_options(list_options const& other) : _impl{new impl{impl::with(other)}} {}

indexes::list_options& indexes::list_options::operator=(list_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

indexes::list_options::list_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

indexes::list_options& indexes::list_options::batch_size(std::int32_t v) {
    impl::with(this)->_batch_size = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> indexes::list_options::batch_size() const {
    return impl::with(this)->_batch_size;
}

indexes::list_options& indexes::list_options::comment(bsoncxx::v1::types::value v) {
    impl::with(this)->_comment = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> indexes::list_options::comment() const {
    return impl::with(this)->_comment;
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

void indexes::internal::drop_one_impl(
    mongoc_collection_t* coll,
    bsoncxx::v1::stdx::string_view name,
    bson_t const* opts) {
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

void indexes::internal::drop_all_impl(mongoc_collection_t* coll, bson_t const* opts) {
    drop_impl(
        coll,
        scoped_bson{BCON_NEW("dropIndexes", BCON_UTF8(libmongoc::collection_get_name(coll)), "index", "*")}.bson(),
        opts);
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

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> const& indexes::create_one_options::internal::comment(
    create_one_options const& self) {
    return create_index_options_impl::with(self._impl)->_comment;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& indexes::create_one_options::internal::commit_quorum(
    create_one_options const& self) {
    return create_index_options_impl::with(self._impl)->_commit_quorum;
}

bsoncxx::v1::stdx::optional<v1::write_concern> const& indexes::create_one_options::internal::write_concern(
    create_one_options const& self) {
    return create_index_options_impl::with(self._impl)->_write_concern;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& indexes::create_one_options::internal::comment(
    create_one_options& self) {
    return create_index_options_impl::with(self._impl)->_comment;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& indexes::create_one_options::internal::commit_quorum(
    create_one_options& self) {
    return create_index_options_impl::with(self._impl)->_commit_quorum;
}

bsoncxx::v1::stdx::optional<v1::write_concern>& indexes::create_one_options::internal::write_concern(
    create_one_options& self) {
    return create_index_options_impl::with(self._impl)->_write_concern;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> const& indexes::create_many_options::internal::comment(
    create_many_options const& self) {
    return create_index_options_impl::with(self._impl)->_comment;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& indexes::create_many_options::internal::commit_quorum(
    create_many_options const& self) {
    return create_index_options_impl::with(self._impl)->_commit_quorum;
}

bsoncxx::v1::stdx::optional<v1::write_concern> const& indexes::create_many_options::internal::write_concern(
    create_many_options const& self) {
    return create_index_options_impl::with(self._impl)->_write_concern;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& indexes::create_many_options::internal::comment(
    create_many_options& self) {
    return create_index_options_impl::with(self._impl)->_comment;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& indexes::create_many_options::internal::commit_quorum(
    create_many_options& self) {
    return create_index_options_impl::with(self._impl)->_commit_quorum;
}

bsoncxx::v1::stdx::optional<v1::write_concern>& indexes::create_many_options::internal::write_concern(
    create_many_options& self) {
    return create_index_options_impl::with(self._impl)->_write_concern;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> const& indexes::drop_one_options::internal::comment(
    drop_one_options const& self) {
    return drop_index_options_impl::with(self._impl)->_comment;
}

bsoncxx::v1::stdx::optional<v1::write_concern> const& indexes::drop_one_options::internal::write_concern(
    drop_one_options const& self) {
    return drop_index_options_impl::with(self._impl)->_write_concern;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& indexes::drop_one_options::internal::comment(
    drop_one_options& self) {
    return drop_index_options_impl::with(self._impl)->_comment;
}

bsoncxx::v1::stdx::optional<v1::write_concern>& indexes::drop_one_options::internal::write_concern(
    drop_one_options& self) {
    return drop_index_options_impl::with(self._impl)->_write_concern;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> const& indexes::drop_all_options::internal::comment(
    drop_all_options const& self) {
    return drop_index_options_impl::with(self._impl)->_comment;
}

bsoncxx::v1::stdx::optional<v1::write_concern> const& indexes::drop_all_options::internal::write_concern(
    drop_all_options const& self) {
    return drop_index_options_impl::with(self._impl)->_write_concern;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& indexes::drop_all_options::internal::comment(
    drop_all_options& self) {
    return drop_index_options_impl::with(self._impl)->_comment;
}

bsoncxx::v1::stdx::optional<v1::write_concern>& indexes::drop_all_options::internal::write_concern(
    drop_all_options& self) {
    return drop_index_options_impl::with(self._impl)->_write_concern;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> const& indexes::list_options::internal::comment(
    list_options const& self) {
    return impl::with(self)._comment;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& indexes::list_options::internal::comment(list_options& self) {
    return impl::with(self)._comment;
}

} // namespace v1
} // namespace mongocxx
