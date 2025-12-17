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

#include <mongocxx/options/bulk_write.hpp>

//

#include <mongocxx/v1/bulk_write.hh>

#include <utility>

#include <bsoncxx/document/value.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

bulk_write::bulk_write(v1::bulk_write::options opts)
    : _ordered{opts.ordered()},
      _write_concern{std::move(v1::bulk_write::options::internal::write_concern(opts))},
      _bypass_document_validation{opts.bypass_document_validation()},
      _let{[&]() -> decltype(_let) {
          if (auto& opt = v1::bulk_write::options::internal::let(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _comment{std::move(v1::bulk_write::options::internal::comment(opts))} {}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
