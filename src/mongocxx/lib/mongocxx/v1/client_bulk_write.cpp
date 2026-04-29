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
#include <mongocxx/v1/write_concern.hpp>

#include <mongocxx/v1/exception.hh>

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

namespace {

[[noreturn]] void not_yet_implemented(char const* where) {
    throw std::logic_error(std::string{"mongocxx::v1::"} + where + ": not yet implemented");
}

} // namespace

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

class client_bulk_write::impl {};

client_bulk_write::~client_bulk_write() {
    delete static_cast<impl*>(_impl);
}

client_bulk_write::client_bulk_write(client_bulk_write&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

client_bulk_write& client_bulk_write::operator=(client_bulk_write&& other) noexcept {
    if (this != &other) {
        delete static_cast<impl*>(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client_bulk_write::client_bulk_write(void* /*bulk*/) : _impl{new impl{}} {}

client_bulk_write& client_bulk_write::append(
    bsoncxx::v1::stdx::string_view /*ns*/,
    bsoncxx::v1::document::value /*document*/,
    insert_one_options const& /*opts*/) {
    not_yet_implemented("client_bulk_write::append(insert_one_options)");
}

client_bulk_write& client_bulk_write::append(
    bsoncxx::v1::stdx::string_view /*ns*/,
    bsoncxx::v1::document::value /*filter*/,
    bsoncxx::v1::document::value /*update*/,
    update_one_options const& /*opts*/) {
    not_yet_implemented("client_bulk_write::append(update_one_options)");
}

client_bulk_write& client_bulk_write::append(
    bsoncxx::v1::stdx::string_view /*ns*/,
    bsoncxx::v1::document::value /*filter*/,
    v1::pipeline const& /*update*/,
    update_one_options const& /*opts*/) {
    not_yet_implemented("client_bulk_write::append(pipeline, update_one_options)");
}

client_bulk_write& client_bulk_write::append(
    bsoncxx::v1::stdx::string_view /*ns*/,
    bsoncxx::v1::document::value /*filter*/,
    bsoncxx::v1::document::value /*update*/,
    update_many_options const& /*opts*/) {
    not_yet_implemented("client_bulk_write::append(update_many_options)");
}

client_bulk_write& client_bulk_write::append(
    bsoncxx::v1::stdx::string_view /*ns*/,
    bsoncxx::v1::document::value /*filter*/,
    v1::pipeline const& /*update*/,
    update_many_options const& /*opts*/) {
    not_yet_implemented("client_bulk_write::append(pipeline, update_many_options)");
}

client_bulk_write& client_bulk_write::append(
    bsoncxx::v1::stdx::string_view /*ns*/,
    bsoncxx::v1::document::value /*filter*/,
    bsoncxx::v1::document::value /*replacement*/,
    replace_one_options const& /*opts*/) {
    not_yet_implemented("client_bulk_write::append(replace_one_options)");
}

client_bulk_write& client_bulk_write::append(
    bsoncxx::v1::stdx::string_view /*ns*/,
    bsoncxx::v1::document::value /*filter*/,
    delete_one_options const& /*opts*/) {
    not_yet_implemented("client_bulk_write::append(delete_one_options)");
}

client_bulk_write& client_bulk_write::append(
    bsoncxx::v1::stdx::string_view /*ns*/,
    bsoncxx::v1::document::value /*filter*/,
    delete_many_options const& /*opts*/) {
    not_yet_implemented("client_bulk_write::append(delete_many_options)");
}

bsoncxx::v1::stdx::optional<client_bulk_write::result> client_bulk_write::execute(options const& /*opts*/) {
    not_yet_implemented("client_bulk_write::execute");
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

class client_bulk_write::result::impl {};

client_bulk_write::result::~result() {
    delete static_cast<impl*>(_impl);
}

client_bulk_write::result::result(result&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

client_bulk_write::result& client_bulk_write::result::operator=(result&& other) noexcept {
    if (this != &other) {
        delete static_cast<impl*>(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client_bulk_write::result::result(result const& /*other*/) : _impl{new impl{}} {}

client_bulk_write::result& client_bulk_write::result::operator=(result const& other) {
    if (this != &other) {
        auto* const tmp = new impl{};
        delete static_cast<impl*>(_impl);
        _impl = tmp;
    }

    return *this;
}

std::int64_t client_bulk_write::result::inserted_count() const {
    not_yet_implemented("client_bulk_write::result::inserted_count");
}

std::int64_t client_bulk_write::result::upserted_count() const {
    not_yet_implemented("client_bulk_write::result::upserted_count");
}

std::int64_t client_bulk_write::result::matched_count() const {
    not_yet_implemented("client_bulk_write::result::matched_count");
}

std::int64_t client_bulk_write::result::modified_count() const {
    not_yet_implemented("client_bulk_write::result::modified_count");
}

std::int64_t client_bulk_write::result::deleted_count() const {
    not_yet_implemented("client_bulk_write::result::deleted_count");
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::result::insert_results() const {
    not_yet_implemented("client_bulk_write::result::insert_results");
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::result::update_results() const {
    not_yet_implemented("client_bulk_write::result::update_results");
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::result::delete_results() const {
    not_yet_implemented("client_bulk_write::result::delete_results");
}

class client_bulk_write::exception::impl {};

void client_bulk_write::exception::key_function() const {}

bsoncxx::v1::document::view client_bulk_write::exception::write_errors() const {
    not_yet_implemented("client_bulk_write::exception::write_errors");
}

bsoncxx::v1::array::view client_bulk_write::exception::write_concern_errors() const {
    not_yet_implemented("client_bulk_write::exception::write_concern_errors");
}

bsoncxx::v1::document::view client_bulk_write::exception::error_reply() const {
    not_yet_implemented("client_bulk_write::exception::error_reply");
}

bsoncxx::v1::stdx::optional<client_bulk_write::result> client_bulk_write::exception::partial_result() const {
    not_yet_implemented("client_bulk_write::exception::partial_result");
}

client_bulk_write::exception::exception(int code, char const* message, std::unique_ptr<impl> impl)
    : v1::exception{v1::exception::internal::make(code, std::generic_category(), message)}, _impl{std::move(impl)} {}

class client_bulk_write::insert_one_options::impl {};

client_bulk_write::insert_one_options::~insert_one_options() {
    delete static_cast<impl*>(_impl);
}

client_bulk_write::insert_one_options::insert_one_options(insert_one_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

client_bulk_write::insert_one_options& client_bulk_write::insert_one_options::operator=(
    insert_one_options&& other) noexcept {
    if (this != &other) {
        delete static_cast<impl*>(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client_bulk_write::insert_one_options::insert_one_options(insert_one_options const& /*other*/) : _impl{new impl{}} {}

client_bulk_write::insert_one_options& client_bulk_write::insert_one_options::operator=(
    insert_one_options const& other) {
    if (this != &other) {
        auto* const tmp = new impl{};
        delete static_cast<impl*>(_impl);
        _impl = tmp;
    }

    return *this;
}

client_bulk_write::insert_one_options::insert_one_options() : _impl{new impl{}} {}

class client_bulk_write::update_one_options::impl {};

client_bulk_write::update_one_options::~update_one_options() {
    delete static_cast<impl*>(_impl);
}

client_bulk_write::update_one_options::update_one_options(update_one_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

client_bulk_write::update_one_options& client_bulk_write::update_one_options::operator=(
    update_one_options&& other) noexcept {
    if (this != &other) {
        delete static_cast<impl*>(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client_bulk_write::update_one_options::update_one_options(update_one_options const& /*other*/) : _impl{new impl{}} {}

client_bulk_write::update_one_options& client_bulk_write::update_one_options::operator=(
    update_one_options const& other) {
    if (this != &other) {
        auto* const tmp = new impl{};
        delete static_cast<impl*>(_impl);
        _impl = tmp;
    }

    return *this;
}

client_bulk_write::update_one_options::update_one_options() : _impl{new impl{}} {}

client_bulk_write::update_one_options& client_bulk_write::update_one_options::array_filters(
    bsoncxx::v1::array::value /*array_filters*/) {
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::array::view> client_bulk_write::update_one_options::array_filters() const {
    not_yet_implemented("client_bulk_write::update_one_options::array_filters");
}

client_bulk_write::update_one_options& client_bulk_write::update_one_options::collation(
    bsoncxx::v1::document::value /*collation*/) {
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::update_one_options::collation() const {
    not_yet_implemented("client_bulk_write::update_one_options::collation");
}

client_bulk_write::update_one_options& client_bulk_write::update_one_options::hint(v1::hint /*hint*/) {
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> client_bulk_write::update_one_options::hint() const {
    not_yet_implemented("client_bulk_write::update_one_options::hint");
}

client_bulk_write::update_one_options& client_bulk_write::update_one_options::sort(
    bsoncxx::v1::document::value /*sort*/) {
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::update_one_options::sort() const {
    not_yet_implemented("client_bulk_write::update_one_options::sort");
}

client_bulk_write::update_one_options& client_bulk_write::update_one_options::upsert(bool /*upsert*/) {
    return *this;
}

bsoncxx::v1::stdx::optional<bool> client_bulk_write::update_one_options::upsert() const {
    not_yet_implemented("client_bulk_write::update_one_options::upsert");
}

class client_bulk_write::update_many_options::impl {};

client_bulk_write::update_many_options::~update_many_options() {
    delete static_cast<impl*>(_impl);
}

client_bulk_write::update_many_options::update_many_options(update_many_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

client_bulk_write::update_many_options& client_bulk_write::update_many_options::operator=(
    update_many_options&& other) noexcept {
    if (this != &other) {
        delete static_cast<impl*>(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client_bulk_write::update_many_options::update_many_options(update_many_options const& /*other*/) : _impl{new impl{}} {}

client_bulk_write::update_many_options& client_bulk_write::update_many_options::operator=(
    update_many_options const& other) {
    if (this != &other) {
        auto* const tmp = new impl{};
        delete static_cast<impl*>(_impl);
        _impl = tmp;
    }

    return *this;
}

client_bulk_write::update_many_options::update_many_options() : _impl{new impl{}} {}

client_bulk_write::update_many_options& client_bulk_write::update_many_options::array_filters(
    bsoncxx::v1::array::value /*array_filters*/) {
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::array::view> client_bulk_write::update_many_options::array_filters() const {
    not_yet_implemented("client_bulk_write::update_many_options::array_filters");
}

client_bulk_write::update_many_options& client_bulk_write::update_many_options::collation(
    bsoncxx::v1::document::value /*collation*/) {
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::update_many_options::collation() const {
    not_yet_implemented("client_bulk_write::update_many_options::collation");
}

client_bulk_write::update_many_options& client_bulk_write::update_many_options::hint(v1::hint /*hint*/) {
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> client_bulk_write::update_many_options::hint() const {
    not_yet_implemented("client_bulk_write::update_many_options::hint");
}

client_bulk_write::update_many_options& client_bulk_write::update_many_options::upsert(bool /*upsert*/) {
    return *this;
}

bsoncxx::v1::stdx::optional<bool> client_bulk_write::update_many_options::upsert() const {
    not_yet_implemented("client_bulk_write::update_many_options::upsert");
}

class client_bulk_write::replace_one_options::impl {};

client_bulk_write::replace_one_options::~replace_one_options() {
    delete static_cast<impl*>(_impl);
}

client_bulk_write::replace_one_options::replace_one_options(replace_one_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

client_bulk_write::replace_one_options& client_bulk_write::replace_one_options::operator=(
    replace_one_options&& other) noexcept {
    if (this != &other) {
        delete static_cast<impl*>(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client_bulk_write::replace_one_options::replace_one_options(replace_one_options const& /*other*/) : _impl{new impl{}} {}

client_bulk_write::replace_one_options& client_bulk_write::replace_one_options::operator=(
    replace_one_options const& other) {
    if (this != &other) {
        auto* const tmp = new impl{};
        delete static_cast<impl*>(_impl);
        _impl = tmp;
    }

    return *this;
}

client_bulk_write::replace_one_options::replace_one_options() : _impl{new impl{}} {}

client_bulk_write::replace_one_options& client_bulk_write::replace_one_options::collation(
    bsoncxx::v1::document::value /*collation*/) {
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::replace_one_options::collation() const {
    not_yet_implemented("client_bulk_write::replace_one_options::collation");
}

client_bulk_write::replace_one_options& client_bulk_write::replace_one_options::hint(v1::hint /*hint*/) {
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> client_bulk_write::replace_one_options::hint() const {
    not_yet_implemented("client_bulk_write::replace_one_options::hint");
}

client_bulk_write::replace_one_options& client_bulk_write::replace_one_options::sort(
    bsoncxx::v1::document::value /*sort*/) {
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::replace_one_options::sort() const {
    not_yet_implemented("client_bulk_write::replace_one_options::sort");
}

client_bulk_write::replace_one_options& client_bulk_write::replace_one_options::upsert(bool /*upsert*/) {
    return *this;
}

bsoncxx::v1::stdx::optional<bool> client_bulk_write::replace_one_options::upsert() const {
    not_yet_implemented("client_bulk_write::replace_one_options::upsert");
}

class client_bulk_write::delete_one_options::impl {};

client_bulk_write::delete_one_options::~delete_one_options() {
    delete static_cast<impl*>(_impl);
}

client_bulk_write::delete_one_options::delete_one_options(delete_one_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

client_bulk_write::delete_one_options& client_bulk_write::delete_one_options::operator=(
    delete_one_options&& other) noexcept {
    if (this != &other) {
        delete static_cast<impl*>(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client_bulk_write::delete_one_options::delete_one_options(delete_one_options const& /*other*/) : _impl{new impl{}} {}

client_bulk_write::delete_one_options& client_bulk_write::delete_one_options::operator=(
    delete_one_options const& other) {
    if (this != &other) {
        auto* const tmp = new impl{};
        delete static_cast<impl*>(_impl);
        _impl = tmp;
    }

    return *this;
}

client_bulk_write::delete_one_options::delete_one_options() : _impl{new impl{}} {}

client_bulk_write::delete_one_options& client_bulk_write::delete_one_options::collation(
    bsoncxx::v1::document::value /*collation*/) {
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::delete_one_options::collation() const {
    not_yet_implemented("client_bulk_write::delete_one_options::collation");
}

client_bulk_write::delete_one_options& client_bulk_write::delete_one_options::hint(v1::hint /*hint*/) {
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> client_bulk_write::delete_one_options::hint() const {
    not_yet_implemented("client_bulk_write::delete_one_options::hint");
}

class client_bulk_write::delete_many_options::impl {};

client_bulk_write::delete_many_options::~delete_many_options() {
    delete static_cast<impl*>(_impl);
}

client_bulk_write::delete_many_options::delete_many_options(delete_many_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

client_bulk_write::delete_many_options& client_bulk_write::delete_many_options::operator=(
    delete_many_options&& other) noexcept {
    if (this != &other) {
        delete static_cast<impl*>(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client_bulk_write::delete_many_options::delete_many_options(delete_many_options const& /*other*/) : _impl{new impl{}} {}

client_bulk_write::delete_many_options& client_bulk_write::delete_many_options::operator=(
    delete_many_options const& other) {
    if (this != &other) {
        auto* const tmp = new impl{};
        delete static_cast<impl*>(_impl);
        _impl = tmp;
    }

    return *this;
}

client_bulk_write::delete_many_options::delete_many_options() : _impl{new impl{}} {}

client_bulk_write::delete_many_options& client_bulk_write::delete_many_options::collation(
    bsoncxx::v1::document::value /*collation*/) {
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_bulk_write::delete_many_options::collation() const {
    not_yet_implemented("client_bulk_write::delete_many_options::collation");
}

client_bulk_write::delete_many_options& client_bulk_write::delete_many_options::hint(v1::hint /*hint*/) {
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> client_bulk_write::delete_many_options::hint() const {
    not_yet_implemented("client_bulk_write::delete_many_options::hint");
}

// NOLINTEND(cppcoreguidelines-owning-memory)

} // namespace v1
} // namespace mongocxx
