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

#include <mongocxx/v1/gridfs/upload_result.hh>

//

#include <bsoncxx/v1/types/value.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <utility>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {
namespace gridfs {

class upload_result::impl {
   public:
    bsoncxx::v1::types::value _id;

    explicit impl(bsoncxx::v1::types::value id) : _id{std::move(id)} {}

    static impl const& with(upload_result const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(upload_result const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(upload_result& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(upload_result* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

upload_result::~upload_result() {
    delete impl::with(_impl);
}

upload_result::upload_result(upload_result&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

upload_result& upload_result::operator=(upload_result&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

upload_result::upload_result(upload_result const& other) : _impl{new impl{impl::with(other)}} {}

upload_result& upload_result::operator=(upload_result const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

bsoncxx::v1::types::view upload_result::id() const {
    return impl::with(this)->_id;
}

bool operator==(upload_result const& lhs, upload_result const& rhs) {
    return upload_result::impl::with(lhs)._id == upload_result::impl::with(rhs)._id;
}

upload_result::upload_result(void* impl) : _impl{impl} {}

upload_result upload_result::internal::make(bsoncxx::v1::types::value id) {
    return {new impl{std::move(id)}};
}

bsoncxx::v1::types::value& upload_result::internal::id(upload_result& self) {
    return impl::with(self)._id;
}

} // namespace gridfs
} // namespace v1
} // namespace mongocxx
