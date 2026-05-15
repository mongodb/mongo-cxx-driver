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

#include <mongocxx/v1/stream_processor_info.hh>

//

#include <bsoncxx/v1/array/value.hpp>
#include <bsoncxx/v1/array/view.hpp>
#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/element/view.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/id.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <chrono>
#include <cstdint>
#include <string>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class stream_processor_info::impl {
   public:
    std::string _id;
    std::string _name;
    std::string _state;
    bsoncxx::v1::array::value _pipeline{};
    bsoncxx::v1::stdx::optional<std::int32_t> _pipeline_version;
    bsoncxx::v1::stdx::optional<std::string> _tier;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _dlq;
    bsoncxx::v1::stdx::optional<std::string> _stream_meta_field_name;
    bool _enable_auto_scaling{false};
    bool _failover_enabled{false};
    std::string _active_region;
    std::string _workspace_default_region;
    bsoncxx::v1::stdx::optional<std::chrono::system_clock::time_point> _last_state_change;
    bsoncxx::v1::stdx::optional<std::chrono::system_clock::time_point> _last_modified_at;
    std::string _modified_by;
    bool _has_started{false};
    std::string _error_msg;
    bool _error_retryable{false};
    bsoncxx::v1::stdx::optional<std::int32_t> _error_code;

    static impl const& with(stream_processor_info const& other) {
        return *static_cast<impl const*>(other._impl);
    }
    static impl const* with(stream_processor_info const* other) {
        return static_cast<impl const*>(other->_impl);
    }
    static impl& with(stream_processor_info& other) {
        return *static_cast<impl*>(other._impl);
    }
    static impl* with(stream_processor_info* other) {
        return static_cast<impl*>(other->_impl);
    }
    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

namespace {

bsoncxx::v1::stdx::optional<std::chrono::system_clock::time_point> date_from_element(bsoncxx::v1::element::view el) {
    if (el && el.type_id() == bsoncxx::v1::types::id::k_date) {
        return std::chrono::system_clock::time_point{
            std::chrono::duration_cast<std::chrono::system_clock::duration>(el.get_date().value)};
    }
    return bsoncxx::v1::stdx::nullopt;
}

} // namespace

// NOLINTBEGIN(cppcoreguidelines-owning-memory)

stream_processor_info::~stream_processor_info() {
    delete impl::with(_impl);
}

stream_processor_info::stream_processor_info(stream_processor_info&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

stream_processor_info& stream_processor_info::operator=(stream_processor_info&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }
    return *this;
}

stream_processor_info::stream_processor_info(stream_processor_info const& other) : _impl{new impl{impl::with(other)}} {}

stream_processor_info& stream_processor_info::operator=(stream_processor_info const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }
    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

