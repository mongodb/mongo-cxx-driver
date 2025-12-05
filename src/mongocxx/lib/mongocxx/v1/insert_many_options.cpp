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

#include <mongocxx/v1/insert_many_options.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/write_concern.hpp>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class insert_many_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bool> _bypass_document_validation;
    bsoncxx::v1::stdx::optional<v1::write_concern> _write_concern;
    bsoncxx::v1::stdx::optional<bool> _ordered;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _comment;

    static impl const& with(insert_many_options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(insert_many_options const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(insert_many_options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(insert_many_options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

insert_many_options::~insert_many_options() {
    delete impl::with(this);
}

insert_many_options::insert_many_options(insert_many_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

insert_many_options& insert_many_options::operator=(insert_many_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

insert_many_options::insert_many_options(insert_many_options const& other) : _impl{new impl{impl::with(other)}} {}

insert_many_options& insert_many_options::operator=(insert_many_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

insert_many_options::insert_many_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

insert_many_options& insert_many_options::bypass_document_validation(bool bypass_document_validation) {
    impl::with(this)->_bypass_document_validation = bypass_document_validation;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> insert_many_options::bypass_document_validation() const {
    return impl::with(this)->_bypass_document_validation;
}

insert_many_options& insert_many_options::write_concern(v1::write_concern wc) {
    impl::with(this)->_write_concern = std::move(wc);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::write_concern> insert_many_options::write_concern() const {
    return impl::with(this)->_write_concern;
}

insert_many_options& insert_many_options::ordered(bool ordered) {
    impl::with(this)->_ordered = ordered;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> insert_many_options::ordered() const {
    return impl::with(this)->_ordered;
}

insert_many_options& insert_many_options::comment(bsoncxx::v1::types::value comment) {
    impl::with(this)->_comment = std::move(comment);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> insert_many_options::comment() const {
    return impl::with(this)->_comment;
}

bsoncxx::v1::stdx::optional<v1::write_concern>& insert_many_options::internal::write_concern(
    insert_many_options& self) {
    return impl::with(self)._write_concern;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& insert_many_options::internal::comment(
    insert_many_options& self) {
    return impl::with(self)._comment;
}

} // namespace v1
} // namespace mongocxx
