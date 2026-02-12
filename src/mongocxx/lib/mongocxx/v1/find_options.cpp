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

#include <mongocxx/v1/find_options.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/cursor.hpp>
#include <mongocxx/v1/read_concern.hpp>
#include <mongocxx/v1/read_preference.hpp>

#include <bsoncxx/v1/types/value.hh>

#include <mongocxx/v1/hint.hh>

#include <chrono>
#include <cstdint>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class find_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bool> _allow_disk_use;
    bsoncxx::v1::stdx::optional<bool> _allow_partial_results;
    bsoncxx::v1::stdx::optional<std::int32_t> _batch_size;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _comment;
    bsoncxx::v1::stdx::optional<v1::cursor::type> _cursor_type;
    bsoncxx::v1::stdx::optional<v1::hint> _hint;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _let;
    bsoncxx::v1::stdx::optional<std::int64_t> _limit;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _max;
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> _max_await_time;
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> _max_time;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _min;
    bsoncxx::v1::stdx::optional<bool> _no_cursor_timeout;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _projection;
    bsoncxx::v1::stdx::optional<v1::read_preference> _read_preference;
    bsoncxx::v1::stdx::optional<v1::read_concern> _read_concern;
    bsoncxx::v1::stdx::optional<bool> _return_key;
    bsoncxx::v1::stdx::optional<bool> _show_record_id;
    bsoncxx::v1::stdx::optional<std::int64_t> _skip;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _sort;

    static impl const& with(find_options const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(find_options const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(find_options& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(find_options* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

find_options::~find_options() {
    delete impl::with(_impl);
}

find_options::find_options(find_options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

find_options& find_options::operator=(find_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

find_options::find_options(find_options const& other) : _impl{new impl{impl::with(other)}} {}

find_options& find_options::operator=(find_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

find_options::find_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

find_options& find_options::allow_disk_use(bool allow_disk_use) {
    impl::with(this)->_allow_disk_use = allow_disk_use;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> find_options::allow_disk_use() const {
    return impl::with(this)->_allow_disk_use;
}

find_options& find_options::allow_partial_results(bool allow_partial) {
    impl::with(this)->_allow_partial_results = allow_partial;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> find_options::allow_partial_results() const {
    return impl::with(this)->_allow_partial_results;
}

find_options& find_options::batch_size(std::int32_t batch_size) {
    impl::with(this)->_batch_size = batch_size;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> find_options::batch_size() const {
    return impl::with(this)->_batch_size;
}

find_options& find_options::collation(bsoncxx::v1::document::value collation) {
    impl::with(this)->_collation = std::move(collation);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> find_options::collation() const {
    return impl::with(this)->_collation;
}

find_options& find_options::comment(bsoncxx::v1::types::value comment) {
    impl::with(this)->_comment = std::move(comment);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> find_options::comment() const {
    return impl::with(this)->_comment;
}

find_options& find_options::cursor_type(v1::cursor::type cursor_type) {
    impl::with(this)->_cursor_type = cursor_type;
    return *this;
}

bsoncxx::v1::stdx::optional<cursor::type> find_options::cursor_type() const {
    return impl::with(this)->_cursor_type;
}

find_options& find_options::hint(v1::hint index_hint) {
    impl::with(this)->_hint = std::move(index_hint);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> find_options::hint() const {
    return impl::with(this)->_hint;
}

find_options& find_options::let(bsoncxx::v1::document::value let) {
    impl::with(this)->_let = std::move(let);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> const find_options::let() const {
    return impl::with(this)->_let;
}

find_options& find_options::limit(std::int64_t limit) {
    impl::with(this)->_limit = limit;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int64_t> find_options::limit() const {
    return impl::with(this)->_limit;
}

find_options& find_options::max(bsoncxx::v1::document::value max) {
    impl::with(this)->_max = std::move(max);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> find_options::max() const {
    return impl::with(this)->_max;
}

find_options& find_options::max_await_time(std::chrono::milliseconds max_await_time) {
    impl::with(this)->_max_await_time = max_await_time;
    return *this;
}

bsoncxx::v1::stdx::optional<std::chrono::milliseconds> find_options::max_await_time() const {
    return impl::with(this)->_max_await_time;
}

find_options& find_options::max_time(std::chrono::milliseconds max_time) {
    impl::with(this)->_max_time = max_time;
    return *this;
}

bsoncxx::v1::stdx::optional<std::chrono::milliseconds> find_options::max_time() const {
    return impl::with(this)->_max_time;
}

find_options& find_options::min(bsoncxx::v1::document::value min) {
    impl::with(this)->_min = std::move(min);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> find_options::min() const {
    return impl::with(this)->_min;
}

find_options& find_options::no_cursor_timeout(bool no_cursor_timeout) {
    impl::with(this)->_no_cursor_timeout = no_cursor_timeout;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> find_options::no_cursor_timeout() const {
    return impl::with(this)->_no_cursor_timeout;
}

find_options& find_options::projection(bsoncxx::v1::document::value projection) {
    impl::with(this)->_projection = std::move(projection);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> find_options::projection() const {
    return impl::with(this)->_projection;
}

find_options& find_options::read_preference(v1::read_preference rp) {
    impl::with(this)->_read_preference = std::move(rp);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::read_preference> find_options::read_preference() const {
    return impl::with(this)->_read_preference;
}

find_options& find_options::read_concern(v1::read_concern rc) {
    impl::with(this)->_read_concern = std::move(rc);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::read_concern> find_options::read_concern() const {
    return impl::with(this)->_read_concern;
}

find_options& find_options::return_key(bool return_key) {
    impl::with(this)->_return_key = return_key;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> find_options::return_key() const {
    return impl::with(this)->_return_key;
}

find_options& find_options::show_record_id(bool show_record_id) {
    impl::with(this)->_show_record_id = show_record_id;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> find_options::show_record_id() const {
    return impl::with(this)->_show_record_id;
}

find_options& find_options::skip(std::int64_t skip) {
    impl::with(this)->_skip = skip;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int64_t> find_options::skip() const {
    return impl::with(this)->_skip;
}

find_options& find_options::sort(bsoncxx::v1::document::value ordering) {
    impl::with(this)->_sort = std::move(ordering);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> find_options::sort() const {
    return impl::with(this)->_sort;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& find_options::internal::collation(
    find_options const& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> const& find_options::internal::comment(
    find_options const& self) {
    return impl::with(self)._comment;
}

bsoncxx::v1::stdx::optional<v1::hint> const& find_options::internal::hint(find_options const& self) {
    return impl::with(self)._hint;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& find_options::internal::let(find_options const& self) {
    return impl::with(self)._let;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& find_options::internal::max(find_options const& self) {
    return impl::with(self)._max;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& find_options::internal::min(find_options const& self) {
    return impl::with(self)._min;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& find_options::internal::projection(
    find_options const& self) {
    return impl::with(self)._projection;
}

bsoncxx::v1::stdx::optional<v1::read_preference> const& find_options::internal::read_preference(
    find_options const& self) {
    return impl::with(self)._read_preference;
}

bsoncxx::v1::stdx::optional<v1::read_concern> const& find_options::internal::read_concern(find_options const& self) {
    return impl::with(self)._read_concern;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& find_options::internal::sort(
    find_options const& self) {
    return impl::with(self)._sort;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& find_options::internal::collation(find_options& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& find_options::internal::comment(find_options& self) {
    return impl::with(self)._comment;
}

bsoncxx::v1::stdx::optional<v1::hint>& find_options::internal::hint(find_options& self) {
    return impl::with(self)._hint;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& find_options::internal::let(find_options& self) {
    return impl::with(self)._let;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& find_options::internal::max(find_options& self) {
    return impl::with(self)._max;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& find_options::internal::min(find_options& self) {
    return impl::with(self)._min;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& find_options::internal::projection(find_options& self) {
    return impl::with(self)._projection;
}

bsoncxx::v1::stdx::optional<v1::read_preference>& find_options::internal::read_preference(find_options& self) {
    return impl::with(self)._read_preference;
}

bsoncxx::v1::stdx::optional<v1::read_concern>& find_options::internal::read_concern(find_options& self) {
    return impl::with(self)._read_concern;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& find_options::internal::sort(find_options& self) {
    return impl::with(self)._sort;
}

} // namespace v1
} // namespace mongocxx
