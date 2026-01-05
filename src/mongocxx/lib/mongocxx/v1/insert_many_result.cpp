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

#include <mongocxx/v1/insert_many_result.hh>

//

#include <bsoncxx/v1/array/value.hpp>

#include <mongocxx/v1/bulk_write.hpp>

#include <cstddef>
#include <cstdint>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class insert_many_result::impl {
   public:
    v1::bulk_write::result _result;
    bsoncxx::v1::array::value _inserted_ids;

    impl(v1::bulk_write::result result, bsoncxx::v1::array::value inserted_ids)
        : _result{std::move(result)}, _inserted_ids{std::move(inserted_ids)} {}

    static impl const& with(insert_many_result const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(insert_many_result const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(insert_many_result& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(insert_many_result* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

insert_many_result::~insert_many_result() {
    delete impl::with(_impl);
}

insert_many_result::insert_many_result(insert_many_result&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

insert_many_result& insert_many_result::operator=(insert_many_result&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

insert_many_result::insert_many_result(insert_many_result const& other) : _impl{new impl{impl::with(other)}} {}

insert_many_result& insert_many_result::operator=(insert_many_result const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

v1::bulk_write::result insert_many_result::result() const {
    return impl::with(this)->_result;
}

std::int64_t insert_many_result::inserted_count() const {
    return impl::with(this)->_result.inserted_count();
}

insert_many_result::id_map insert_many_result::inserted_ids() const {
    id_map ret;

    std::int64_t idx = 0;
    for (auto const e : impl::with(this)->_inserted_ids) {
        ret.emplace(idx++, e.type_view());
    }

    return ret;
}

bool operator==(insert_many_result const& lhs, insert_many_result const& rhs) {
    auto& l = insert_many_result::impl::with(lhs);
    auto& r = insert_many_result::impl::with(rhs);

    return l._result == r._result && l._inserted_ids == r._inserted_ids;
}

insert_many_result insert_many_result::internal::make(
    v1::bulk_write::result result,
    bsoncxx::v1::array::value inserted_ids) {
    return {new impl{std::move(result), std::move(inserted_ids)}};
}

v1::bulk_write::result& insert_many_result::internal::result(insert_many_result& self) {
    return impl::with(self)._result;
}

bsoncxx::v1::array::value& insert_many_result::internal::inserted_ids(insert_many_result& self) {
    return impl::with(self)._inserted_ids;
}

insert_many_result::insert_many_result(void* impl) : _impl{impl} {}

} // namespace v1
} // namespace mongocxx
