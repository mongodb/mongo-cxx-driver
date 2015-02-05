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

#include <mongo/driver/config/prelude.hpp>

#include <mongo/bson/document.hpp>
#include <mongo/driver/write_concern.hpp>
#include <mongo/bson/stdx/optional.hpp>

namespace mongo {
namespace driver {
namespace options {

///
/// Class representing the optional arguments to a MongoDB delete operation
///
class LIBMONGOCXX_API delete_options {

   public:

    ///
    /// Sets the write_concern for this operation.
    ///
    /// @param wc
    ///   The new write_concern.
    ///
    /// @see http://docs.mongodb.org/manual/core/write-concern/
    ///
    void write_concern(write_concern wc);

    ///
    /// The current write_concern for this operation.
    ///
    /// @return
    ///   The current write_concern.
    ///
    /// @see http://docs.mongodb.org/manual/core/write-concern/
    ///
    ///
    const stdx::optional<class write_concern>& write_concern() const;

   private:
    stdx::optional<class write_concern> _write_concern;

};

}  // namespace options
}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
