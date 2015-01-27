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

#include "driver/config/prelude.hpp"

#include "stdx/optional.hpp"

namespace mongo {
namespace driver {
namespace options {

///
/// Class representing the optional arguments to a MongoDB bulk write
///
class LIBMONGOCXX_EXPORT bulk_write {

   public:

    ///
    /// Sets whether the writes must be executed in order by the server.
    ///
    /// The server-side default is @c true.
    ///
    /// @param ordered
    ///   If @c true all write operations will be executed serially in the order they were appended,
    ///   and the entire bulk operation will abort on the first error. If @c false operations will
    ///   be executed in arbitrary order (possibly in parallel on the server) and any errors will be
    ///   reported after attempting all operations.
    ///
    void ordered(bool ordered);

    ///
    /// Gets the current value of the ordered option.
    ///
    /// @return The optional value of the ordered option.
    ///
    const optional<bool>& ordered() const;

    ///
    /// Sets the write_concern for this operation.
    ///
    /// @param wc
    ///   The new write_concern.
    ///
    /// @see http://docs.mongodb.org/manual/core/write-concern/
    ///
    void write_concern(class write_concern wc);

    ///
    /// The current write_concern for this operation.
    ///
    /// @return
    ///   The current write_concern.
    ///
    /// @see http://docs.mongodb.org/manual/core/write-concern/
    ///
    const optional<class write_concern>& write_concern() const;

   private:
    optional<bool> _ordered;
    optional<class write_concern> _write_concern;

};

}  // namespace options
}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
