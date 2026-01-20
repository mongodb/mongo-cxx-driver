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

#include <mongocxx/v1/find_one_and_delete_options.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/hint.hpp>
#include <mongocxx/v1/write_concern.hpp>

#include <chrono>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class find_one_and_delete_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> _max_time;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _projection;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _sort;
    bsoncxx::v1::stdx::optional<v1::write_concern> _write_concern;
    bsoncxx::v1::stdx::optional<v1::hint> _hint;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _let;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _comment;

    static impl const& with(find_one_and_delete_options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(find_one_and_delete_options const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(find_one_and_delete_options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(find_one_and_delete_options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

find_one_and_delete_options::~find_one_and_delete_options() {
    delete impl::with(this);
}

find_one_and_delete_options::find_one_and_delete_options(find_one_and_delete_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

find_one_and_delete_options& find_one_and_delete_options::operator=(find_one_and_delete_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

find_one_and_delete_options::find_one_and_delete_options(find_one_and_delete_options const& other)
    : _impl{new impl{impl::with(other)}} {}

find_one_and_delete_options& find_one_and_delete_options::operator=(find_one_and_delete_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

find_one_and_delete_options::find_one_and_delete_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

find_one_and_delete_options& find_one_and_delete_options::collation(bsoncxx::v1::document::value collation) {
    impl::with(this)->_collation = std::move(collation);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> find_one_and_delete_options::collation() const {
    return impl::with(this)->_collation;
}

find_one_and_delete_options& find_one_and_delete_options::max_time(std::chrono::milliseconds max_time) {
    impl::with(this)->_max_time = std::move(max_time);
    return *this;
}

bsoncxx::v1::stdx::optional<std::chrono::milliseconds> find_one_and_delete_options::max_time() const {
    return impl::with(this)->_max_time;
}

find_one_and_delete_options& find_one_and_delete_options::projection(bsoncxx::v1::document::value projection) {
    impl::with(this)->_projection = std::move(projection);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> find_one_and_delete_options::projection() const {
    return impl::with(this)->_projection;
}

find_one_and_delete_options& find_one_and_delete_options::sort(bsoncxx::v1::document::value ordering) {
    impl::with(this)->_sort = std::move(ordering);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> find_one_and_delete_options::sort() const {
    return impl::with(this)->_sort;
}

find_one_and_delete_options& find_one_and_delete_options::write_concern(v1::write_concern write_concern) {
    impl::with(this)->_write_concern = std::move(write_concern);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::write_concern> find_one_and_delete_options::write_concern() const {
    return impl::with(this)->_write_concern;
}

find_one_and_delete_options& find_one_and_delete_options::hint(v1::hint index_hint) {
    impl::with(this)->_hint = std::move(index_hint);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> find_one_and_delete_options::hint() const {
    return impl::with(this)->_hint;
}

find_one_and_delete_options& find_one_and_delete_options::let(bsoncxx::v1::document::value let) {
    impl::with(this)->_let = std::move(let);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> const find_one_and_delete_options::let() const {
    return impl::with(this)->_let;
}

find_one_and_delete_options& find_one_and_delete_options::comment(bsoncxx::v1::types::value comment) {
    impl::with(this)->_comment = std::move(comment);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> const find_one_and_delete_options::comment() const {
    return impl::with(this)->_comment;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& find_one_and_delete_options::internal::collation(
    find_one_and_delete_options& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& find_one_and_delete_options::internal::projection(
    find_one_and_delete_options& self) {
    return impl::with(self)._projection;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& find_one_and_delete_options::internal::sort(
    find_one_and_delete_options& self) {
    return impl::with(self)._sort;
}

bsoncxx::v1::stdx::optional<v1::write_concern>& find_one_and_delete_options::internal::write_concern(
    find_one_and_delete_options& self) {
    return impl::with(self)._write_concern;
}

bsoncxx::v1::stdx::optional<v1::hint>& find_one_and_delete_options::internal::hint(find_one_and_delete_options& self) {
    return impl::with(self)._hint;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& find_one_and_delete_options::internal::let(
    find_one_and_delete_options& self) {
    return impl::with(self)._let;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& find_one_and_delete_options::internal::comment(
    find_one_and_delete_options& self) {
    return impl::with(self)._comment;
}

} // namespace v1
} // namespace mongocxx
