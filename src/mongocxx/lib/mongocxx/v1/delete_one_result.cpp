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

#include <mongocxx/v1/delete_one_result.hh>

//

#include <mongocxx/v1/bulk_write.hpp>

#include <cstdint>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class delete_one_result::impl {
   public:
    v1::bulk_write::result _result;

    impl(v1::bulk_write::result result) : _result{std::move(result)} {}

    static impl const& with(delete_one_result const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(delete_one_result const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(delete_one_result& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(delete_one_result* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

delete_one_result::~delete_one_result() {
    delete impl::with(_impl);
}

delete_one_result::delete_one_result(delete_one_result&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

delete_one_result& delete_one_result::operator=(delete_one_result&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

delete_one_result::delete_one_result(delete_one_result const& other) : _impl{new impl{impl::with(other)}} {}

delete_one_result& delete_one_result::operator=(delete_one_result const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

v1::bulk_write::result delete_one_result::result() const {
    return impl::with(this)->_result;
}

std::int64_t delete_one_result::deleted_count() const {
    return impl::with(this)->_result.deleted_count();
}

bool operator==(delete_one_result const& lhs, delete_one_result const& rhs) {
    auto& l = delete_one_result::impl::with(lhs);
    auto& r = delete_one_result::impl::with(rhs);

    return l._result == r._result;
}

delete_one_result delete_one_result::internal::make(v1::bulk_write::result result) {
    return {new impl{std::move(result)}};
}

v1::bulk_write::result& delete_one_result::internal::result(delete_one_result& self) {
    return impl::with(self)._result;
}

delete_one_result::delete_one_result(void* impl) : _impl{impl} {}

} // namespace v1
} // namespace mongocxx
