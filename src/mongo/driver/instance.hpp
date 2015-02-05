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

#include <memory>

namespace mongo {
namespace driver {

///
/// Class representing an instance of the MongoDB driver.
///
/// Life cycle: An instance of the driver *MUST* be kept around.
///
class LIBMONGOCXX_API instance {

   public:
    ///
    /// Creates an instance of the driver.
    ///
    instance();

    ///
    /// Move constructs an instance of the driver.
    ///
    instance(instance&&) noexcept;

    ///
    /// Move assigns an instance of the driver.
    ///
    instance& operator=(instance&&) noexcept;

    ///
    /// Destroys an instance of the driver.
    ///
    ~instance();

   private:
    class impl;
    std::unique_ptr<impl> _impl;

};

}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
