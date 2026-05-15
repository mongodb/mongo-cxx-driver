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

#include <mongocxx/v1/stream_processing_client.hh>

//

#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/client.hpp>
#include <mongocxx/v1/database.hpp>

#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/stream_processors.hh>
#include <mongocxx/v1/uri.hh>

#include <string>
#include <vector>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

namespace {

bool is_workspace_host(v1::uri const& uri) {
    auto const hosts = uri.hosts();
    for (auto const& host : hosts) {
        std::string const& hostname = host.name;
        if (hostname.find("atlas-stream-") == 0) {
            return true;
        }
    }
    return false;
}

v1::uri prepare_workspace_uri(v1::uri const& in_uri) {
    auto const auth_src = in_uri.auth_source();

    if (!auth_src.empty() && auth_src != "admin") {
        throw v1::exception::internal::make(
            v1::uri::errc::set_failure,
            "only authSource=admin is supported for stream processing workspace connections");
    }

    // Build a modified URI string:
    // - Force TLS=true for atlas-stream-* hosts.
    // - Inject authSource=admin if not already present.
    std::string uri_str{in_uri.to_string()};

    // Determine how to append query parameters.
    bool has_query = uri_str.find('?') != std::string::npos;
    auto append_param = [&](std::string const& param) {
        if (has_query) {
            uri_str += '&';
        } else {
            uri_str += '?';
            has_query = true;
        }
        uri_str += param;
    };

    if (is_workspace_host(in_uri) && !in_uri.tls()) {
        append_param("tls=true");
    }

    if (auth_src.empty()) {
        append_param("authSource=admin");
    }

    return v1::uri{uri_str};
}

} // namespace

class stream_processing_client::impl {
   public:
    v1::client _client;

    static impl const& with(stream_processing_client const& other) {
        return *static_cast<impl const*>(other._impl);
    }
    static impl const* with(stream_processing_client const* other) {
        return static_cast<impl const*>(other->_impl);
    }
    static impl& with(stream_processing_client& other) {
        return *static_cast<impl*>(other._impl);
    }
    static impl* with(stream_processing_client* other) {
        return static_cast<impl*>(other->_impl);
    }
    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory)

stream_processing_client::~stream_processing_client() {
    delete impl::with(_impl);
}

stream_processing_client::stream_processing_client(stream_processing_client&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

stream_processing_client& stream_processing_client::operator=(stream_processing_client&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }
    return *this;
}

stream_processing_client::stream_processing_client(v1::uri uri) : stream_processing_client{std::move(uri), {}} {}

stream_processing_client::stream_processing_client(v1::uri uri, v1::client::options const& options)
    : _impl{new impl{v1::client{prepare_workspace_uri(uri), options}}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

v1::stream_processors stream_processing_client::stream_processors() {
    return v1::stream_processors::internal::make(impl::with(this)->_client["admin"]);
}

} // namespace v1
} // namespace mongocxx
