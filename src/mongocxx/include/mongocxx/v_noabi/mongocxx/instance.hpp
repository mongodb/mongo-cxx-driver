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

#include <mongocxx/instance-fwd.hpp>

//

#include <mongocxx/v1/instance.hpp> // IWYU pragma: export

#include <memory>

#include <mongocxx/logger-fwd.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

/// @copydoc mongocxx::v1::instance
class instance {
   public:
    ///
    /// Creates an instance of the driver.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() instance();

    ///
    /// Creates an instance of the driver with a user provided log handler.
    ///  @param logger The logger that the driver will direct log messages to.
    ///
    /// @throws mongocxx::v_noabi::logic_error if an instance already exists.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() instance(std::unique_ptr<v_noabi::logger> logger);

    ///
    /// Move constructs an instance of the driver.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() instance(instance&& other) noexcept;

    ///
    /// Move assigns an instance of the driver.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(instance&) operator=(instance&& other) noexcept;

    ///
    /// Destroys an instance of the driver.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~instance();

    ///
    /// This class is not copyable.
    ///
    instance(instance const&) = delete;

    ///
    /// This class is not copyable.
    ///
    instance& operator=(instance const&) = delete;

    ///
    /// @warning For internal use only!
    ///
    static MONGOCXX_ABI_EXPORT_CDECL(instance&) current();

   private:
    class impl;
    std::unique_ptr<impl> _impl;
};

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::instance.
///
/// @par Includes
/// - @ref mongocxx/v1/instance.hpp
///
