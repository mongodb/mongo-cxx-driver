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

#include <mongocxx/v1/distinct_options.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/read_preference.hpp>

#include <bsoncxx/v1/types/value.hh>

#include <chrono>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class distinct_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> _max_time;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _comment;
    bsoncxx::v1::stdx::optional<mongocxx::v1::read_preference> _read_preference;

    static impl const& with(distinct_options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(distinct_options const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(distinct_options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(distinct_options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

distinct_options::~distinct_options() {
    delete impl::with(this);
}

distinct_options::distinct_options(distinct_options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

distinct_options& distinct_options::operator=(distinct_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

distinct_options::distinct_options(distinct_options const& other) : _impl{new impl{impl::with(other)}} {}

distinct_options& distinct_options::operator=(distinct_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

distinct_options::distinct_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

distinct_options& distinct_options::collation(bsoncxx::v1::document::value collation) {
    impl::with(this)->_collation = std::move(collation);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> distinct_options::collation() const {
    return impl::with(this)->_collation;
}

distinct_options& distinct_options::max_time(std::chrono::milliseconds max_time) {
    impl::with(this)->_max_time = std::move(max_time);
    return *this;
}

bsoncxx::v1::stdx::optional<std::chrono::milliseconds> distinct_options::max_time() const {
    return impl::with(this)->_max_time;
}

distinct_options& distinct_options::comment(bsoncxx::v1::types::value comment) {
    impl::with(this)->_comment = std::move(comment);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> distinct_options::comment() const {
    return impl::with(this)->_comment;
}

distinct_options& distinct_options::read_preference(v1::read_preference rp) {
    impl::with(this)->_read_preference = std::move(rp);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::read_preference> distinct_options::read_preference() const {
    return impl::with(this)->_read_preference;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& distinct_options::internal::collation(
    distinct_options const& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<std::chrono::milliseconds> const& distinct_options::internal::max_time(
    distinct_options const& self) {
    return impl::with(self)._max_time;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> const& distinct_options::internal::comment(
    distinct_options const& self) {
    return impl::with(self)._comment;
}

bsoncxx::v1::stdx::optional<mongocxx::v1::read_preference> const& distinct_options::internal::read_preference(
    distinct_options const& self) {
    return impl::with(self)._read_preference;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& distinct_options::internal::collation(
    distinct_options& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<std::chrono::milliseconds>& distinct_options::internal::max_time(distinct_options& self) {
    return impl::with(self)._max_time;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& distinct_options::internal::comment(distinct_options& self) {
    return impl::with(self)._comment;
}

bsoncxx::v1::stdx::optional<mongocxx::v1::read_preference>& distinct_options::internal::read_preference(
    distinct_options& self) {
    return impl::with(self)._read_preference;
}

} // namespace v1
} // namespace mongocxx
