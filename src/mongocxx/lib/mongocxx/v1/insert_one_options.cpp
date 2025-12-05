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

#include <mongocxx/v1/insert_one_options.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/write_concern.hpp>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class insert_one_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bool> _bypass_document_validation;
    bsoncxx::v1::stdx::optional<v1::write_concern> _write_concern;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _comment;

    static impl const& with(insert_one_options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(insert_one_options const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(insert_one_options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(insert_one_options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

insert_one_options::~insert_one_options() {
    delete impl::with(this);
}

insert_one_options::insert_one_options(insert_one_options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

insert_one_options& insert_one_options::operator=(insert_one_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

insert_one_options::insert_one_options(insert_one_options const& other) : _impl{new impl{impl::with(other)}} {}

insert_one_options& insert_one_options::operator=(insert_one_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

insert_one_options::insert_one_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

insert_one_options& insert_one_options::bypass_document_validation(bool bypass_document_validation) {
    impl::with(this)->_bypass_document_validation = bypass_document_validation;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> insert_one_options::bypass_document_validation() const {
    return impl::with(this)->_bypass_document_validation;
}

insert_one_options& insert_one_options::write_concern(v1::write_concern wc) {
    impl::with(this)->_write_concern = std::move(wc);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::write_concern> insert_one_options::write_concern() const {
    return impl::with(this)->_write_concern;
}

insert_one_options& insert_one_options::comment(bsoncxx::v1::types::value comment) {
    impl::with(this)->_comment = std::move(comment);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> insert_one_options::comment() const {
    return impl::with(this)->_comment;
}

bsoncxx::v1::stdx::optional<v1::write_concern>& insert_one_options::internal::write_concern(insert_one_options& self) {
    return impl::with(self)._write_concern;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& insert_one_options::internal::comment(
    insert_one_options& self) {
    return impl::with(self)._comment;
}

} // namespace v1
} // namespace mongocxx
