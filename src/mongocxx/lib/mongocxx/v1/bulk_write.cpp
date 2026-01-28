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

#include <mongocxx/v1/bulk_write.hh>

//

#include <bsoncxx/v1/array/value.hpp>
#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/detail/macros.hpp>
#include <mongocxx/v1/pipeline.hpp>
#include <mongocxx/v1/write_concern.hpp>

#include <bsoncxx/v1/types/value.hh>

#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/hint.hh>

#include <cstdint>
#include <stdexcept>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class bulk_write::impl {
   public:
    mongoc_bulk_operation_t* _bulk = {};
    bool _is_empty = true;

    ~impl() {
        libmongoc::bulk_operation_destroy(_bulk);
    }

    impl(impl&& other) = delete;
    impl& operator=(impl&& other) = delete;
    impl(impl const& other) = delete;
    impl& operator=(impl const& other) = delete;

    explicit impl(mongoc_bulk_operation_t* bulk) : _bulk{bulk} {}

    static impl const& with(bulk_write const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(bulk_write const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(bulk_write& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(bulk_write* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

bulk_write::~bulk_write() {
    delete impl::with(this);
}

bulk_write::bulk_write(bulk_write&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

bulk_write& bulk_write::operator=(bulk_write&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

bool bulk_write::empty() const {
    return impl::with(this)->_is_empty;
}

namespace {

template <typename Op>
void append_collation(scoped_bson& options, Op const& op) {
    if (auto const opt = op.collation()) {
        auto const doc = scoped_bson_view{*opt};
        options += scoped_bson{BCON_NEW("collation", BCON_DOCUMENT(doc.bson()))};
    }
}

template <typename Op>
void append_hint(scoped_bson& options, Op const& op) {
    if (auto const& opt = Op::internal::hint(op)) {
        if (auto const& doc_opt = v1::hint::internal::doc(*opt)) {
            options += scoped_bson{BCON_NEW("hint", BCON_DOCUMENT(scoped_bson_view{*doc_opt}.bson()))};
        }

        if (auto const& str_opt = v1::hint::internal::str(*opt)) {
            options += scoped_bson{BCON_NEW("hint", BCON_UTF8(str_opt->c_str()))};
        }
    }
}

template <typename Op>
void append_sort(scoped_bson& options, Op const& op) {
    if (auto const opt = op.sort()) {
        auto const doc = scoped_bson_view{*opt};
        options += scoped_bson{BCON_NEW("sort", BCON_DOCUMENT(doc.bson()))};
    }
}

template <typename Op>
void append_upsert(scoped_bson& options, Op const& op) {
    if (auto const opt = op.upsert()) {
        options += scoped_bson{BCON_NEW("upsert", BCON_BOOL(*opt))};
    }
}

template <typename Op>
void append_array_filters(scoped_bson& options, Op const& op) {
    if (auto const opt = op.array_filters()) {
        auto const doc = scoped_bson_view{*opt};
        options += scoped_bson{BCON_NEW("arrayFilters", BCON_ARRAY(doc.bson()))};
    }
}

void append_insert_one(mongoc_bulk_operation_t* bulk, v1::bulk_write::insert_one const& op) {
    bson_error_t error = {};

    if (!libmongoc::bulk_operation_insert_with_opts(bulk, scoped_bson_view{op.value}.bson(), nullptr, &error)) {
        v1::throw_exception(error);
    }
}

void append_delete_one(mongoc_bulk_operation_t* bulk, v1::bulk_write::delete_one const& op) {
    scoped_bson options;

    append_collation(options, op);
    append_hint(options, op);

    bson_error_t error = {};

    if (!libmongoc::bulk_operation_remove_one_with_opts(
            bulk, scoped_bson_view{op.filter()}.bson(), options.bson(), &error)) {
        v1::throw_exception(error);
    }
}

void append_delete_many(mongoc_bulk_operation_t* bulk, v1::bulk_write::delete_many const& op) {
    scoped_bson options;

    append_collation(options, op);
    append_hint(options, op);

    bson_error_t error = {};

    if (!libmongoc::bulk_operation_remove_many_with_opts(
            bulk, scoped_bson_view{op.filter()}.bson(), options.bson(), &error)) {
        v1::throw_exception(error);
    }
}

void append_update_one(mongoc_bulk_operation_t* bulk, v1::bulk_write::update_one const& op) {
    scoped_bson options;

    append_collation(options, op);
    append_hint(options, op);
    append_sort(options, op);
    append_upsert(options, op);
    append_array_filters(options, op);

    bson_error_t error = {};

    if (!libmongoc::bulk_operation_update_one_with_opts(
            bulk, scoped_bson_view{op.filter()}.bson(), scoped_bson_view{op.update()}.bson(), options.bson(), &error)) {
        v1::throw_exception(error);
    }
}

void append_update_many(mongoc_bulk_operation_t* bulk, v1::bulk_write::update_many const& op) {
    scoped_bson options;

    append_collation(options, op);
    append_hint(options, op);
    append_upsert(options, op);
    append_array_filters(options, op);

    bson_error_t error = {};

    if (!libmongoc::bulk_operation_update_many_with_opts(
            bulk, scoped_bson_view{op.filter()}.bson(), scoped_bson_view{op.update()}.bson(), options.bson(), &error)) {
        v1::throw_exception(error);
    }
}

void append_replace_one(mongoc_bulk_operation_t* bulk, v1::bulk_write::replace_one const& op) {
    scoped_bson options;

    append_collation(options, op);
    append_hint(options, op);
    append_sort(options, op);
    append_upsert(options, op);

    bson_error_t error = {};

    if (!libmongoc::bulk_operation_replace_one_with_opts(
            bulk,
            scoped_bson_view{op.filter()}.bson(),
            scoped_bson_view{op.replacement()}.bson(),
            options.bson(),
            &error)) {
        v1::throw_exception(error);
    }
}

} // namespace

bulk_write& bulk_write::append(single const& op) {
    auto& _bulk = impl::with(this)->_bulk;

    switch (op.type()) {
        case type::k_insert_one:
            append_insert_one(_bulk, op.get_insert_one());
            break;
        case type::k_delete_one:
            append_delete_one(_bulk, op.get_delete_one());
            break;
        case type::k_delete_many:
            append_delete_many(_bulk, op.get_delete_many());
            break;
        case type::k_update_one:
            append_update_one(_bulk, op.get_update_one());
            break;
        case type::k_update_many:
            append_update_many(_bulk, op.get_update_many());
            break;
        case type::k_replace_one:
            append_replace_one(_bulk, op.get_replace_one());
            break;
        default:
            MONGOCXX_PRIVATE_UNREACHABLE;
    }

    impl::with(this)->_is_empty = false;

    return *this;
}

bsoncxx::v1::stdx::optional<bulk_write::result> bulk_write::execute() {
    scoped_bson reply;
    bson_error_t error = {};

    if (libmongoc::bulk_operation_execute(impl::with(this)->_bulk, reply.out_ptr(), &error) == 0u) {
        v1::throw_exception(error, std::move(reply).value());
    }

    // Unacknowledged writes do not have a reply.
    if (reply.view().empty()) {
        return {};
    }

    return result::internal::make(std::move(reply).value());
}

// NOLINTNEXTLINE(cppcoreguidelines-owning-memory): owning void* for ABI stability.
bulk_write::bulk_write(void* bulk) : _impl{new impl{static_cast<mongoc_bulk_operation_t*>(bulk)}} {}

bulk_write bulk_write::internal::make(mongoc_bulk_operation_t* bulk) {
    return {bulk};
}

mongoc_bulk_operation_t* bulk_write::internal::as_mongoc(bulk_write& self) {
    return impl::with(self)._bulk;
}

bool& bulk_write::internal::is_empty(bulk_write& self) {
    return impl::with(self)._is_empty;
}

class bulk_write::update_one::impl {
   public:
    bsoncxx::v1::document::value _filter;
    bsoncxx::v1::document::value _update;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::array::value> _array_filters;
    bsoncxx::v1::stdx::optional<bool> _upsert;
    bsoncxx::v1::stdx::optional<mongocxx::v1::hint> _hint;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _sort;

    static impl const& with(update_one const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(update_one const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(update_one& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(update_one* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

bulk_write::update_one::~update_one() {
    delete impl::with(this);
}

bulk_write::update_one::update_one(update_one&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

bulk_write::update_one& bulk_write::update_one::operator=(update_one&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

bulk_write::update_one::update_one(update_one const& other) : _impl{new impl{impl::with(other)}} {}

bulk_write::update_one& bulk_write::update_one::operator=(update_one const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

bulk_write::update_one::update_one(bsoncxx::v1::document::value filter, bsoncxx::v1::document::value update)
    : _impl{new impl{}} {
    impl::with(this)->_filter = std::move(filter);
    impl::with(this)->_update = std::move(update);
}

bulk_write::update_one::update_one(bsoncxx::v1::document::value filter, v1::pipeline update)
    : update_one{std::move(filter), bsoncxx::v1::document::value{update.view_array()}} {}

bsoncxx::v1::document::view bulk_write::update_one::filter() const {
    return impl::with(this)->_filter;
}

bsoncxx::v1::document::view bulk_write::update_one::update() const {
    return impl::with(this)->_update;
}

bulk_write::update_one& bulk_write::update_one::collation(bsoncxx::v1::document::value v) {
    impl::with(this)->_collation = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> bulk_write::update_one::collation() const {
    return impl::with(this)->_collation;
}

bulk_write::update_one& bulk_write::update_one::hint(v1::hint v) {
    impl::with(this)->_hint = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> bulk_write::update_one::hint() const {
    return impl::with(this)->_hint;
}

bulk_write::update_one& bulk_write::update_one::sort(bsoncxx::v1::document::value v) {
    impl::with(this)->_sort = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> bulk_write::update_one::sort() const {
    return impl::with(this)->_sort;
}

bulk_write::update_one& bulk_write::update_one::upsert(bool v) {
    impl::with(this)->_upsert = v;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> bulk_write::update_one::upsert() const {
    return impl::with(this)->_upsert;
}

bulk_write::update_one& bulk_write::update_one::array_filters(bsoncxx::v1::array::value v) {
    impl::with(this)->_array_filters = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::array::view> bulk_write::update_one::array_filters() const {
    return impl::with(this)->_array_filters;
}

bsoncxx::v1::stdx::optional<mongocxx::v1::hint> const& bulk_write::update_one::internal::hint(
    bulk_write::update_one const& self) {
    return impl::with(self)._hint;
}

bsoncxx::v1::document::value& bulk_write::update_one::internal::filter(update_one& self) {
    return impl::with(self)._filter;
}

bsoncxx::v1::document::value& bulk_write::update_one::internal::update(update_one& self) {
    return impl::with(self)._update;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& bulk_write::update_one::internal::collation(
    update_one& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::array::value>& bulk_write::update_one::internal::array_filters(
    update_one& self) {
    return impl::with(self)._array_filters;
}

bsoncxx::v1::stdx::optional<mongocxx::v1::hint>& bulk_write::update_one::internal::hint(update_one& self) {
    return impl::with(self)._hint;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& bulk_write::update_one::internal::sort(update_one& self) {
    return impl::with(self)._sort;
}

class bulk_write::update_many::impl {
   public:
    bsoncxx::v1::document::value _filter;
    bsoncxx::v1::document::value _update;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::array::value> _array_filters;
    bsoncxx::v1::stdx::optional<bool> _upsert;
    bsoncxx::v1::stdx::optional<mongocxx::v1::hint> _hint;

    static impl const& with(update_many const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(update_many const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(update_many& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(update_many* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

bulk_write::update_many::~update_many() {
    delete impl::with(this);
}

bulk_write::update_many::update_many(update_many&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

bulk_write::update_many& bulk_write::update_many::operator=(update_many&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

bulk_write::update_many::update_many(update_many const& other) : _impl{new impl{impl::with(other)}} {}

bulk_write::update_many& bulk_write::update_many::operator=(update_many const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

bulk_write::update_many::update_many(bsoncxx::v1::document::value filter, bsoncxx::v1::document::value update)
    : _impl{new impl{}} {
    impl::with(this)->_filter = std::move(filter);
    impl::with(this)->_update = std::move(update);
}

// NOLINTEND(cppcoreguidelines-owning-memory)

bulk_write::update_many::update_many(bsoncxx::v1::document::value filter, pipeline const& update)
    : update_many{std::move(filter), bsoncxx::v1::document::value{update.view_array()}} {}

bsoncxx::v1::document::view bulk_write::update_many::filter() const {
    return impl::with(this)->_filter;
}

bsoncxx::v1::document::view bulk_write::update_many::update() const {
    return impl::with(this)->_update;
}

bulk_write::update_many& bulk_write::update_many::collation(bsoncxx::v1::document::value v) {
    impl::with(this)->_collation = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> bulk_write::update_many::collation() const {
    return impl::with(this)->_collation;
}

bulk_write::update_many& bulk_write::update_many::hint(v1::hint v) {
    impl::with(this)->_hint = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> bulk_write::update_many::hint() const {
    return impl::with(this)->_hint;
}

bulk_write::update_many& bulk_write::update_many::upsert(bool v) {
    impl::with(this)->_upsert = v;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> bulk_write::update_many::upsert() const {
    return impl::with(this)->_upsert;
}

bulk_write::update_many& bulk_write::update_many::array_filters(bsoncxx::v1::array::value v) {
    impl::with(this)->_array_filters = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::array::view> bulk_write::update_many::array_filters() const {
    return impl::with(this)->_array_filters;
}

bsoncxx::v1::stdx::optional<mongocxx::v1::hint> const& bulk_write::update_many::internal::hint(
    update_many const& self) {
    return impl::with(self)._hint;
}

bsoncxx::v1::document::value& bulk_write::update_many::internal::filter(update_many& self) {
    return impl::with(self)._filter;
}

bsoncxx::v1::document::value& bulk_write::update_many::internal::update(update_many& self) {
    return impl::with(self)._update;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& bulk_write::update_many::internal::collation(
    update_many& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::array::value>& bulk_write::update_many::internal::array_filters(
    update_many& self) {
    return impl::with(self)._array_filters;
}

bsoncxx::v1::stdx::optional<bool>& bulk_write::update_many::internal::upsert(update_many& self) {
    return impl::with(self)._upsert;
}

bsoncxx::v1::stdx::optional<mongocxx::v1::hint>& bulk_write::update_many::internal::hint(update_many& self) {
    return impl::with(self)._hint;
}

class bulk_write::replace_one::impl {
   public:
    bsoncxx::v1::document::value _filter;
    bsoncxx::v1::document::value _replacement;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<bool> _upsert;
    bsoncxx::v1::stdx::optional<v1::hint> _hint;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _sort;

    static impl const& with(replace_one const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(replace_one const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(replace_one& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(replace_one* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

bulk_write::replace_one::~replace_one() {
    delete impl::with(this);
}

bulk_write::replace_one::replace_one(replace_one&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

bulk_write::replace_one& bulk_write::replace_one::operator=(replace_one&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

bulk_write::replace_one::replace_one(replace_one const& other) : _impl{new impl{impl::with(other)}} {}

bulk_write::replace_one& bulk_write::replace_one::operator=(replace_one const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

bulk_write::replace_one::replace_one(bsoncxx::v1::document::value filter, bsoncxx::v1::document::value replacement)
    : _impl{new impl{}} {
    impl::with(this)->_filter = std::move(filter);
    impl::with(this)->_replacement = std::move(replacement);
}

// NOLINTEND(cppcoreguidelines-owning-memory)

bsoncxx::v1::document::view bulk_write::replace_one::filter() const {
    return impl::with(this)->_filter;
}

bsoncxx::v1::document::view bulk_write::replace_one::replacement() const {
    return impl::with(this)->_replacement;
}

bulk_write::replace_one& bulk_write::replace_one::collation(bsoncxx::v1::document::value v) {
    impl::with(this)->_collation = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> bulk_write::replace_one::collation() const {
    return impl::with(this)->_collation;
}

bulk_write::replace_one& bulk_write::replace_one::hint(v1::hint v) {
    impl::with(this)->_hint = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> bulk_write::replace_one::hint() const {
    return impl::with(this)->_hint;
}

bulk_write::replace_one& bulk_write::replace_one::sort(bsoncxx::v1::document::value v) {
    impl::with(this)->_sort = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> bulk_write::replace_one::sort() const {
    return impl::with(this)->_sort;
}

bulk_write::replace_one& bulk_write::replace_one::upsert(bool v) {
    impl::with(this)->_upsert = v;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> bulk_write::replace_one::upsert() const {
    return impl::with(this)->_upsert;
}

bsoncxx::v1::stdx::optional<mongocxx::v1::hint> const& bulk_write::replace_one::internal::hint(
    replace_one const& self) {
    return impl::with(self)._hint;
}

bsoncxx::v1::document::value& bulk_write::replace_one::internal::filter(replace_one& self) {
    return impl::with(self)._filter;
}

bsoncxx::v1::document::value& bulk_write::replace_one::internal::replacement(replace_one& self) {
    return impl::with(self)._replacement;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& bulk_write::replace_one::internal::collation(
    replace_one& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<v1::hint>& bulk_write::replace_one::internal::hint(replace_one& self) {
    return impl::with(self)._hint;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& bulk_write::replace_one::internal::sort(replace_one& self) {
    return impl::with(self)._sort;
}

class bulk_write::delete_one::impl {
   public:
    bsoncxx::v1::document::value _filter;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<v1::hint> _hint;

    static impl const& with(delete_one const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(delete_one const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(delete_one& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(delete_one* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

bulk_write::delete_one::~delete_one() {
    delete impl::with(this);
}

bulk_write::delete_one::delete_one(delete_one&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

bulk_write::delete_one& bulk_write::delete_one::operator=(delete_one&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

bulk_write::delete_one::delete_one(delete_one const& other) : _impl{new impl{impl::with(other)}} {}

bulk_write::delete_one& bulk_write::delete_one::operator=(delete_one const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

bulk_write::delete_one::delete_one(bsoncxx::v1::document::value filter) : _impl{new impl{}} {
    impl::with(this)->_filter = std::move(filter);
}

// NOLINTEND(cppcoreguidelines-owning-memory)

bsoncxx::v1::document::view bulk_write::delete_one::filter() const {
    return impl::with(this)->_filter;
}

bulk_write::delete_one& bulk_write::delete_one::collation(bsoncxx::v1::document::value v) {
    impl::with(this)->_collation = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> bulk_write::delete_one::collation() const {
    return impl::with(this)->_collation;
}

bulk_write::delete_one& bulk_write::delete_one::hint(v1::hint v) {
    impl::with(this)->_hint = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> bulk_write::delete_one::hint() const {
    return impl::with(this)->_hint;
}

bsoncxx::v1::stdx::optional<mongocxx::v1::hint> const& bulk_write::delete_one::internal::hint(delete_one const& self) {
    return impl::with(self)._hint;
}

bsoncxx::v1::document::value& bulk_write::delete_one::internal::filter(delete_one& self) {
    return impl::with(self)._filter;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& bulk_write::delete_one::internal::collation(
    delete_one& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<v1::hint>& bulk_write::delete_one::internal::hint(delete_one& self) {
    return impl::with(self)._hint;
}

class bulk_write::delete_many::impl {
   public:
    bsoncxx::v1::document::value _filter;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<v1::hint> _hint;

    static impl const& with(delete_many const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(delete_many const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(delete_many& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(delete_many* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

bulk_write::delete_many::~delete_many() {
    delete impl::with(this);
}

bulk_write::delete_many::delete_many(delete_many&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

bulk_write::delete_many& bulk_write::delete_many::operator=(delete_many&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

bulk_write::delete_many::delete_many(delete_many const& other) : _impl{new impl{impl::with(other)}} {}

bulk_write::delete_many& bulk_write::delete_many::operator=(delete_many const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

bulk_write::delete_many::delete_many(bsoncxx::v1::document::value filter) : _impl{new impl{}} {
    impl::with(this)->_filter = std::move(filter);
}

// NOLINTEND(cppcoreguidelines-owning-memory)

bsoncxx::v1::document::view bulk_write::delete_many::filter() const {
    return impl::with(this)->_filter;
}

bulk_write::delete_many& bulk_write::delete_many::collation(bsoncxx::v1::document::value v) {
    impl::with(this)->_collation = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> bulk_write::delete_many::collation() const {
    return impl::with(this)->_collation;
}

bulk_write::delete_many& bulk_write::delete_many::hint(v1::hint v) {
    impl::with(this)->_hint = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> bulk_write::delete_many::hint() const {
    return impl::with(this)->_hint;
}

bsoncxx::v1::stdx::optional<mongocxx::v1::hint> const& bulk_write::delete_many::internal::hint(
    delete_many const& self) {
    return impl::with(self)._hint;
}

bsoncxx::v1::document::value& bulk_write::delete_many::internal::filter(delete_many& self) {
    return impl::with(self)._filter;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& bulk_write::delete_many::internal::collation(
    delete_many& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<v1::hint>& bulk_write::delete_many::internal::hint(delete_many& self) {
    return impl::with(self)._hint;
}

class bulk_write::single::impl {
   public:
    v1::bulk_write::type _type;

    // C++17 is required for `std::variant`.
    union {
        insert_one _insert_one;
        update_one _update_one;
        update_many _update_many;
        delete_one _delete_one;
        delete_many _delete_many;
        replace_one _replace_one;
    };

    ~impl() {
        switch (_type) {
            case v1::bulk_write::type::k_insert_one:
                _insert_one.~insert_one();
                break;
            case v1::bulk_write::type::k_update_one:
                _update_one.~update_one();
                break;
            case v1::bulk_write::type::k_update_many:
                _update_many.~update_many();
                break;
            case v1::bulk_write::type::k_delete_one:
                _delete_one.~delete_one();
                break;
            case v1::bulk_write::type::k_delete_many:
                _delete_many.~delete_many();
                break;
            case v1::bulk_write::type::k_replace_one:
                _replace_one.~replace_one();
                break;
            default:
                MONGOCXX_PRIVATE_UNREACHABLE;
        }
    }

    impl(impl&& other) noexcept = delete;

    impl& operator=(impl&& other) noexcept = delete;

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init): union data members.
    impl(impl const& other) : _type{other._type} {
        switch (_type) {
            case v1::bulk_write::type::k_insert_one:
                new (&_insert_one) insert_one{other._insert_one};
                break;
            case v1::bulk_write::type::k_update_one:
                new (&_update_one) update_one{other._update_one};
                break;
            case v1::bulk_write::type::k_update_many:
                new (&_update_many) update_many{other._update_many};
                break;
            case v1::bulk_write::type::k_delete_one:
                new (&_delete_one) delete_one{other._delete_one};
                break;
            case v1::bulk_write::type::k_delete_many:
                new (&_delete_many) delete_many{other._delete_many};
                break;
            case v1::bulk_write::type::k_replace_one:
                new (&_replace_one) replace_one{other._replace_one};
                break;
            default:
                MONGOCXX_PRIVATE_UNREACHABLE;
        }
    }

    impl& operator=(impl const& other) = delete;

    impl(insert_one op) : _type{v1::bulk_write::type::k_insert_one}, _insert_one{std::move(op)} {}
    impl(update_one op) : _type{v1::bulk_write::type::k_update_one}, _update_one{std::move(op)} {}
    impl(update_many op) : _type{v1::bulk_write::type::k_update_many}, _update_many{std::move(op)} {}
    impl(delete_one op) : _type{v1::bulk_write::type::k_delete_one}, _delete_one{std::move(op)} {}
    impl(delete_many op) : _type{v1::bulk_write::type::k_delete_many}, _delete_many{std::move(op)} {}
    impl(replace_one op) : _type{v1::bulk_write::type::k_replace_one}, _replace_one{std::move(op)} {}

    static impl const& with(single const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(single const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(single& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(single* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

bulk_write::single::~single() {
    delete impl::with(this);
}

bulk_write::single::single(single&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

bulk_write::single& bulk_write::single::operator=(single&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

bulk_write::single::single(single const& other) : _impl{new impl{impl::with(other)}} {}

bulk_write::single& bulk_write::single::operator=(single const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

bulk_write::single::single(bulk_write::insert_one op) : _impl{new impl{std::move(op)}} {}
bulk_write::single::single(bulk_write::update_one op) : _impl{new impl{std::move(op)}} {}
bulk_write::single::single(bulk_write::update_many op) : _impl{new impl{std::move(op)}} {}
bulk_write::single::single(bulk_write::delete_one op) : _impl{new impl{std::move(op)}} {}
bulk_write::single::single(bulk_write::delete_many op) : _impl{new impl{std::move(op)}} {}
bulk_write::single::single(bulk_write::replace_one op) : _impl{new impl{std::move(op)}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

v1::bulk_write::type bulk_write::single::type() const {
    return impl::with(this)->_type;
}

bulk_write::insert_one const& bulk_write::single::get_insert_one() const& {
    return impl::with(this)->_insert_one;
}

bulk_write::insert_one&& bulk_write::single::get_insert_one() && {
    return std::move(impl::with(this)->_insert_one);
}

bulk_write::update_one const& bulk_write::single::get_update_one() const& {
    return impl::with(this)->_update_one;
}

bulk_write::update_one&& bulk_write::single::get_update_one() && {
    return std::move(impl::with(this)->_update_one);
}

bulk_write::update_many const& bulk_write::single::get_update_many() const& {
    return impl::with(this)->_update_many;
}

bulk_write::update_many&& bulk_write::single::get_update_many() && {
    return std::move(impl::with(this)->_update_many);
}

bulk_write::delete_one const& bulk_write::single::get_delete_one() const& {
    return impl::with(this)->_delete_one;
}

bulk_write::delete_one&& bulk_write::single::get_delete_one() && {
    return std::move(impl::with(this)->_delete_one);
}

bulk_write::delete_many const& bulk_write::single::get_delete_many() const& {
    return impl::with(this)->_delete_many;
}

bulk_write::delete_many&& bulk_write::single::get_delete_many() && {
    return std::move(impl::with(this)->_delete_many);
}

bulk_write::replace_one const& bulk_write::single::get_replace_one() const& {
    return impl::with(this)->_replace_one;
}

bulk_write::replace_one&& bulk_write::single::get_replace_one() && {
    return std::move(impl::with(this)->_replace_one);
}

bulk_write::insert_one& bulk_write::single::internal::get_insert_one(single& self) {
    return impl::with(self)._insert_one;
}

bulk_write::update_one& bulk_write::single::internal::get_update_one(single& self) {
    return impl::with(self)._update_one;
}

bulk_write::update_many& bulk_write::single::internal::get_update_many(single& self) {
    return impl::with(self)._update_many;
}

bulk_write::delete_one& bulk_write::single::internal::get_delete_one(single& self) {
    return impl::with(self)._delete_one;
}

bulk_write::delete_many& bulk_write::single::internal::get_delete_many(single& self) {
    return impl::with(self)._delete_many;
}

bulk_write::replace_one& bulk_write::single::internal::get_replace_one(single& self) {
    return impl::with(self)._replace_one;
}

class bulk_write::options::impl {
   public:
    bsoncxx::v1::stdx::optional<bool> _bypass_document_validation;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _comment;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _let;
    bool _ordered = true;
    bsoncxx::v1::stdx::optional<v1::write_concern> _write_concern;

    static impl const& with(options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(options const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

bulk_write::options::~options() {
    delete impl::with(this);
}

bulk_write::options::options(options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

bulk_write::options& bulk_write::options::operator=(options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

bulk_write::options::options(options const& other) : _impl{new impl{impl::with(other)}} {}

bulk_write::options& bulk_write::options::operator=(options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

bulk_write::options::options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

bulk_write::options& bulk_write::options::bypass_document_validation(bool bypass_document_validation) {
    impl::with(this)->_bypass_document_validation = bypass_document_validation;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> bulk_write::options::bypass_document_validation() const {
    return impl::with(this)->_bypass_document_validation;
}

bulk_write::options& bulk_write::options::comment(bsoncxx::v1::types::value comment) {
    impl::with(this)->_comment = std::move(comment);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> bulk_write::options::comment() const {
    return impl::with(this)->_comment;
}

bulk_write::options& bulk_write::options::let(bsoncxx::v1::document::value let) {
    impl::with(this)->_let = std::move(let);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> bulk_write::options::let() const {
    return impl::with(this)->_let;
}

bulk_write::options& bulk_write::options::ordered(bool ordered) {
    impl::with(this)->_ordered = ordered;
    return *this;
}

bool bulk_write::options::ordered() const {
    return impl::with(this)->_ordered;
}

bulk_write::options& bulk_write::options::write_concern(v1::write_concern wc) {
    impl::with(this)->_write_concern = std::move(wc);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::write_concern> bulk_write::options::write_concern() const {
    return impl::with(this)->_write_concern;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& bulk_write::options::internal::comment(options& self) {
    return impl::with(self)._comment;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& bulk_write::options::internal::let(options& self) {
    return impl::with(self)._let;
}

bsoncxx::v1::stdx::optional<v1::write_concern>& bulk_write::options::internal::write_concern(options& self) {
    return impl::with(self)._write_concern;
}

void bulk_write::options::internal::append_to(options const& self, scoped_bson& doc) {
    if (!impl::with(self)._ordered) {
        // ordered is true by default. Only append it if set to false.
        doc += scoped_bson{BCON_NEW("ordered", BCON_BOOL(false))};
    }

    if (auto const& opt = impl::with(self)._write_concern) {
        auto const v = opt->to_document();
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson_view{v}.bson()))};
    }

    if (auto const& opt = impl::with(self)._let) {
        auto const v = opt->view();
        doc += scoped_bson{BCON_NEW("let", BCON_DOCUMENT(scoped_bson_view{v}.bson()))};
    }

    if (auto const& opt = impl::with(self)._comment) {
        scoped_bson v;

        if (!BSON_APPEND_VALUE(v.inout_ptr(), "comment", &bsoncxx::v1::types::value::internal::get_bson_value(*opt))) {
            throw std::logic_error{"mongocxx::v1::bulk_write::options::internal::append_to: BSON_APPEND_VALUE failed"};
        }

        doc += v;
    }
}

class bulk_write::result::impl {
   public:
    bsoncxx::v1::document::value _reply;

    explicit impl(bsoncxx::v1::document::value reply) : _reply{std::move(reply)} {}

    static impl const& with(result const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(result const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(result& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(result* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

bulk_write::result::~result() {
    delete impl::with(this);
    _impl = nullptr; // scan-build: warning: Use of memory after it is freed [cplusplus.NewDelete]
}

bulk_write::result::result(result&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

bulk_write::result& bulk_write::result::operator=(result&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

bulk_write::result::result(result const& other) : _impl{new impl{impl::with(other)}} {}

bulk_write::result& bulk_write::result::operator=(result const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

std::int64_t bulk_write::result::inserted_count() const {
    return impl::with(this)->_reply["nInserted"].get_int32().value;
}

std::int64_t bulk_write::result::matched_count() const {
    return impl::with(this)->_reply["nMatched"].get_int32().value;
}

std::int64_t bulk_write::result::modified_count() const {
    return impl::with(this)->_reply["nModified"].get_int32().value;
}

std::int64_t bulk_write::result::deleted_count() const {
    return impl::with(this)->_reply["nRemoved"].get_int32().value;
}

std::int64_t bulk_write::result::upserted_count() const {
    return impl::with(this)->_reply["nUpserted"].get_int32().value;
}

bulk_write::result::id_map bulk_write::result::upserted_ids() const {
    auto const ids = impl::with(this)->_reply["upserted"];

    if (!ids || ids.type_id() != bsoncxx::v1::types::id::k_array) {
        return {};
    }

    id_map ret;

    for (auto const e : ids.get_array().value) {
        auto const id = e["_id"];
        auto const index = e["index"];

        if (!id || !index || index.type_id() != bsoncxx::v1::types::id::k_int32) {
            continue;
        }

        ret.emplace(index.get_int32().value, id.type_view());
    }

    return ret;
}

bool operator==(bulk_write::result const& lhs, bulk_write::result const& rhs) {
    return bulk_write::result::impl::with(lhs)._reply == bulk_write::result::impl::with(rhs)._reply;
}

bulk_write::result::result(void* impl) : _impl{impl} {}

bulk_write::result bulk_write::result::internal::make(bsoncxx::v1::document::value reply) {
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory): owning void* for ABI stability.
    return {new result::impl{std::move(reply)}};
}

bsoncxx::v1::document::view bulk_write::result::internal::reply(result const& self) {
    return impl::with(self)._reply;
}

} // namespace v1
} // namespace mongocxx
