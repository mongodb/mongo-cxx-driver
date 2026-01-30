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

#include <mongocxx/v1/delete_one_options.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/hint.hpp>
#include <mongocxx/v1/write_concern.hpp>

#include <bsoncxx/v1/types/value.hh>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class delete_one_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<v1::write_concern> _write_concern;
    bsoncxx::v1::stdx::optional<v1::hint> _hint;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _let;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _comment;

    static impl const& with(delete_one_options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(delete_one_options const* self) {
        return static_cast<impl const*>(self->_impl);
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

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

delete_one_options::~delete_one_options() {
    delete impl::with(this);
}

delete_one_options::delete_one_options(delete_one_options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

delete_one_options& delete_one_options::operator=(delete_one_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

delete_one_options::delete_one_options(delete_one_options const& other) : _impl{new impl{impl::with(other)}} {}

delete_one_options& delete_one_options::operator=(delete_one_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

delete_one_options::delete_one_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

delete_one_options& delete_one_options::collation(bsoncxx::v1::document::value collation) {
    impl::with(this)->_collation = std::move(collation);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> delete_one_options::collation() const {
    return impl::with(this)->_collation;
}

delete_one_options& delete_one_options::write_concern(v1::write_concern wc) {
    impl::with(this)->_write_concern = std::move(wc);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::write_concern> delete_one_options::write_concern() const {
    return impl::with(this)->_write_concern;
}

delete_one_options& delete_one_options::hint(v1::hint index_hint) {
    impl::with(this)->_hint = std::move(index_hint);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> delete_one_options::hint() const {
    return impl::with(this)->_hint;
}

delete_one_options& delete_one_options::let(bsoncxx::v1::document::value let) {
    impl::with(this)->_let = std::move(let);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> const delete_one_options::let() const {
    return impl::with(this)->_let;
}

delete_one_options& delete_one_options::comment(bsoncxx::v1::types::value comment) {
    impl::with(this)->_comment = std::move(comment);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> const delete_one_options::comment() const {
    return impl::with(this)->_comment;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& delete_one_options::internal::collation(
    delete_one_options const& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<v1::write_concern> const& delete_one_options::internal::write_concern(
    delete_one_options const& self) {
    return impl::with(self)._write_concern;
}

bsoncxx::v1::stdx::optional<v1::hint> const& delete_one_options::internal::hint(delete_one_options const& self) {
    return impl::with(self)._hint;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& delete_one_options::internal::let(
    delete_one_options const& self) {
    return impl::with(self)._let;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> const& delete_one_options::internal::comment(
    delete_one_options const& self) {
    return impl::with(self)._comment;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& delete_one_options::internal::collation(
    delete_one_options& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<v1::write_concern>& delete_one_options::internal::write_concern(delete_one_options& self) {
    return impl::with(self)._write_concern;
}

bsoncxx::v1::stdx::optional<v1::hint>& delete_one_options::internal::hint(delete_one_options& self) {
    return impl::with(self)._hint;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& delete_one_options::internal::let(delete_one_options& self) {
    return impl::with(self)._let;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& delete_one_options::internal::comment(
    delete_one_options& self) {
    return impl::with(self)._comment;
}

} // namespace v1
} // namespace mongocxx
