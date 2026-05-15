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

#include <mongocxx/v1/get_stream_processor_samples_options.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>

#include <cstdint>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class get_stream_processor_samples_options::impl {
   public:
    bsoncxx::v1::stdx::optional<std::int64_t> _cursor_id;
    bsoncxx::v1::stdx::optional<std::int32_t> _limit;
    bsoncxx::v1::stdx::optional<std::int32_t> _batch_size;

    static impl const& with(get_stream_processor_samples_options const& other) {
        return *static_cast<impl const*>(other._impl);
    }
    static impl const* with(get_stream_processor_samples_options const* other) {
        return static_cast<impl const*>(other->_impl);
    }
    static impl& with(get_stream_processor_samples_options& other) {
        return *static_cast<impl*>(other._impl);
    }
    static impl* with(get_stream_processor_samples_options* other) {
        return static_cast<impl*>(other->_impl);
    }
    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory)

get_stream_processor_samples_options::~get_stream_processor_samples_options() {
    delete impl::with(_impl);
}

get_stream_processor_samples_options::get_stream_processor_samples_options(
    get_stream_processor_samples_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

get_stream_processor_samples_options& get_stream_processor_samples_options::operator=(
    get_stream_processor_samples_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }
    return *this;
}

get_stream_processor_samples_options::get_stream_processor_samples_options(
    get_stream_processor_samples_options const& other)
    : _impl{new impl{impl::with(other)}} {}

get_stream_processor_samples_options& get_stream_processor_samples_options::operator=(
    get_stream_processor_samples_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }
    return *this;
}

get_stream_processor_samples_options::get_stream_processor_samples_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

get_stream_processor_samples_options& get_stream_processor_samples_options::cursor_id(std::int64_t v) {
    impl::with(this)->_cursor_id = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int64_t> get_stream_processor_samples_options::cursor_id() const {
    return impl::with(this)->_cursor_id;
}

get_stream_processor_samples_options& get_stream_processor_samples_options::limit(std::int32_t v) {
    impl::with(this)->_limit = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> get_stream_processor_samples_options::limit() const {
    return impl::with(this)->_limit;
}

get_stream_processor_samples_options& get_stream_processor_samples_options::batch_size(std::int32_t v) {
    impl::with(this)->_batch_size = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> get_stream_processor_samples_options::batch_size() const {
    return impl::with(this)->_batch_size;
}

} // namespace v1
} // namespace mongocxx
