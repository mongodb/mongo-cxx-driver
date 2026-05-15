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

#pragma once

#include <mongocxx/stream_processing_client-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/stream_processing_client.hpp> // IWYU pragma: export

#include <mongocxx/stream_processors.hpp>
#include <mongocxx/uri.hpp>

#include <mongocxx/config/prelude.hpp>

#include <utility>

namespace mongocxx {
namespace v_noabi {

///
/// A client connected to an Atlas Stream Processing workspace.
///
/// TLS is silently enabled for atlas-stream-* hosts if not already set.
/// authSource defaults to "admin"; specifying another authSource throws.
///
class stream_processing_client {
   public:
    /* explicit(false) */ stream_processing_client(v1::stream_processing_client client)
        : _client{std::move(client)} {}

    explicit operator v1::stream_processing_client() && {
        return std::move(_client);
    }

    ///
    /// Constructs a client for the given workspace URI.
    ///
    explicit stream_processing_client(v_noabi::uri const& uri)
        : _client{v1::stream_processing_client{v1::uri{uri.to_string()}}} {}

    ///
    /// Constructs a client with the given workspace URI and client options.
    ///
    stream_processing_client(v_noabi::uri const& uri, v_noabi::options::client const& options)
        : _client{v1::stream_processing_client{v1::uri{uri.to_string()}, v1::client::options{options}}} {}

    ///
    /// Returns a handle for managing stream processors in this workspace.
    ///
    v_noabi::stream_processors stream_processors() {
        return from_v1(_client.stream_processors());
    }

   private:
    v1::stream_processing_client _client;
};

inline v_noabi::stream_processing_client from_v1(v1::stream_processing_client v) {
    return {std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::stream_processing_client.
///
/// @par Includes
/// - @ref mongocxx/v1/stream_processing_client.hpp
///
