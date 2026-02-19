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

#include <mongocxx/options/gridfs/bucket.hpp>

//

#include <mongocxx/v1/gridfs/bucket.hh>

#include <utility>

#include <bsoncxx/stdx/optional.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {
namespace gridfs {

bucket::bucket(v1::gridfs::bucket::options opts)
    : _bucket_name{std::move(v1::gridfs::bucket::options::internal::bucket_name(opts))},
      _chunk_size_bytes{opts.chunk_size_bytes()},
      _read_concern{std::move(v1::gridfs::bucket::options::internal::read_concern(opts))},
      _read_preference{std::move(v1::gridfs::bucket::options::internal::read_preference(opts))},
      _write_concern{std::move(v1::gridfs::bucket::options::internal::write_concern(opts))} {}

} // namespace gridfs
} // namespace options
} // namespace v_noabi
} // namespace mongocxx
