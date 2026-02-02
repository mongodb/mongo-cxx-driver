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

#include <mongocxx/options/index_view.hpp>

//

#include <mongocxx/v1/indexes.hh>

#include <string>
#include <utility>

#include <bsoncxx/document/value.hpp>

#include <mongocxx/scoped_bson.hh>

#include <bsoncxx/private/bson.hh>

namespace mongocxx {
namespace v_noabi {
namespace options {

index_view::index_view(v1::indexes::options opts)
    : _max_time{opts.max_time()},
      _write_concern{std::move(v1::indexes::options::internal::write_concern(opts))},
      _commit_quorum{[&]() -> decltype(_commit_quorum) {
          if (auto& opt = v1::indexes::options::internal::commit_quorum(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }

          return {};
      }()} {}

index_view& index_view::commit_quorum(int commit_quorum) {
    _commit_quorum =
        bsoncxx::v_noabi::from_v1(scoped_bson{BCON_NEW("commitQuorum", BCON_INT32(commit_quorum))}.value());
    return *this;
}

index_view& index_view::commit_quorum(std::string commit_quorum) {
    _commit_quorum =
        bsoncxx::v_noabi::from_v1(scoped_bson{BCON_NEW("commitQuorum", BCON_UTF8(commit_quorum.c_str()))}.value());
    return *this;
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
