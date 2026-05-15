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

#include <mongocxx/v1/start_stream_processor_options.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <mongocxx/v1/failover_options.hh>

#include <cstdint>
#include <stdexcept>
#include <string>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class start_stream_processor_options::impl {
   public:
    bsoncxx::v1::stdx::optional<std::int32_t> _workers;
    bsoncxx::v1::stdx::optional<bool> _clear_checkpoints;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::b_timestamp> _start_at_operation_time;
    bsoncxx::v1::stdx::optional<std::string> _tier;
    bsoncxx::v1::stdx::optional<bool> _enable_auto_scaling;
    bsoncxx::v1::stdx::optional<v1::failover_options> _failover;

    static impl const& with(start_stream_processor_options const& other) {
        return *static_cast<impl const*>(other._impl);
    }
    static impl const* with(start_stream_processor_options const* other) {
        return static_cast<impl const*>(other->_impl);
    }
    static impl& with(start_stream_processor_options& other) {
        return *static_cast<impl*>(other._impl);
    }
    static impl* with(start_stream_processor_options* other) {
        return static_cast<impl*>(other->_impl);
    }
    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory)

start_stream_processor_options::~start_stream_processor_options() {
    delete impl::with(_impl);
}

start_stream_processor_options::start_stream_processor_options(start_stream_processor_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

start_stream_processor_options& start_stream_processor_options::operator=(
    start_stream_processor_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }
    return *this;
}

start_stream_processor_options::start_stream_processor_options(start_stream_processor_options const& other)
    : _impl{new impl{impl::with(other)}} {}

start_stream_processor_options& start_stream_processor_options::operator=(start_stream_processor_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }
    return *this;
}

start_stream_processor_options::start_stream_processor_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

start_stream_processor_options& start_stream_processor_options::workers(std::int32_t v) {
    impl::with(this)->_workers = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> start_stream_processor_options::workers() const {
    return impl::with(this)->_workers;
}

start_stream_processor_options& start_stream_processor_options::clear_checkpoints(bool v) {
    impl::with(this)->_clear_checkpoints = v;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> start_stream_processor_options::clear_checkpoints() const {
    return impl::with(this)->_clear_checkpoints;
}

start_stream_processor_options& start_stream_processor_options::start_at_operation_time(
    bsoncxx::v1::types::b_timestamp v) {
    impl::with(this)->_start_at_operation_time = v;
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::b_timestamp> start_stream_processor_options::start_at_operation_time()
    const {
    return impl::with(this)->_start_at_operation_time;
}

start_stream_processor_options& start_stream_processor_options::tier(std::string v) {
    impl::with(this)->_tier = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> start_stream_processor_options::tier() const {
    if (auto const& v = impl::with(this)->_tier) {
        return bsoncxx::v1::stdx::string_view{*v};
    }
    return bsoncxx::v1::stdx::nullopt;
}

start_stream_processor_options& start_stream_processor_options::enable_auto_scaling(bool v) {
    impl::with(this)->_enable_auto_scaling = v;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> start_stream_processor_options::enable_auto_scaling() const {
    return impl::with(this)->_enable_auto_scaling;
}

start_stream_processor_options& start_stream_processor_options::failover(v1::failover_options v) {
    impl::with(this)->_failover = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::failover_options> start_stream_processor_options::failover() const {
    return impl::with(this)->_failover;
}

void start_stream_processor_options::internal::append_to(start_stream_processor_options const& self, scoped_bson& doc) {
    auto const& i = impl::with(self);

    if (i._workers) {
        doc += scoped_bson{BCON_NEW("workers", BCON_INT32(*i._workers))};
    }

    scoped_bson opts_doc;
    if (i._clear_checkpoints) {
        opts_doc += scoped_bson{BCON_NEW("clearCheckpoints", BCON_BOOL(*i._clear_checkpoints))};
    }
    if (i._start_at_operation_time) {
        auto const& ts = *i._start_at_operation_time;
        scoped_bson v;
        // BCON_TIMESTAMP() incorrectly uses int32_ptr instead of uint32_ptr. Use BSON_*() API instead.
        if (!BSON_APPEND_TIMESTAMP(v.inout_ptr(), "startAtOperationTime", ts.timestamp, ts.increment)) {
            throw std::logic_error{"start_stream_processor_options: BSON_APPEND_TIMESTAMP failed"};
        }
        opts_doc += v;
    }
    // NOTE: startAfter is RESERVED and MUST NOT be serialized to the wire.
    if (i._tier) {
        opts_doc += scoped_bson{BCON_NEW("tier", BCON_UTF8(i._tier->c_str()))};
    }
    if (i._enable_auto_scaling) {
        opts_doc += scoped_bson{BCON_NEW("enableAutoScaling", BCON_BOOL(*i._enable_auto_scaling))};
    }
    if (opts_doc.view()) {
        doc += scoped_bson{BCON_NEW("options", BCON_DOCUMENT(opts_doc.bson()))};
    }

    if (i._failover) {
        scoped_bson failover_doc;
        v1::failover_options::internal::append_to(*i._failover, failover_doc);
        doc += scoped_bson{BCON_NEW("failover", BCON_DOCUMENT(failover_doc.bson()))};
    }
}

} // namespace v1
} // namespace mongocxx
