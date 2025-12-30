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

#include <mongocxx/v1/insert_one_result.hh>

//

#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/bulk_write.hpp>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class insert_one_result::impl {
   public:
    v1::bulk_write::result _result;
    bsoncxx::v1::types::value _inserted_id;

    impl(v1::bulk_write::result result, bsoncxx::v1::types::value inserted_id)
        : _result{std::move(result)}, _inserted_id{std::move(inserted_id)} {}

    static impl const& with(insert_one_result const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(insert_one_result const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(insert_one_result& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(insert_one_result* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

insert_one_result::~insert_one_result() {
    delete impl::with(_impl);
}

insert_one_result::insert_one_result(insert_one_result&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

insert_one_result& insert_one_result::operator=(insert_one_result&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

insert_one_result::insert_one_result(insert_one_result const& other) : _impl{new impl{impl::with(other)}} {}

insert_one_result& insert_one_result::operator=(insert_one_result const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

v1::bulk_write::result insert_one_result::result() const {
    return impl::with(this)->_result;
}

std::int64_t insert_one_result::inserted_count() const {
    return impl::with(this)->_result.inserted_count();
}

bsoncxx::v1::types::view insert_one_result::inserted_id() const {
    return impl::with(this)->_inserted_id;
}

bool operator==(insert_one_result const& lhs, insert_one_result const& rhs) {
    auto& l = insert_one_result::impl::with(lhs);
    auto& r = insert_one_result::impl::with(rhs);

    return l._result == r._result && l._inserted_id == r._inserted_id;
}

insert_one_result insert_one_result::internal::make(
    v1::bulk_write::result result,
    bsoncxx::v1::types::value inserted_id) {
    return {new impl{std::move(result), std::move(inserted_id)}};
}

v1::bulk_write::result& insert_one_result::internal::result(insert_one_result& self) {
    return impl::with(self)._result;
}

bsoncxx::v1::types::value& insert_one_result::internal::inserted_id(insert_one_result& self) {
    return impl::with(self)._inserted_id;
}

insert_one_result::insert_one_result(void* impl) : _impl{impl} {}

} // namespace v1
} // namespace mongocxx
