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

#include <mongo/driver/database.hpp>

#include <mongo/driver/client.hpp>
#include <mongo/driver/private/database.hpp>
#include <mongo/driver/private/client.hpp>
#include <mongo/driver/private/read_preference.hpp>
#include <mongo/driver/private/libmongoc.hpp>

#include <mongo/bson/stdx/make_unique.hpp>

namespace mongo {
namespace driver {

database::database(database&&) noexcept = default;
database& database::operator=(database&&) noexcept = default;

database::~database() = default;

database::database(const class client& client, const std::string& name)
    : _impl(stdx::make_unique<impl>(
          libmongoc::client_get_database(client._impl->client_t, name.c_str()), client._impl.get(),
          name.c_str())) {
}

const std::string& database::name() const {
    return _impl->name;
}

void database::read_preference(class read_preference rp) {
    libmongoc::database_set_read_prefs(_impl->database_t, rp._impl->read_preference_t);
}

class read_preference database::read_preference() const {
    class read_preference rp(stdx::make_unique<read_preference::impl>(
        libmongoc::read_prefs_copy(libmongoc::database_get_read_prefs(_impl->database_t))));
    return rp;
}

void database::write_concern(class write_concern wc) {
    libmongoc::database_set_write_concern(_impl->database_t, wc._impl->write_concern_t);
}

class write_concern database::write_concern() const {
    class write_concern wc(stdx::make_unique<write_concern::impl>(
        libmongoc::write_concern_copy(libmongoc::database_get_write_concern(_impl->database_t))));
    return wc;
}

collection database::collection(const std::string& name) const {
    return mongo::driver::collection(*this, name);
}

}  // namespace driver
}  // namespace mongo
