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
#include <string>

#include "driver/base/collection.hpp"
#include "driver/base/write_concern.hpp"
#include "driver/base/read_preference.hpp"

namespace mongo {
namespace driver {

class client;

/// The database class serves as a representation of a MongoDB database. It acts as a gateway
/// for accessing collections that are contained within the particular database that an instance
/// of the class represents. It inherits all of its default settings from the client that calls
/// it's constructor.
class LIBMONGOCXX_EXPORT database {

    // TODO: iterable for collections in the database
    // TODO: make copyable when c-driver supports this
    // TODO: add auth functions (add_user, remove_all_users, remove_user)
   public:
    database(database&& other) noexcept;
    database& operator=(database&& rhs) noexcept;

    ~database();

    bson::document::value command(bson::document::view command);

    class collection create_collection(const std::string& name, bson::document::view options);

    void drop();

    bool has_collection(const std::string& name);

    cursor list_collections();

    const std::string& name() const;

    // TODO: move this next to write concern
    void read_preference(class read_preference rp);
    class read_preference read_preference() const;

    // TODO: should this be called move?
    void rename(
        const std::string& new_name,
        bool drop_target_before_rename
    );

    bson::document::value stats();

    void write_concern(class write_concern wc);
    class write_concern write_concern() const;

    class collection collection(const std::string& name) const;
    inline class collection operator[](const std::string& name) const;

   private:
    friend class client;
    friend class collection;

    database(const class client& client, const std::string& name);

    class impl;
    std::unique_ptr<impl> _impl;

}; // class database

inline collection database::operator[](const std::string& name) const {
    return collection(name);
}

}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
