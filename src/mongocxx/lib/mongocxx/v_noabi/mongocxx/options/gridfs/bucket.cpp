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

#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/options/gridfs/bucket.hpp>
#include <mongocxx/read_concern.hpp>
#include <mongocxx/read_preference.hpp>
#include <mongocxx/write_concern.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {
namespace gridfs {

bucket& bucket::bucket_name(std::string bucket_name) {
    _bucket_name = bucket_name;
    return *this;
}

bsoncxx::v_noabi::stdx::optional<std::string> const& bucket::bucket_name() const {
    return _bucket_name;
}

bucket& bucket::chunk_size_bytes(std::int32_t chunk_size_bytes) {
    _chunk_size_bytes = chunk_size_bytes;
    return *this;
}

bsoncxx::v_noabi::stdx::optional<std::int32_t> const& bucket::chunk_size_bytes() const {
    return _chunk_size_bytes;
}

bucket& bucket::read_concern(mongocxx::v_noabi::read_concern read_concern) {
    _read_concern = read_concern;
    return *this;
}

bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::read_concern> const& bucket::read_concern() const {
    return _read_concern;
}

bucket& bucket::read_preference(mongocxx::v_noabi::read_preference read_preference) {
    _read_preference = std::move(read_preference);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::read_preference> const& bucket::read_preference() const {
    return _read_preference;
}

bucket& bucket::write_concern(mongocxx::v_noabi::write_concern write_concern) {
    _write_concern = std::move(write_concern);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& bucket::write_concern() const {
    return _write_concern;
}

} // namespace gridfs
} // namespace options
} // namespace v_noabi
} // namespace mongocxx
