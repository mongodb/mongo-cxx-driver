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

#include <mongocxx/v1/update_many_options.hh>

//

#include <bsoncxx/v1/array/value.hpp>
#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <mongocxx/v1/hint.hpp>
#include <mongocxx/v1/write_concern.hpp>

#include <bsoncxx/v1/types/value.hh>

#include <stdexcept>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class update_many_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bool> _bypass_document_validation;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<v1::hint> _hint;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _let;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _sort;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _comment;
    bsoncxx::v1::stdx::optional<bool> _upsert;
    bsoncxx::v1::stdx::optional<v1::write_concern> _write_concern;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::array::value> _array_filters;

    static impl const& with(update_many_options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(update_many_options const* self) {
        return static_cast<impl const*>(self->_impl);
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

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

update_many_options::~update_many_options() {
    delete impl::with(this);
}

update_many_options::update_many_options(update_many_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

update_many_options& update_many_options::operator=(update_many_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

update_many_options::update_many_options(update_many_options const& other) : _impl{new impl{impl::with(other)}} {}

update_many_options& update_many_options::operator=(update_many_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

update_many_options::update_many_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

update_many_options& update_many_options::bypass_document_validation(bool bypass_document_validation) {
    impl::with(this)->_bypass_document_validation = bypass_document_validation;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> update_many_options::bypass_document_validation() const {
    return impl::with(this)->_bypass_document_validation;
}

update_many_options& update_many_options::collation(bsoncxx::v1::document::value collation) {
    impl::with(this)->_collation = std::move(collation);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> update_many_options::collation() const {
    return impl::with(this)->_collation;
}

update_many_options& update_many_options::hint(v1::hint index_hint) {
    impl::with(this)->_hint = std::move(index_hint);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> update_many_options::hint() const {
    return impl::with(this)->_hint;
}

update_many_options& update_many_options::let(bsoncxx::v1::document::value let) {
    impl::with(this)->_let = std::move(let);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> const update_many_options::let() const {
    return impl::with(this)->_let;
}

update_many_options& update_many_options::sort(bsoncxx::v1::document::value sort) {
    impl::with(this)->_sort = std::move(sort);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> update_many_options::sort() const {
    return impl::with(this)->_sort;
}

update_many_options& update_many_options::comment(bsoncxx::v1::types::value comment) {
    impl::with(this)->_comment = std::move(comment);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> const update_many_options::comment() const {
    return impl::with(this)->_comment;
}

update_many_options& update_many_options::upsert(bool upsert) {
    impl::with(this)->_upsert = upsert;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> update_many_options::upsert() const {
    return impl::with(this)->_upsert;
}

update_many_options& update_many_options::write_concern(v1::write_concern wc) {
    impl::with(this)->_write_concern = std::move(wc);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::write_concern> update_many_options::write_concern() const {
    return impl::with(this)->_write_concern;
}

update_many_options& update_many_options::array_filters(bsoncxx::v1::array::value array_filters) {
    impl::with(this)->_array_filters = std::move(array_filters);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::array::view> update_many_options::array_filters() const {
    return impl::with(this)->_array_filters;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& update_many_options::internal::collation(
    update_many_options const& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<v1::hint> const& update_many_options::internal::hint(update_many_options const& self) {
    return impl::with(self)._hint;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::array::value> const& update_many_options::internal::array_filters(
    update_many_options const& self) {
    return impl::with(self)._array_filters;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& update_many_options::internal::collation(
    update_many_options& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<v1::hint>& update_many_options::internal::hint(update_many_options& self) {
    return impl::with(self)._hint;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& update_many_options::internal::let(
    update_many_options& self) {
    return impl::with(self)._let;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& update_many_options::internal::sort(
    update_many_options& self) {
    return impl::with(self)._sort;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& update_many_options::internal::comment(
    update_many_options& self) {
    return impl::with(self)._comment;
}

bsoncxx::v1::stdx::optional<v1::write_concern>& update_many_options::internal::write_concern(
    update_many_options& self) {
    return impl::with(self)._write_concern;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::array::value>& update_many_options::internal::array_filters(
    update_many_options& self) {
    return impl::with(self)._array_filters;
}

void update_many_options::internal::append_to(update_many_options const& self, scoped_bson& doc) {
    if (auto const& opt = impl::with(self)._bypass_document_validation) {
        doc += scoped_bson{BCON_NEW("bypassDocumentValidation", BCON_BOOL(*opt))};
    }

    if (auto const& opt = impl::with(self)._write_concern) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(scoped_bson{opt->to_document()}.bson()))};
    }

    if (auto const& opt = impl::with(self)._let) {
        doc += scoped_bson{BCON_NEW("let", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = impl::with(self)._comment) {
        scoped_bson v;

        if (!BSON_APPEND_VALUE(v.inout_ptr(), "comment", &bsoncxx::v1::types::value::internal::get_bson_value(*opt))) {
            throw std::logic_error{
                "mongocxx::v1::update_many_options::internal::to_document: BSON_APPEND_VALUE failed"};
        }

        doc += v;
    }
}

} // namespace v1
} // namespace mongocxx
