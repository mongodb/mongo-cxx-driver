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

#include <mongocxx/v1/aggregate_options.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/hint.hpp>
#include <mongocxx/v1/read_concern.hpp>
#include <mongocxx/v1/read_preference.hpp>
#include <mongocxx/v1/write_concern.hpp>

#include <chrono>
#include <cstdint>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class aggregate_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bool> _allow_disk_use;
    bsoncxx::v1::stdx::optional<std::int32_t> _batch_size;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _let;
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> _max_time;
    bsoncxx::v1::stdx::optional<v1::read_preference> _read_preference;
    bsoncxx::v1::stdx::optional<bool> _bypass_document_validation;
    bsoncxx::v1::stdx::optional<v1::hint> _hint;
    bsoncxx::v1::stdx::optional<v1::write_concern> _write_concern;
    bsoncxx::v1::stdx::optional<v1::read_concern> _read_concern;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _comment;

    static impl const& with(aggregate_options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(aggregate_options const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(aggregate_options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(aggregate_options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

aggregate_options::~aggregate_options() {
    delete impl::with(this);
}

aggregate_options::aggregate_options(aggregate_options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

aggregate_options& aggregate_options::operator=(aggregate_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

aggregate_options::aggregate_options(aggregate_options const& other) : _impl{new impl{impl::with(other)}} {}

aggregate_options& aggregate_options::operator=(aggregate_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

aggregate_options::aggregate_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

aggregate_options& aggregate_options::allow_disk_use(bool allow_disk_use) {
    impl::with(this)->_allow_disk_use = allow_disk_use;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> aggregate_options::allow_disk_use() const {
    return impl::with(this)->_allow_disk_use;
}

aggregate_options& aggregate_options::batch_size(std::int32_t batch_size) {
    impl::with(this)->_batch_size = batch_size;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> aggregate_options::batch_size() const {
    return impl::with(this)->_batch_size;
}

aggregate_options& aggregate_options::collation(bsoncxx::v1::document::value collation) {
    impl::with(this)->_collation = std::move(collation);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> aggregate_options::collation() const {
    return impl::with(this)->_collation;
}

aggregate_options& aggregate_options::let(bsoncxx::v1::document::value let) {
    impl::with(this)->_let = std::move(let);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> aggregate_options::let() const {
    return impl::with(this)->_let;
}

aggregate_options& aggregate_options::max_time(std::chrono::milliseconds max_time) {
    impl::with(this)->_max_time = max_time;
    return *this;
}

bsoncxx::v1::stdx::optional<std::chrono::milliseconds> aggregate_options::max_time() const {
    return impl::with(this)->_max_time;
}

aggregate_options& aggregate_options::read_preference(v1::read_preference rp) {
    impl::with(this)->_read_preference = std::move(rp);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::read_preference> aggregate_options::read_preference() const {
    return impl::with(this)->_read_preference;
}

aggregate_options& aggregate_options::bypass_document_validation(bool bypass_document_validation) {
    impl::with(this)->_bypass_document_validation = bypass_document_validation;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> aggregate_options::bypass_document_validation() const {
    return impl::with(this)->_bypass_document_validation;
}

aggregate_options& aggregate_options::hint(v1::hint index_hint) {
    impl::with(this)->_hint = std::move(index_hint);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> aggregate_options::hint() const {
    return impl::with(this)->_hint;
}

aggregate_options& aggregate_options::write_concern(v1::write_concern write_concern) {
    impl::with(this)->_write_concern = std::move(write_concern);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::write_concern> aggregate_options::write_concern() const {
    return impl::with(this)->_write_concern;
}

aggregate_options& aggregate_options::read_concern(v1::read_concern read_concern) {
    impl::with(this)->_read_concern = std::move(read_concern);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::read_concern> aggregate_options::read_concern() const {
    return impl::with(this)->_read_concern;
}

aggregate_options& aggregate_options::comment(bsoncxx::v1::types::value comment) {
    impl::with(this)->_comment = std::move(comment);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> aggregate_options::comment() const {
    return impl::with(this)->_comment;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& aggregate_options::internal::collation(
    aggregate_options& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& aggregate_options::internal::let(aggregate_options& self) {
    return impl::with(self)._let;
}

bsoncxx::v1::stdx::optional<v1::read_preference>& aggregate_options::internal::read_preference(
    aggregate_options& self) {
    return impl::with(self)._read_preference;
}

bsoncxx::v1::stdx::optional<v1::hint>& aggregate_options::internal::hint(aggregate_options& self) {
    return impl::with(self)._hint;
}

bsoncxx::v1::stdx::optional<v1::write_concern>& aggregate_options::internal::write_concern(aggregate_options& self) {
    return impl::with(self)._write_concern;
}

bsoncxx::v1::stdx::optional<v1::read_concern>& aggregate_options::internal::read_concern(aggregate_options& self) {
    return impl::with(self)._read_concern;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& aggregate_options::internal::comment(aggregate_options& self) {
    return impl::with(self)._comment;
}

} // namespace v1
} // namespace mongocxx
