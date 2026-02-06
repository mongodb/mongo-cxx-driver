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

#include <mongocxx/v1/range_options.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/types/value.hpp>

#include <cstdint>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class range_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _min;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _max;
    bsoncxx::v1::stdx::optional<std::int64_t> _sparsity;
    bsoncxx::v1::stdx::optional<std::int32_t> _trim_factor;
    bsoncxx::v1::stdx::optional<std::int32_t> _precision;

    static impl const& with(range_options const& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl const* with(range_options const* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl& with(range_options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(range_options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

range_options::~range_options() {
    delete impl::with(this);
}

range_options::range_options(range_options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

range_options& range_options::operator=(range_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

range_options::range_options(range_options const& other) : _impl{new impl{impl::with(other)}} {}

range_options& range_options::operator=(range_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

range_options::range_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

range_options& range_options::min(bsoncxx::v1::types::value value) {
    impl::with(this)->_min = std::move(value);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> range_options::min() const {
    return impl::with(this)->_min;
}

range_options& range_options::max(bsoncxx::v1::types::value value) {
    impl::with(this)->_max = std::move(value);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> range_options::max() const {
    return impl::with(this)->_max;
}

range_options& range_options::sparsity(std::int64_t value) {
    impl::with(this)->_sparsity = std::move(value);
    return *this;
}

bsoncxx::v1::stdx::optional<std::int64_t> range_options::sparsity() const {
    return impl::with(this)->_sparsity;
}

range_options& range_options::trim_factor(std::int32_t value) {
    impl::with(this)->_trim_factor = std::move(value);
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> range_options::trim_factor() const {
    return impl::with(this)->_trim_factor;
}

range_options& range_options::precision(std::int32_t value) {
    impl::with(this)->_precision = std::move(value);
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> range_options::precision() const {
    return impl::with(this)->_precision;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> const& range_options::internal::min(range_options const& self) {
    return impl::with(self)._min;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> const& range_options::internal::max(range_options const& self) {
    return impl::with(self)._max;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& range_options::internal::min(range_options& self) {
    return impl::with(self)._min;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& range_options::internal::max(range_options& self) {
    return impl::with(self)._max;
}

} // namespace v1
} // namespace mongocxx
