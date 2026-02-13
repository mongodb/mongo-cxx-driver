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

#include <mongocxx/v1/gridfs/upload_options.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>

#include <cstdint>
#include <utility>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {
namespace gridfs {

class upload_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _metadata;
    bsoncxx::v1::stdx::optional<std::int32_t> _chunk_size_bytes;

    static impl const& with(upload_options const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(upload_options const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(upload_options& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(upload_options* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

upload_options::~upload_options() {
    delete impl::with(_impl);
}

upload_options::upload_options(upload_options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

upload_options& upload_options::operator=(upload_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

upload_options::upload_options(upload_options const& other) : _impl{new impl{impl::with(other)}} {}

upload_options& upload_options::operator=(upload_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

upload_options::upload_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

upload_options& upload_options::chunk_size_bytes(std::int32_t chunk_size_bytes) {
    impl::with(this)->_chunk_size_bytes = chunk_size_bytes;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> upload_options::chunk_size_bytes() const {
    return impl::with(this)->_chunk_size_bytes;
}

upload_options& upload_options::metadata(bsoncxx::v1::document::value metadata) {
    impl::with(this)->_metadata = std::move(metadata);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> upload_options::metadata() const {
    return impl::with(this)->_metadata;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& upload_options::internal::metadata(
    upload_options const& self) {
    return impl::with(self)._metadata;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& upload_options::internal::metadata(upload_options& self) {
    return impl::with(self)._metadata;
}

} // namespace gridfs
} // namespace v1
} // namespace mongocxx
