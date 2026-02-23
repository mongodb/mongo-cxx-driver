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

#include <mongocxx/v1/estimated_document_count_options.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/read_preference.hpp>

#include <bsoncxx/v1/types/value.hh>

#include <chrono>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class estimated_document_count_options::impl {
   public:
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> _max_time;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _comment;
    bsoncxx::v1::stdx::optional<v1::read_preference> _read_preference;

    static impl const& with(estimated_document_count_options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(estimated_document_count_options const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(estimated_document_count_options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(estimated_document_count_options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

estimated_document_count_options::~estimated_document_count_options() {
    delete impl::with(this);
}

estimated_document_count_options::estimated_document_count_options(estimated_document_count_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

estimated_document_count_options& estimated_document_count_options::operator=(
    estimated_document_count_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

estimated_document_count_options::estimated_document_count_options(estimated_document_count_options const& other)
    : _impl{new impl{impl::with(other)}} {}

estimated_document_count_options& estimated_document_count_options::operator=(
    estimated_document_count_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

estimated_document_count_options::estimated_document_count_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

estimated_document_count_options& estimated_document_count_options::max_time(std::chrono::milliseconds max_time) {
    impl::with(this)->_max_time = max_time;
    return *this;
}

bsoncxx::v1::stdx::optional<std::chrono::milliseconds> estimated_document_count_options::max_time() const {
    return impl::with(this)->_max_time;
}

estimated_document_count_options& estimated_document_count_options::comment(bsoncxx::v1::types::value comment) {
    impl::with(this)->_comment = std::move(comment);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> estimated_document_count_options::comment() const {
    return impl::with(this)->_comment;
}

estimated_document_count_options& estimated_document_count_options::read_preference(v1::read_preference rp) {
    impl::with(this)->_read_preference = std::move(rp);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::read_preference> estimated_document_count_options::read_preference() const {
    return impl::with(this)->_read_preference;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> const& estimated_document_count_options::internal::comment(
    estimated_document_count_options const& self) {
    return impl::with(self)._comment;
}

bsoncxx::v1::stdx::optional<v1::read_preference> const& estimated_document_count_options::internal::read_preference(
    estimated_document_count_options const& self) {
    return impl::with(self)._read_preference;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& estimated_document_count_options::internal::comment(
    estimated_document_count_options& self) {
    return impl::with(self)._comment;
}

bsoncxx::v1::stdx::optional<v1::read_preference>& estimated_document_count_options::internal::read_preference(
    estimated_document_count_options& self) {
    return impl::with(self)._read_preference;
}

} // namespace v1
} // namespace mongocxx
