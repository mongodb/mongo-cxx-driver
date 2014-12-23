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

#include <memory>

#include "driver/base/database.hpp"
#include "driver/base/read_preference.hpp"
#include "driver/base/uri.hpp"
#include "driver/base/write_concern.hpp"
#include "driver/options/client.hpp"

namespace mongo {
namespace driver {

/// The client class is the entry point into the MongoDB driver. It acts as a logical gateway for
/// accessing the databases of MongoDB clusters. Databases that are accessed via a client inherit
/// all of the options specified on the client.
class LIBMONGOCXX_EXPORT client {

    // TODO: iterable for databases on the server
    // TODO: add + implement client api methods
   public:
    client(
        const uri& mongodb_uri = uri(),
        const options::client& options = options::client()
    );

    client(client&& rhs) noexcept;
    client& operator=(client&& rhs) noexcept;

    ~client();

    // TODO: document that modifications at this level do not affect existing clients + databases
    void read_preference(class read_preference rp);
    class read_preference read_preference() const;

    // TODO: document that modifications at this level do not affect existing clients + databases
    void write_concern(class write_concern wc);
    class write_concern write_concern() const;

    class database database(const std::string& name) const &;
    class database database(const std::string& name) const && = delete;

    inline class database operator[](const std::string& name) const &;
    inline class database operator[](const std::string& name) const && = delete;

   private:
    friend class database;
    friend class collection;

    class impl;
    std::unique_ptr<impl> _impl;

}; // class client

inline class database client::operator[](const std::string& name) const & {
    return database(name);
}

}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