stream_processor_info::stream_processor_info(bsoncxx::v1::document::view doc) : _impl{new impl{}} {
    auto* i = impl::with(this);

    // The server nests processor fields under different keys depending on the
    // command and server version. Try known wrapper keys in priority order,
    // then fall back to the top-level document.
    bsoncxx::v1::document::view src = doc;
    for (auto const* key : {"result", "streamProcessor"}) {
        if (auto el = doc[key]; el && el.type_id() == bsoncxx::v1::types::id::k_document) {
            src = el.get_document().value;
            break;
        }
    }

    auto extract_string = [&](char const* key, std::string& out) {
        auto el = src[key];
        if (el && el.type_id() == bsoncxx::v1::types::id::k_string) {
            out = std::string{el.get_string().value};
        }
    };

    auto extract_bool = [&](char const* key, bool& out) {
        auto el = src[key];
        if (el && el.type_id() == bsoncxx::v1::types::id::k_bool) {
            out = el.get_bool().value;
        }
    };

    extract_string("_id", i->_id);
    extract_string("id", i->_id); // server may use either
    extract_string("name", i->_name);
    extract_string("state", i->_state);

    if (auto el = src["pipeline"]; el && el.type_id() == bsoncxx::v1::types::id::k_array) {
        i->_pipeline = bsoncxx::v1::array::value{el.get_array().value};
    }

    if (auto el = src["pipelineVersion"]; el && el.type_id() == bsoncxx::v1::types::id::k_int32) {
        i->_pipeline_version = el.get_int32().value;
    }
    if (auto el = src["tier"]; el && el.type_id() == bsoncxx::v1::types::id::k_string) {
        i->_tier = std::string{el.get_string().value};
    }
    if (auto el = src["dlq"]; el && el.type_id() == bsoncxx::v1::types::id::k_document) {
        i->_dlq = bsoncxx::v1::document::value{el.get_document().value};
    }
    if (auto el = src["streamMetaFieldName"]; el && el.type_id() == bsoncxx::v1::types::id::k_string) {
        i->_stream_meta_field_name = std::string{el.get_string().value};
    }

    extract_bool("enableAutoScaling", i->_enable_auto_scaling);
    extract_bool("failoverEnabled", i->_failover_enabled);
    extract_string("activeRegion", i->_active_region);
    extract_string("workspaceDefaultRegion", i->_workspace_default_region);

    i->_last_state_change = date_from_element(src["lastStateChange"]);
    i->_last_modified_at = date_from_element(src["lastModifiedAt"]);

    extract_string("modifiedBy", i->_modified_by);
    extract_bool("hasStarted", i->_has_started);
    extract_string("errorMsg", i->_error_msg);
    extract_bool("errorRetryable", i->_error_retryable);

    if (auto el = src["errorCode"]; el && el.type_id() == bsoncxx::v1::types::id::k_int32) {
        i->_error_code = el.get_int32().value;
    }
}

bsoncxx::v1::stdx::string_view stream_processor_info::id() const {
    return impl::with(this)->_id;
}
bsoncxx::v1::stdx::string_view stream_processor_info::name() const {
    return impl::with(this)->_name;
}
bsoncxx::v1::stdx::string_view stream_processor_info::state() const {
    return impl::with(this)->_state;
}
bsoncxx::v1::array::view stream_processor_info::pipeline() const {
    return impl::with(this)->_pipeline.view();
}
bsoncxx::v1::stdx::optional<std::int32_t> stream_processor_info::pipeline_version() const {
    return impl::with(this)->_pipeline_version;
}
bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> stream_processor_info::tier() const {
    if (auto const& v = impl::with(this)->_tier) {
        return bsoncxx::v1::stdx::string_view{*v};
    }
    return bsoncxx::v1::stdx::nullopt;
}
bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> stream_processor_info::dlq() const {
    return impl::with(this)->_dlq;
}
bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> stream_processor_info::stream_meta_field_name() const {
    if (auto const& v = impl::with(this)->_stream_meta_field_name) {
        return bsoncxx::v1::stdx::string_view{*v};
    }
    return bsoncxx::v1::stdx::nullopt;
}
bool stream_processor_info::enable_auto_scaling() const {
    return impl::with(this)->_enable_auto_scaling;
}
bool stream_processor_info::failover_enabled() const {
    return impl::with(this)->_failover_enabled;
}
bsoncxx::v1::stdx::string_view stream_processor_info::active_region() const {
    return impl::with(this)->_active_region;
}
bsoncxx::v1::stdx::string_view stream_processor_info::workspace_default_region() const {
    return impl::with(this)->_workspace_default_region;
}
bsoncxx::v1::stdx::optional<std::chrono::system_clock::time_point> stream_processor_info::last_state_change() const {
    return impl::with(this)->_last_state_change;
}
bsoncxx::v1::stdx::optional<std::chrono::system_clock::time_point> stream_processor_info::last_modified_at() const {
    return impl::with(this)->_last_modified_at;
}
bsoncxx::v1::stdx::string_view stream_processor_info::modified_by() const {
    return impl::with(this)->_modified_by;
}
bool stream_processor_info::has_started() const {
    return impl::with(this)->_has_started;
}
bsoncxx::v1::stdx::string_view stream_processor_info::error_msg() const {
    return impl::with(this)->_error_msg;
}
bool stream_processor_info::error_retryable() const {
    return impl::with(this)->_error_retryable;
}
bsoncxx::v1::stdx::optional<std::int32_t> stream_processor_info::error_code() const {
    return impl::with(this)->_error_code;
}

} // namespace v1
} // namespace mongocxx
