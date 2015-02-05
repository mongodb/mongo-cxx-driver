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

#include <mongo/driver/model/write.hpp>

namespace mongo {
namespace driver {
namespace model {

write::write(insert_one value)
    : _type(write_type::k_insert_one), _insert_one(std::move(value)) {
}
write::write(delete_one value)
    : _type(write_type::k_delete_one), _delete_one(std::move(value)) {
}
write::write(delete_many value)
    : _type(write_type::k_delete_many), _delete_many(std::move(value)) {
}
write::write(update_one value)
    : _type(write_type::k_update_one), _update_one(std::move(value)) {
}
write::write(update_many value)
    : _type(write_type::k_update_many), _update_many(std::move(value)) {
}
write::write(replace_one value)
    : _type(write_type::k_replace_one), _replace_one(std::move(value)) {
}

write::write(write&& rhs) : _type(write_type::k_uninitialized) {
    *this = std::move(rhs);
}

void write::destroy_member() {
    switch (_type) {
        case write_type::k_insert_one:
            _insert_one.~insert_one();
            break;
        case write_type::k_update_one:
            _update_one.~update_one();
            break;
        case write_type::k_update_many:
            _update_many.~update_many();
            break;
        case write_type::k_delete_one:
            _delete_one.~delete_one();
            break;
        case write_type::k_delete_many:
            _delete_many.~delete_many();
            break;
        case write_type::k_replace_one:
            _replace_one.~replace_one();
            break;
        case write_type::k_uninitialized:
            break;
    }

    _type = write_type::k_uninitialized;
}

write& write::operator=(write&& rhs) {
    destroy_member();

    switch (rhs._type) {
        case write_type::k_insert_one:
            _insert_one = std::move(rhs._insert_one);
            break;
        case write_type::k_update_one:
            _update_one = std::move(rhs._update_one);
            break;
        case write_type::k_update_many:
            _update_many = std::move(rhs._update_many);
            break;
        case write_type::k_delete_one:
            _delete_one = std::move(rhs._delete_one);
            break;
        case write_type::k_delete_many:
            _delete_many = std::move(rhs._delete_many);
            break;
        case write_type::k_replace_one:
            _replace_one = std::move(rhs._replace_one);
            break;
        case write_type::k_uninitialized:
            break;
    }

    _type = rhs._type;

    return *this;
}

write_type write::type() const {
    return _type;
}

const insert_one& write::get_insert_one() const {
    return _insert_one;
}
const update_one& write::get_update_one() const {
    return _update_one;
}
const update_many& write::get_update_many() const {
    return _update_many;
}
const delete_one& write::get_delete_one() const {
    return _delete_one;
}
const delete_many& write::get_delete_many() const {
    return _delete_many;
}
const replace_one& write::get_replace_one() const {
    return _replace_one;
}

write::~write() {
    destroy_member();
}

}  // namespace model
}  // namespace driver
}  // namespace mongo
