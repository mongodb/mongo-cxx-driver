// Copyright 2014 MongoDB Inc.
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

#include <string>

#include <mongocxx/options/ssl.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

// NOTE: client options interface still evolving

///
/// Class representing the optional arguments to a MongoDB driver client object.
///
class MONGOCXX_API client {
   public:
    ///
    /// Sets the SSL-related options.
    ///
    /// @param ssl_opts
    ///   The SSL related options.
    ///
    client& ssl_opts(ssl ssl_opts);

    ///
    /// The current SSL-related options.
    ///
    /// @return The SSL-related options.
    ///
    const stdx::optional<ssl>& ssl_opts() const;

   private:
    stdx::optional<ssl> _ssl_opts;
};

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
