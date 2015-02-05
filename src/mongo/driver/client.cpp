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

#include <mongo/driver/client.hpp>

#include <mongo/driver/private/client.hpp>
#include <mongo/driver/private/read_preference.hpp>
#include <mongo/driver/private/write_concern.hpp>
#include <mongo/driver/private/uri.hpp>
#include <mongo/bson/stdx/make_unique.hpp>

namespace mongo {
namespace driver {

client::client(const uri& uri, const options::client&)
    : _impl(stdx::make_unique<impl>(libmongoc::client_new_from_uri(uri._impl->uri_t))) {
}

client::client(client&&) noexcept = default;
client& client::operator=(client&&) noexcept = default;

client::~client() = default;

void client::read_preference(class read_preference rp) {
    libmongoc::client_set_read_prefs(_impl->client_t, rp._impl->read_preference_t);
}

class read_preference client::read_preference() const {
    class read_preference rp(stdx::make_unique<read_preference::impl>(
        libmongoc::read_prefs_copy(libmongoc::client_get_read_prefs(_impl->client_t))));
    return rp;
}

void client::write_concern(class write_concern wc) {
    libmongoc::client_set_write_concern(_impl->client_t, wc._impl->write_concern_t);
}

class write_concern client::write_concern() const {
    class write_concern wc(stdx::make_unique<write_concern::impl>(
        libmongoc::write_concern_copy(libmongoc::client_get_write_concern(_impl->client_t))));
    return wc;
}

class database client::database(const std::string& name) const & {
    return mongo::driver::database(*this, name);
}

}  // namespace driver
}  // namespace mongo
