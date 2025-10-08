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

#include <chrono>
#include <memory>

#include <mongocxx/client_session-fwd.hpp>
#include <mongocxx/options/transaction-fwd.hpp>
#include <mongocxx/read_concern-fwd.hpp>
#include <mongocxx/read_preference-fwd.hpp>
#include <mongocxx/write_concern-fwd.hpp>

#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by MongoDB transaction operations.
///
class transaction {
   public:
    MONGOCXX_ABI_EXPORT_CDECL() transaction();

    ///
    /// Copy constructs transaction options.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() transaction(transaction const&);

    ///
    /// Copy assigns transaction options.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(transaction&) operator=(transaction const&);

    ///
    /// Move constructs transaction options.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() transaction(transaction&&) noexcept;

    ///
    /// Move assigns transaction options.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(transaction&) operator=(transaction&&) noexcept;

    ///
    /// Destroys the transaction options.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~transaction() noexcept;

    ///
    /// Sets the transaction read concern.
    ///
    /// @param rc
    ///   The read concern.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(transaction&) read_concern(mongocxx::v_noabi::read_concern const& rc);

    ///
    /// Gets the current transaction read concern.
    ///
    /// @return
    ///    An optional containing the read concern. If the read concern has not been set, a
    ///    disengaged optional is returned.
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::read_concern>)
    read_concern() const;

    ///
    /// Sets the transaction write concern.
    ///
    /// @param wc
    ///   The write concern.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(transaction&)
    write_concern(mongocxx::v_noabi::write_concern const& wc);

    ///
    /// Gets the current transaction write concern.
    ///
    /// @return The write concern.
    ///
    /// @return
    ///    An optional containing the write concern. If the write concern has not been set, a
    ///    disengaged optional is returned.
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern>)
    write_concern() const;

    ///
    /// Sets the transaction read preference.
    ///
    /// @param rp
    ///   The read preference.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(transaction&)
    read_preference(mongocxx::v_noabi::read_preference const& rp);

    ///
    /// Gets the current transaction read preference.
    ///
    /// @return
    ///    An optional containing the read preference. If the read preference has not been set, a
    ///    disengaged optional is returned.
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::read_preference>)
    read_preference() const;

    ///
    /// Sets the transaction's max commit time, in milliseconds.
    ///
    /// @param ms
    ///   The max commit time in milliseconds.
    ///
    /// @return
    ///   A reference to the object on which this function is being called.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(transaction&) max_commit_time_ms(std::chrono::milliseconds ms);

    ///
    /// Gets the current transaction commit time, in milliseconds.
    ///
    /// @return
    ///   An optional containing the timeout. If the max commit time has not been set,
    ///   a disengaged optional is returned.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::chrono::milliseconds>)
    max_commit_time_ms() const;

   private:
    friend ::mongocxx::v_noabi::client_session;

    class impl;

    impl& _get_impl();
    impl const& _get_impl() const;
    std::unique_ptr<impl> _impl;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::transaction.
///
