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

#include <mongocxx/options/find.hpp>

//

#include <mongocxx/v1/find_options.hh>

#include <utility>

#include <bsoncxx/document/value.hpp>

#include <mongocxx/cursor.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

find::find(v1::find_options opts)
    : _allow_disk_use{opts.allow_disk_use()},
      _allow_partial_results{opts.allow_partial_results()},
      _batch_size{opts.batch_size()},
      _collation{[&]() -> decltype(_collation) {
          if (auto& opt = v1::find_options::internal::collation(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _cursor_type{[&]() -> decltype(_cursor_type) {
          if (auto const& opt = opts.cursor_type()) {
              return static_cast<v_noabi::cursor::type>(*opt);
          }
          return {};
      }()},
      _hint{std::move(v1::find_options::internal::hint(opts))},
      _let{[&]() -> decltype(_let) {
          if (auto& opt = v1::find_options::internal::let(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _comment_option{std::move(v1::find_options::internal::comment(opts))},
      _limit{opts.limit()},
      _max{[&]() -> decltype(_max) {
          if (auto& opt = v1::find_options::internal::max(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _max_await_time{opts.max_await_time()},
      _max_time{opts.max_time()},
      _min{[&]() -> decltype(_min) {
          if (auto& opt = v1::find_options::internal::min(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _no_cursor_timeout{opts.no_cursor_timeout()},
      _projection{[&]() -> decltype(_projection) {
          if (auto& opt = v1::find_options::internal::projection(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _read_preference{std::move(v1::find_options::internal::read_preference(opts))},
      _return_key{opts.return_key()},
      _show_record_id{opts.show_record_id()},
      _skip{opts.skip()},
      _ordering{[&]() -> decltype(_ordering) {
          if (auto& opt = v1::find_options::internal::sort(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()} {}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
