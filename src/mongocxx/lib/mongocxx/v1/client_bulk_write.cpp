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

#include <mongocxx/v1/client_bulk_write.hh>

//

#include <bsoncxx/v1/array/value.hpp>
#include <bsoncxx/v1/array/view.hpp>
#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/hint.hpp>
#include <mongocxx/v1/pipeline.hpp>

#include <bsoncxx/v1/types/value.hh>

#include <mongocxx/v1/client_session.hh>
#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/write_concern.hh>

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

class client_bulk_write::impl {
   public:
    mongoc_bulkwrite_t* _bulk;

    ~impl() {
        libmongoc::bulkwrite_destroy(_bulk);
    }

    impl(impl&&) noexcept = delete;
    impl& operator=(impl&&) noexcept = delete;
    impl(impl const&) = delete;
    impl& operator=(impl const&) = delete;

    explicit impl(mongoc_bulkwrite_t* bulk) : _bulk{bulk} {}

    static impl const& with(client_bulk_write const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl& with(client_bulk_write& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(client_bulk_write* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

client_bulk_write::~client_bulk_write() {
    delete impl::with(this);
}

client_bulk_write::client_bulk_write(client_bulk_write&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

client_bulk_write& client_bulk_write::operator=(client_bulk_write&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client_bulk_write::client_bulk_write(void* bulk) : _impl{new impl{static_cast<mongoc_bulkwrite_t*>(bulk)}} {}

client_bulk_write client_bulk_write::internal::make(mongoc_bulkwrite_t* bulk) {
    return client_bulk_write{bulk};
}

mongoc_bulkwrite_t* client_bulk_write::internal::as_mongoc(client_bulk_write& self) {
    return impl::with(self)._bulk;
}

void client_bulk_write::internal::set_session(client_bulk_write& self, v1::client_session& session) {
    libmongoc::bulkwrite_set_session(as_mongoc(self), v1::client_session::internal::as_mongoc(session));
}

class client_bulk_write::options::impl {
   public:
    bsoncxx::v1::stdx::optional<bool> _bypass_document_validation;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _comment;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _let;
    bsoncxx::v1::stdx::optional<bool> _ordered;
    bsoncxx::v1::stdx::optional<bool> _verbose_results;
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

client_bulk_write::options::~options() {
    delete impl::with(this);
}

client_bulk_write::options::options(options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

client_bulk_write::options& client_bulk_write::options::operator=(options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client_bulk_write::options::options(options const& other) : _impl{new impl{impl::with(other)}} {}

client_bulk_write::options& client_bulk_write::options::operator=(options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

client_bulk_write::options::options() : _impl{new impl{}} {}

client_bulk_write::options& client_bulk_write::options::bypass_document_validation(bool bypass_document_validation) {
    impl::with(this)->_bypass_document_validation = bypass_document_validation;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> client_bulk_write::options::bypass_document_validation() const {
    return impl::with(this)->_bypass_document_validation;
}

client_bulk_write::options& client_bulk_write::options::comment(bsoncxx::v1::types::value comment) {
    impl::with(this)->_comment = std::move(comment);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> client_bulk_write::options::comment() const {
    return impl::with(this)->_comment;
}

client_bulk_write::options& client_bulk_write::options::let(bsoncxx::v1::document::value let) {
    impl::with(this)->_let = std::move(let);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::options::let() const {
    return impl::with(this)->_let;
}

client_bulk_write::options& client_bulk_write::options::ordered(bool ordered) {
    impl::with(this)->_ordered = ordered;
    return *this;
}

bool client_bulk_write::options::ordered() const {
    return impl::with(this)->_ordered.value_or(true);
}

client_bulk_write::options& client_bulk_write::options::verbose_results(bool verbose_results) {
    impl::with(this)->_verbose_results = verbose_results;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> client_bulk_write::options::verbose_results() const {
    return impl::with(this)->_verbose_results;
}

client_bulk_write::options& client_bulk_write::options::write_concern(v1::write_concern wc) {
    impl::with(this)->_write_concern = std::move(wc);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::write_concern> client_bulk_write::options::write_concern() const {
    return impl::with(this)->_write_concern;
}

bsoncxx::v1::stdx::optional<bool> const& client_bulk_write::options::internal::bypass_document_validation(
    options const& self) {
    return impl::with(self)._bypass_document_validation;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> const& client_bulk_write::options::internal::comment(
    options const& self) {
    return impl::with(self)._comment;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& client_bulk_write::options::internal::let(
    options const& self) {
    return impl::with(self)._let;
}

bool client_bulk_write::options::internal::ordered(options const& self) {
    return impl::with(self)._ordered.value_or(true);
}

bsoncxx::v1::stdx::optional<bool> const& client_bulk_write::options::internal::verbose_results(options const& self) {
    return impl::with(self)._verbose_results;
}

bsoncxx::v1::stdx::optional<v1::write_concern> const& client_bulk_write::options::internal::write_concern(
    options const& self) {
    return impl::with(self)._write_concern;
}

class client_bulk_write::result::impl {
   public:
    std::int64_t _inserted_count = 0;
    std::int64_t _upserted_count = 0;
    std::int64_t _matched_count = 0;
    std::int64_t _modified_count = 0;
    std::int64_t _deleted_count = 0;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _insert_results;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _update_results;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _delete_results;

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

client_bulk_write::result::~result() {
    delete impl::with(this);
    _impl = nullptr; // scan-build: warning: Use of memory after it is freed [cplusplus.NewDelete]
}

client_bulk_write::result::result(result&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

client_bulk_write::result& client_bulk_write::result::operator=(result&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client_bulk_write::result::result(result const& other) : _impl{new impl{impl::with(other)}} {}

client_bulk_write::result& client_bulk_write::result::operator=(result const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

std::int64_t client_bulk_write::result::inserted_count() const {
    return impl::with(this)->_inserted_count;
}

std::int64_t client_bulk_write::result::upserted_count() const {
    return impl::with(this)->_upserted_count;
}

std::int64_t client_bulk_write::result::matched_count() const {
    return impl::with(this)->_matched_count;
}

std::int64_t client_bulk_write::result::modified_count() const {
    return impl::with(this)->_modified_count;
}

std::int64_t client_bulk_write::result::deleted_count() const {
    return impl::with(this)->_deleted_count;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::result::insert_results() const {
    return impl::with(this)->_insert_results;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::result::update_results() const {
    return impl::with(this)->_update_results;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::result::delete_results() const {
    return impl::with(this)->_delete_results;
}

client_bulk_write::result::result(void* impl_ptr) : _impl{impl_ptr} {}

client_bulk_write::result client_bulk_write::result::internal::make(mongoc_bulkwriteresult_t* res) {
    struct deleter {
        void operator()(mongoc_bulkwriteresult_t* ptr) const noexcept {
            libmongoc::bulkwriteresult_destroy(ptr);
        }
    };

    auto const guard = std::unique_ptr<mongoc_bulkwriteresult_t, deleter>{res};

    auto p = std::unique_ptr<impl>{new impl{}};

    p->_inserted_count = libmongoc::bulkwriteresult_insertedcount(res);
    p->_upserted_count = libmongoc::bulkwriteresult_upsertedcount(res);
    p->_matched_count = libmongoc::bulkwriteresult_matchedcount(res);
    p->_modified_count = libmongoc::bulkwriteresult_modifiedcount(res);
    p->_deleted_count = libmongoc::bulkwriteresult_deletedcount(res);

    if (auto const* const doc = libmongoc::bulkwriteresult_insertresults(res)) {
        p->_insert_results = scoped_bson_view{doc}.value();
    }

    if (auto const* const doc = libmongoc::bulkwriteresult_updateresults(res)) {
        p->_update_results = scoped_bson_view{doc}.value();
    }

    if (auto const* const doc = libmongoc::bulkwriteresult_deleteresults(res)) {
        p->_delete_results = scoped_bson_view{doc}.value();
    }

    return result{static_cast<void*>(p.release())};
}

client_bulk_write::result client_bulk_write::result::internal::make() {
    return result{static_cast<void*>(new impl{})};
}

std::int64_t& client_bulk_write::result::internal::inserted_count(result& self) {
    return impl::with(self)._inserted_count;
}

std::int64_t& client_bulk_write::result::internal::upserted_count(result& self) {
    return impl::with(self)._upserted_count;
}

std::int64_t& client_bulk_write::result::internal::matched_count(result& self) {
    return impl::with(self)._matched_count;
}

std::int64_t& client_bulk_write::result::internal::modified_count(result& self) {
    return impl::with(self)._modified_count;
}

std::int64_t& client_bulk_write::result::internal::deleted_count(result& self) {
    return impl::with(self)._deleted_count;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& client_bulk_write::result::internal::insert_results(
    result& self) {
    return impl::with(self)._insert_results;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& client_bulk_write::result::internal::update_results(
    result& self) {
    return impl::with(self)._update_results;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& client_bulk_write::result::internal::delete_results(
    result& self) {
    return impl::with(self)._delete_results;
}

class client_bulk_write::exception::impl {
   public:
    bsoncxx::v1::document::value _write_errors;
    bsoncxx::v1::document::value _write_concern_errors;
    bsoncxx::v1::document::value _error_reply;
    bsoncxx::v1::stdx::optional<result> _partial_result;
};

void client_bulk_write::exception::key_function() const {}

bsoncxx::v1::document::view client_bulk_write::exception::write_errors() const {
    return _impl->_write_errors;
}

bsoncxx::v1::array::view client_bulk_write::exception::write_concern_errors() const {
    return bsoncxx::v1::array::view{_impl->_write_concern_errors.view().data()};
}

bsoncxx::v1::document::view client_bulk_write::exception::error_reply() const {
    return _impl->_error_reply;
}

bsoncxx::v1::stdx::optional<client_bulk_write::result> client_bulk_write::exception::partial_result() const {
    return _impl->_partial_result;
}

client_bulk_write::exception::exception(int code, char const* message, std::unique_ptr<impl> impl)
    : v1::exception{v1::exception::internal::make(code, std::generic_category(), message)}, _impl{std::move(impl)} {}

client_bulk_write::exception client_bulk_write::exception::internal::make(
    mongoc_bulkwriteexception_t* exc,
    bsoncxx::v1::stdx::optional<result> partial_result) {
    struct deleter {
        void operator()(mongoc_bulkwriteexception_t* ptr) const noexcept {
            libmongoc::bulkwriteexception_destroy(ptr);
        }
    };

    auto const guard = std::unique_ptr<mongoc_bulkwriteexception_t, deleter>{exc};

    auto p = std::unique_ptr<impl>{new impl{}};

    if (auto const* const doc = libmongoc::bulkwriteexception_writeerrors(exc)) {
        p->_write_errors = scoped_bson_view{doc}.value();
    }

    if (auto const* const arr = libmongoc::bulkwriteexception_writeconcernerrors(exc)) {
        p->_write_concern_errors = scoped_bson_view{arr}.value();
    }

    if (auto const* const doc = libmongoc::bulkwriteexception_errorreply(exc)) {
        p->_error_reply = scoped_bson_view{doc}.value();
    }

    p->_partial_result = std::move(partial_result);

    bson_error_t error = {};
    auto const has_error = libmongoc::bulkwriteexception_error(exc, &error);

    auto const code = has_error ? static_cast<int>(error.code) : 0;

    auto const* const message = has_error ? error.message : "";

    return exception{code, message, std::move(p)};
}

client_bulk_write::exception client_bulk_write::exception::internal::make() {
    return exception{0, "", std::unique_ptr<impl>{new impl{}}};
}

bsoncxx::v1::document::value& client_bulk_write::exception::internal::write_errors(exception& self) {
    return self._impl->_write_errors;
}

bsoncxx::v1::document::value& client_bulk_write::exception::internal::write_concern_errors(exception& self) {
    return self._impl->_write_concern_errors;
}

bsoncxx::v1::document::value& client_bulk_write::exception::internal::error_reply(exception& self) {
    return self._impl->_error_reply;
}

class client_bulk_write::insert_one_options::impl {
   public:
    static impl* with(insert_one_options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

client_bulk_write::insert_one_options::~insert_one_options() {
    delete impl::with(this);
}

client_bulk_write::insert_one_options::insert_one_options(insert_one_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

client_bulk_write::insert_one_options& client_bulk_write::insert_one_options::operator=(
    insert_one_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client_bulk_write::insert_one_options::insert_one_options(insert_one_options const& /*other*/) : _impl{new impl{}} {}

client_bulk_write::insert_one_options& client_bulk_write::insert_one_options::operator=(
    insert_one_options const& other) {
    if (this != &other) {
        auto* const tmp = new impl{};
        delete impl::with(_impl);
        _impl = tmp;
    }

    return *this;
}

client_bulk_write::insert_one_options::insert_one_options() : _impl{new impl{}} {}

class client_bulk_write::update_one_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bsoncxx::v1::array::value> _array_filters;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<v1::hint> _hint;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _sort;
    bsoncxx::v1::stdx::optional<bool> _upsert;

    static impl const& with(update_one_options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl& with(update_one_options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(update_one_options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

client_bulk_write::update_one_options::~update_one_options() {
    delete impl::with(this);
}

client_bulk_write::update_one_options::update_one_options(update_one_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

client_bulk_write::update_one_options& client_bulk_write::update_one_options::operator=(
    update_one_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client_bulk_write::update_one_options::update_one_options(update_one_options const& other)
    : _impl{new impl{impl::with(other)}} {}

client_bulk_write::update_one_options& client_bulk_write::update_one_options::operator=(
    update_one_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

client_bulk_write::update_one_options::update_one_options() : _impl{new impl{}} {}

client_bulk_write::update_one_options& client_bulk_write::update_one_options::array_filters(
    bsoncxx::v1::array::value array_filters) {
    impl::with(*this)._array_filters = std::move(array_filters);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::array::view> client_bulk_write::update_one_options::array_filters() const {
    return impl::with(*this)._array_filters;
}

client_bulk_write::update_one_options& client_bulk_write::update_one_options::collation(
    bsoncxx::v1::document::value collation) {
    impl::with(*this)._collation = std::move(collation);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::update_one_options::collation() const {
    return impl::with(*this)._collation;
}

client_bulk_write::update_one_options& client_bulk_write::update_one_options::hint(v1::hint hint) {
    impl::with(*this)._hint = std::move(hint);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> client_bulk_write::update_one_options::hint() const {
    return impl::with(*this)._hint;
}

client_bulk_write::update_one_options& client_bulk_write::update_one_options::sort(bsoncxx::v1::document::value sort) {
    impl::with(*this)._sort = std::move(sort);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::update_one_options::sort() const {
    return impl::with(*this)._sort;
}

client_bulk_write::update_one_options& client_bulk_write::update_one_options::upsert(bool upsert) {
    impl::with(*this)._upsert = upsert;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> client_bulk_write::update_one_options::upsert() const {
    return impl::with(*this)._upsert;
}

class client_bulk_write::update_many_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bsoncxx::v1::array::value> _array_filters;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<v1::hint> _hint;
    bsoncxx::v1::stdx::optional<bool> _upsert;

    static impl const& with(update_many_options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl& with(update_many_options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(update_many_options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

client_bulk_write::update_many_options::~update_many_options() {
    delete impl::with(this);
}

client_bulk_write::update_many_options::update_many_options(update_many_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

client_bulk_write::update_many_options& client_bulk_write::update_many_options::operator=(
    update_many_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client_bulk_write::update_many_options::update_many_options(update_many_options const& other)
    : _impl{new impl{impl::with(other)}} {}

client_bulk_write::update_many_options& client_bulk_write::update_many_options::operator=(
    update_many_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

client_bulk_write::update_many_options::update_many_options() : _impl{new impl{}} {}

client_bulk_write::update_many_options& client_bulk_write::update_many_options::array_filters(
    bsoncxx::v1::array::value array_filters) {
    impl::with(*this)._array_filters = std::move(array_filters);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::array::view> client_bulk_write::update_many_options::array_filters() const {
    return impl::with(*this)._array_filters;
}

client_bulk_write::update_many_options& client_bulk_write::update_many_options::collation(
    bsoncxx::v1::document::value collation) {
    impl::with(*this)._collation = std::move(collation);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::update_many_options::collation() const {
    return impl::with(*this)._collation;
}

client_bulk_write::update_many_options& client_bulk_write::update_many_options::hint(v1::hint hint) {
    impl::with(*this)._hint = std::move(hint);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> client_bulk_write::update_many_options::hint() const {
    return impl::with(*this)._hint;
}

client_bulk_write::update_many_options& client_bulk_write::update_many_options::upsert(bool upsert) {
    impl::with(*this)._upsert = upsert;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> client_bulk_write::update_many_options::upsert() const {
    return impl::with(*this)._upsert;
}

class client_bulk_write::replace_one_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<v1::hint> _hint;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _sort;
    bsoncxx::v1::stdx::optional<bool> _upsert;

    static impl const& with(replace_one_options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl& with(replace_one_options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(replace_one_options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

client_bulk_write::replace_one_options::~replace_one_options() {
    delete impl::with(this);
}

client_bulk_write::replace_one_options::replace_one_options(replace_one_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

client_bulk_write::replace_one_options& client_bulk_write::replace_one_options::operator=(
    replace_one_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client_bulk_write::replace_one_options::replace_one_options(replace_one_options const& other)
    : _impl{new impl{impl::with(other)}} {}

client_bulk_write::replace_one_options& client_bulk_write::replace_one_options::operator=(
    replace_one_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

client_bulk_write::replace_one_options::replace_one_options() : _impl{new impl{}} {}

client_bulk_write::replace_one_options& client_bulk_write::replace_one_options::collation(
    bsoncxx::v1::document::value collation) {
    impl::with(*this)._collation = std::move(collation);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::replace_one_options::collation() const {
    return impl::with(*this)._collation;
}

client_bulk_write::replace_one_options& client_bulk_write::replace_one_options::hint(v1::hint hint) {
    impl::with(*this)._hint = std::move(hint);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> client_bulk_write::replace_one_options::hint() const {
    return impl::with(*this)._hint;
}

client_bulk_write::replace_one_options& client_bulk_write::replace_one_options::sort(
    bsoncxx::v1::document::value sort) {
    impl::with(*this)._sort = std::move(sort);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::replace_one_options::sort() const {
    return impl::with(*this)._sort;
}

client_bulk_write::replace_one_options& client_bulk_write::replace_one_options::upsert(bool upsert) {
    impl::with(*this)._upsert = upsert;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> client_bulk_write::replace_one_options::upsert() const {
    return impl::with(*this)._upsert;
}

class client_bulk_write::delete_one_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<v1::hint> _hint;

    static impl const& with(delete_one_options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl& with(delete_one_options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(delete_one_options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

client_bulk_write::delete_one_options::~delete_one_options() {
    delete impl::with(this);
}

client_bulk_write::delete_one_options::delete_one_options(delete_one_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

client_bulk_write::delete_one_options& client_bulk_write::delete_one_options::operator=(
    delete_one_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client_bulk_write::delete_one_options::delete_one_options(delete_one_options const& other)
    : _impl{new impl{impl::with(other)}} {}

client_bulk_write::delete_one_options& client_bulk_write::delete_one_options::operator=(
    delete_one_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

client_bulk_write::delete_one_options::delete_one_options() : _impl{new impl{}} {}

client_bulk_write::delete_one_options& client_bulk_write::delete_one_options::collation(
    bsoncxx::v1::document::value collation) {
    impl::with(*this)._collation = std::move(collation);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::delete_one_options::collation() const {
    return impl::with(*this)._collation;
}

client_bulk_write::delete_one_options& client_bulk_write::delete_one_options::hint(v1::hint hint) {
    impl::with(*this)._hint = std::move(hint);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> client_bulk_write::delete_one_options::hint() const {
    return impl::with(*this)._hint;
}

class client_bulk_write::delete_many_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<v1::hint> _hint;

    static impl const& with(delete_many_options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl& with(delete_many_options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(delete_many_options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

client_bulk_write::delete_many_options::~delete_many_options() {
    delete impl::with(this);
}

client_bulk_write::delete_many_options::delete_many_options(delete_many_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

client_bulk_write::delete_many_options& client_bulk_write::delete_many_options::operator=(
    delete_many_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client_bulk_write::delete_many_options::delete_many_options(delete_many_options const& other)
    : _impl{new impl{impl::with(other)}} {}

client_bulk_write::delete_many_options& client_bulk_write::delete_many_options::operator=(
    delete_many_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

client_bulk_write::delete_many_options::delete_many_options() : _impl{new impl{}} {}

client_bulk_write::delete_many_options& client_bulk_write::delete_many_options::collation(
    bsoncxx::v1::document::value collation) {
    impl::with(*this)._collation = std::move(collation);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::delete_many_options::collation() const {
    return impl::with(*this)._collation;
}

client_bulk_write::delete_many_options& client_bulk_write::delete_many_options::hint(v1::hint hint) {
    impl::with(*this)._hint = std::move(hint);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> client_bulk_write::delete_many_options::hint() const {
    return impl::with(*this)._hint;
}

namespace {

struct update_one_options_c {
    mongoc_bulkwrite_updateoneopts_t* _opts = nullptr;

    ~update_one_options_c() {
        libmongoc::bulkwrite_updateoneopts_destroy(_opts);
    }

    update_one_options_c(update_one_options_c&&) = delete;
    update_one_options_c& operator=(update_one_options_c&&) = delete;
    update_one_options_c(update_one_options_c const&) = delete;
    update_one_options_c& operator=(update_one_options_c const&) = delete;

    explicit update_one_options_c(client_bulk_write::update_one_options const& opts) {
        if (!(opts.array_filters() || opts.collation() || opts.hint() || opts.sort() || opts.upsert())) {
            return;
        }

        _opts = libmongoc::bulkwrite_updateoneopts_new();

        if (auto const v = opts.array_filters()) {
            libmongoc::bulkwrite_updateoneopts_set_arrayfilters(_opts, scoped_bson_view{*v});
        }

        if (auto const v = opts.collation()) {
            libmongoc::bulkwrite_updateoneopts_set_collation(_opts, scoped_bson_view{*v});
        }

        if (auto const h = opts.hint()) {
            auto const hv = bsoncxx::v1::types::value{h->to_value()};
            libmongoc::bulkwrite_updateoneopts_set_hint(
                _opts, &bsoncxx::v1::types::value::internal::get_bson_value(hv));
        }

        if (auto const v = opts.sort()) {
            libmongoc::bulkwrite_updateoneopts_set_sort(_opts, scoped_bson_view{*v});
        }

        if (auto const v = opts.upsert()) {
            libmongoc::bulkwrite_updateoneopts_set_upsert(_opts, *v);
        }
    }

    mongoc_bulkwrite_updateoneopts_t* get() const {
        return _opts;
    }
};

struct update_many_options_c {
    mongoc_bulkwrite_updatemanyopts_t* _opts = nullptr;

    ~update_many_options_c() {
        libmongoc::bulkwrite_updatemanyopts_destroy(_opts);
    }

    update_many_options_c(update_many_options_c&&) = delete;
    update_many_options_c& operator=(update_many_options_c&&) = delete;
    update_many_options_c(update_many_options_c const&) = delete;
    update_many_options_c& operator=(update_many_options_c const&) = delete;

    explicit update_many_options_c(client_bulk_write::update_many_options const& opts) {
        if (!(opts.array_filters() || opts.collation() || opts.hint() || opts.upsert())) {
            return;
        }

        _opts = libmongoc::bulkwrite_updatemanyopts_new();

        if (auto const v = opts.array_filters()) {
            libmongoc::bulkwrite_updatemanyopts_set_arrayfilters(_opts, scoped_bson_view{*v});
        }

        if (auto const v = opts.collation()) {
            libmongoc::bulkwrite_updatemanyopts_set_collation(_opts, scoped_bson_view{*v});
        }

        if (auto const h = opts.hint()) {
            auto const hv = bsoncxx::v1::types::value{h->to_value()};
            libmongoc::bulkwrite_updatemanyopts_set_hint(
                _opts, &bsoncxx::v1::types::value::internal::get_bson_value(hv));
        }

        if (auto const v = opts.upsert()) {
            libmongoc::bulkwrite_updatemanyopts_set_upsert(_opts, *v);
        }
    }

    mongoc_bulkwrite_updatemanyopts_t* get() const {
        return _opts;
    }
};

struct replace_one_options_c {
    mongoc_bulkwrite_replaceoneopts_t* _opts = nullptr;

    ~replace_one_options_c() {
        libmongoc::bulkwrite_replaceoneopts_destroy(_opts);
    }

    replace_one_options_c(replace_one_options_c&&) = delete;
    replace_one_options_c& operator=(replace_one_options_c&&) = delete;
    replace_one_options_c(replace_one_options_c const&) = delete;
    replace_one_options_c& operator=(replace_one_options_c const&) = delete;

    explicit replace_one_options_c(client_bulk_write::replace_one_options const& opts) {
        if (!(opts.collation() || opts.hint() || opts.sort() || opts.upsert())) {
            return;
        }

        _opts = libmongoc::bulkwrite_replaceoneopts_new();

        if (auto const v = opts.collation()) {
            libmongoc::bulkwrite_replaceoneopts_set_collation(_opts, scoped_bson_view{*v});
        }

        if (auto const h = opts.hint()) {
            auto const hv = bsoncxx::v1::types::value{h->to_value()};
            libmongoc::bulkwrite_replaceoneopts_set_hint(
                _opts, &bsoncxx::v1::types::value::internal::get_bson_value(hv));
        }

        if (auto const v = opts.sort()) {
            libmongoc::bulkwrite_replaceoneopts_set_sort(_opts, scoped_bson_view{*v});
        }

        if (auto const v = opts.upsert()) {
            libmongoc::bulkwrite_replaceoneopts_set_upsert(_opts, *v);
        }
    }

    mongoc_bulkwrite_replaceoneopts_t* get() const {
        return _opts;
    }
};

struct delete_one_options_c {
    mongoc_bulkwrite_deleteoneopts_t* _opts = nullptr;

    ~delete_one_options_c() {
        libmongoc::bulkwrite_deleteoneopts_destroy(_opts);
    }

    delete_one_options_c(delete_one_options_c&&) = delete;
    delete_one_options_c& operator=(delete_one_options_c&&) = delete;
    delete_one_options_c(delete_one_options_c const&) = delete;
    delete_one_options_c& operator=(delete_one_options_c const&) = delete;

    explicit delete_one_options_c(client_bulk_write::delete_one_options const& opts) {
        if (!(opts.collation() || opts.hint())) {
            return;
        }

        _opts = libmongoc::bulkwrite_deleteoneopts_new();

        if (auto const v = opts.collation()) {
            libmongoc::bulkwrite_deleteoneopts_set_collation(_opts, scoped_bson_view{*v});
        }

        if (auto const h = opts.hint()) {
            auto const hv = bsoncxx::v1::types::value{h->to_value()};
            libmongoc::bulkwrite_deleteoneopts_set_hint(
                _opts, &bsoncxx::v1::types::value::internal::get_bson_value(hv));
        }
    }

    mongoc_bulkwrite_deleteoneopts_t* get() const {
        return _opts;
    }
};

struct delete_many_options_c {
    mongoc_bulkwrite_deletemanyopts_t* _opts = nullptr;

    ~delete_many_options_c() {
        libmongoc::bulkwrite_deletemanyopts_destroy(_opts);
    }

    delete_many_options_c(delete_many_options_c&&) = delete;
    delete_many_options_c& operator=(delete_many_options_c&&) = delete;
    delete_many_options_c(delete_many_options_c const&) = delete;
    delete_many_options_c& operator=(delete_many_options_c const&) = delete;

    explicit delete_many_options_c(client_bulk_write::delete_many_options const& opts) {
        if (!(opts.collation() || opts.hint())) {
            return;
        }

        _opts = libmongoc::bulkwrite_deletemanyopts_new();

        if (auto const v = opts.collation()) {
            libmongoc::bulkwrite_deletemanyopts_set_collation(_opts, scoped_bson_view{*v});
        }

        if (auto const h = opts.hint()) {
            auto const hv = bsoncxx::v1::types::value{h->to_value()};
            libmongoc::bulkwrite_deletemanyopts_set_hint(
                _opts, &bsoncxx::v1::types::value::internal::get_bson_value(hv));
        }
    }

    mongoc_bulkwrite_deletemanyopts_t* get() const {
        return _opts;
    }
};

struct bulk_write_options_c {
    mongoc_bulkwriteopts_t* _opts;

    ~bulk_write_options_c() {
        libmongoc::bulkwriteopts_destroy(_opts);
    }

    bulk_write_options_c(bulk_write_options_c&&) = delete;
    bulk_write_options_c& operator=(bulk_write_options_c&&) = delete;
    bulk_write_options_c(bulk_write_options_c const&) = delete;
    bulk_write_options_c& operator=(bulk_write_options_c const&) = delete;

    explicit bulk_write_options_c(client_bulk_write::options const& opts) : _opts{libmongoc::bulkwriteopts_new()} {
        if (!opts.ordered()) {
            libmongoc::bulkwriteopts_set_ordered(_opts, false);
        }

        if (auto const v = opts.bypass_document_validation()) {
            libmongoc::bulkwriteopts_set_bypassdocumentvalidation(_opts, *v);
        }

        if (auto const& v = client_bulk_write::options::internal::comment(opts)) {
            libmongoc::bulkwriteopts_set_comment(_opts, &bsoncxx::v1::types::value::internal::get_bson_value(*v));
        }

        if (auto const v = opts.let()) {
            libmongoc::bulkwriteopts_set_let(_opts, scoped_bson_view{*v});
        }

        if (auto const v = opts.verbose_results()) {
            libmongoc::bulkwriteopts_set_verboseresults(_opts, *v);
        }

        if (auto const& v = client_bulk_write::options::internal::write_concern(opts)) {
            libmongoc::bulkwriteopts_set_writeconcern(_opts, write_concern::internal::as_mongoc(*v));
        }
    }

    mongoc_bulkwriteopts_t* get() const {
        return _opts;
    }
};

struct bulk_write_return {
    mongoc_bulkwriteresult_t* res = nullptr;
    mongoc_bulkwriteexception_t* exc = nullptr;

    ~bulk_write_return() {
        libmongoc::bulkwriteresult_destroy(res);
        libmongoc::bulkwriteexception_destroy(exc);
    }

    bulk_write_return(bulk_write_return&&) = delete;
    bulk_write_return& operator=(bulk_write_return&&) = delete;
    bulk_write_return(bulk_write_return const&) = delete;
    bulk_write_return& operator=(bulk_write_return const&) = delete;

    explicit bulk_write_return(mongoc_bulkwritereturn_t ret) : res{ret.res}, exc{ret.exc} {}

    bool has_exception() const {
        return exc != nullptr;
    }

    bsoncxx::v1::stdx::optional<client_bulk_write::result> make_result() {
        if (!res) {
            return bsoncxx::v1::stdx::nullopt;
        }

        return client_bulk_write::result::internal::make(exchange(res, nullptr));
    }

    client_bulk_write::exception make_exception() {
        return client_bulk_write::exception::internal::make(exchange(exc, nullptr), make_result());
    }
};

} // namespace

client_bulk_write& client_bulk_write::append(
    bsoncxx::v1::stdx::string_view ns,
    bsoncxx::v1::document::value document,
    insert_one_options const& /*opts*/) {
    bson_error_t error = {};

    if (!libmongoc::bulkwrite_append_insertone(
            impl::with(*this)._bulk, std::string{ns}.c_str(), scoped_bson_view{document}, nullptr, &error)) {
        throw_exception(error);
    }

    return *this;
}

client_bulk_write& client_bulk_write::append(
    bsoncxx::v1::stdx::string_view ns,
    bsoncxx::v1::document::value filter,
    bsoncxx::v1::document::value update,
    update_one_options const& opts) {
    update_one_options_c const c_opts{opts};

    bson_error_t error = {};

    if (!libmongoc::bulkwrite_append_updateone(
            impl::with(*this)._bulk,
            std::string{ns}.c_str(),
            scoped_bson_view{filter},
            scoped_bson_view{update},
            c_opts.get(),
            &error)) {
        throw_exception(error);
    }

    return *this;
}

client_bulk_write& client_bulk_write::append(
    bsoncxx::v1::stdx::string_view ns,
    bsoncxx::v1::document::value filter,
    v1::pipeline const& update,
    update_one_options const& opts) {
    return append(ns, std::move(filter), bsoncxx::v1::document::value{update.view_array()}, opts);
}

client_bulk_write& client_bulk_write::append(
    bsoncxx::v1::stdx::string_view ns,
    bsoncxx::v1::document::value filter,
    bsoncxx::v1::document::value update,
    update_many_options const& opts) {
    update_many_options_c const c_opts{opts};

    bson_error_t error = {};

    if (!libmongoc::bulkwrite_append_updatemany(
            impl::with(*this)._bulk,
            std::string{ns}.c_str(),
            scoped_bson_view{filter},
            scoped_bson_view{update},
            c_opts.get(),
            &error)) {
        throw_exception(error);
    }

    return *this;
}

client_bulk_write& client_bulk_write::append(
    bsoncxx::v1::stdx::string_view ns,
    bsoncxx::v1::document::value filter,
    v1::pipeline const& update,
    update_many_options const& opts) {
    return append(ns, std::move(filter), bsoncxx::v1::document::value{update.view_array()}, opts);
}

client_bulk_write& client_bulk_write::append(
    bsoncxx::v1::stdx::string_view ns,
    bsoncxx::v1::document::value filter,
    bsoncxx::v1::document::value replacement,
    replace_one_options const& opts) {
    replace_one_options_c const c_opts{opts};

    bson_error_t error = {};

    if (!libmongoc::bulkwrite_append_replaceone(
            impl::with(*this)._bulk,
            std::string{ns}.c_str(),
            scoped_bson_view{filter},
            scoped_bson_view{replacement},
            c_opts.get(),
            &error)) {
        throw_exception(error);
    }

    return *this;
}

client_bulk_write& client_bulk_write::append(
    bsoncxx::v1::stdx::string_view ns,
    bsoncxx::v1::document::value filter,
    delete_one_options const& opts) {
    delete_one_options_c const c_opts{opts};

    bson_error_t error = {};

    if (!libmongoc::bulkwrite_append_deleteone(
            impl::with(*this)._bulk, std::string{ns}.c_str(), scoped_bson_view{filter}, c_opts.get(), &error)) {
        throw_exception(error);
    }

    return *this;
}

client_bulk_write& client_bulk_write::append(
    bsoncxx::v1::stdx::string_view ns,
    bsoncxx::v1::document::value filter,
    delete_many_options const& opts) {
    delete_many_options_c const c_opts{opts};

    bson_error_t error = {};

    if (!libmongoc::bulkwrite_append_deletemany(
            impl::with(*this)._bulk, std::string{ns}.c_str(), scoped_bson_view{filter}, c_opts.get(), &error)) {
        throw_exception(error);
    }

    return *this;
}

bsoncxx::v1::stdx::optional<client_bulk_write::result> client_bulk_write::execute(options const& opts) {
    bulk_write_options_c const c_opts{opts};
    bulk_write_return ret{libmongoc::bulkwrite_execute(impl::with(*this)._bulk, c_opts.get())};

    if (ret.has_exception()) {
        throw ret.make_exception();
    }

    return ret.make_result();
}

// NOLINTEND(cppcoreguidelines-owning-memory)

} // namespace v1
} // namespace mongocxx
