// Copyright 2017 MongoDB Inc.
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

#include <mongocxx/read_concern.hpp>
#include <mongocxx/read_preference.hpp>
#include <mongocxx/write_concern.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

class MONGOCXX_API session {
   public:
    ///
    /// Sets the write concern for this session.
    ///
    /// @param write_concern
    ///   Object representing the write concern.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called. This facilitates
    ///   method chaining.
    ///
    /// @see
    ///   https://docs.mongodb.com/master/reference/write-concern/
    ///
    session& write_concern(mongocxx::write_concern write_concern);

    ///
    /// Gets the current write concern.
    ///
    /// @return
    ///   The current write concern.
    ///
    const stdx::optional<mongocxx::write_concern>& write_concern() const;

    ///
    /// Sets the read concern for this session.
    ///
    /// @param read_concern
    ///   The new @c read_concern.
    ///
    /// @see
    ///   https://docs.mongodb.com/master/reference/read-concern/
    ///
    /// @return
    ///   A reference to the object on which this member function is being called. This facilitates
    ///   method chaining.
    ///
    session& read_concern(class read_concern rc);

    ///
    /// Returns the current read concern.
    ///
    /// @return
    ///   The current @c read_concern.
    ///
    const stdx::optional<class read_concern>& read_concern() const;

    ///
    /// Sets the read_preference for this session.
    ///
    /// @param read_preference
    ///   The read_preference to set.
    ///
    /// @see
    ///   https://docs.mongodb.com/master/core/read-preference/
    ///
    /// @return
    ///   A reference to the object on which this member function is being called. This facilitates
    ///   method chaining.
    ///
    session& read_preference(class read_preference rp);

    ///
    /// Gets the read_preference.
    ///
    /// @return
    ///   The current read_preference.
    ///
    /// @see
    ///   https://docs.mongodb.com/master/core/read-preference/
    ///
    const stdx::optional<class read_preference>& read_preference() const;

   private:
    friend MONGOCXX_API bool MONGOCXX_CALL operator==(const session&, const session&);
    friend MONGOCXX_API bool MONGOCXX_CALL operator!=(const session&, const session&);

    stdx::optional<class read_preference> _read_pref;
    stdx::optional<class write_concern> _write_concern;
    stdx::optional<class read_concern> _read_concern;
};

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>