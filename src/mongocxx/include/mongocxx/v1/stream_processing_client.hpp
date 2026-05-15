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

#include <mongocxx/v1/stream_processing_client-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <mongocxx/v1/stream_processors-fwd.hpp>

#include <mongocxx/v1/client.hpp>
#include <mongocxx/v1/config/export.hpp>
#include <mongocxx/v1/stream_processors.hpp> // IWYU pragma: export
#include <mongocxx/v1/uri.hpp>

namespace mongocxx {
namespace v1 {

///
/// A client connected to an Atlas Stream Processing workspace.
///
/// TLS is required for all workspace connections. If the URI hostname matches the
/// atlas-stream-* pattern and TLS is not explicitly enabled in the URI, TLS is silently
/// enabled. authSource defaults to "admin"; specifying any other authSource throws an exception.
///
class stream_processing_client {
   private:
    class impl;
    void* _impl;

   public:
    MONGOCXX_ABI_EXPORT_CDECL() ~stream_processing_client();
    MONGOCXX_ABI_EXPORT_CDECL() stream_processing_client(stream_processing_client&&) noexcept;
    MONGOCXX_ABI_EXPORT_CDECL(stream_processing_client&) operator=(stream_processing_client&&) noexcept;

    stream_processing_client(stream_processing_client const&) = delete;
    stream_processing_client& operator=(stream_processing_client const&) = delete;

    ///
    /// Constructs a client connected to the given URI.
    ///
    /// @throws mongocxx::v1::exception if the URI specifies authSource other than "admin".
    ///
    MONGOCXX_ABI_EXPORT_CDECL() explicit stream_processing_client(v1::uri uri);

    ///
    /// Constructs a client connected to the given URI with additional client options.
    ///
    /// @throws mongocxx::v1::exception if the URI specifies authSource other than "admin".
    ///
    MONGOCXX_ABI_EXPORT_CDECL() stream_processing_client(v1::uri uri, v1::client::options const& options);

    ///
    /// Returns a handle for managing stream processors in this workspace.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::stream_processors) stream_processors();
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::stream_processing_client.
///
/// @par Includes
/// - @ref mongocxx/v1/stream_processors.hpp
///
