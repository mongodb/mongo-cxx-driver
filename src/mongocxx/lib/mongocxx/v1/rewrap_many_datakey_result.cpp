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

#include <mongocxx/v1/rewrap_many_datakey_result.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/bulk_write.hpp>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class rewrap_many_datakey_result::impl {
   public:
    bsoncxx::v1::stdx::optional<v1::bulk_write::result> _result;

    static impl const& with(rewrap_many_datakey_result const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(rewrap_many_datakey_result const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(rewrap_many_datakey_result& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(rewrap_many_datakey_result* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

rewrap_many_datakey_result::~rewrap_many_datakey_result() {
    delete impl::with(_impl);
}

rewrap_many_datakey_result::rewrap_many_datakey_result(rewrap_many_datakey_result&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

rewrap_many_datakey_result& rewrap_many_datakey_result::operator=(rewrap_many_datakey_result&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

rewrap_many_datakey_result::rewrap_many_datakey_result(rewrap_many_datakey_result const& other)
    : _impl{new impl{impl::with(other)}} {}

rewrap_many_datakey_result& rewrap_many_datakey_result::operator=(rewrap_many_datakey_result const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

bsoncxx::v1::stdx::optional<v1::bulk_write::result> rewrap_many_datakey_result::result() const {
    return impl::with(this)->_result;
}

rewrap_many_datakey_result rewrap_many_datakey_result::internal::make() {
    return {new impl{}};
}

bsoncxx::v1::stdx::optional<v1::bulk_write::result>& rewrap_many_datakey_result::internal::result(
    rewrap_many_datakey_result& self) {
    return impl::with(self)._result;
}

rewrap_many_datakey_result::rewrap_many_datakey_result(void* impl) : _impl{impl} {}

} // namespace v1
} // namespace mongocxx
