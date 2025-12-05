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

#include <mongocxx/options/update.hpp>

//

#include <mongocxx/v1/update_many_options.hh>
#include <mongocxx/v1/update_one_options.hh>

#include <utility>

#include <bsoncxx/array/value.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/stdx/optional.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

update::update(v1::update_many_options opts)
    : _bypass_document_validation{opts.bypass_document_validation()},
      _collation{[&]() -> decltype(_collation) {
          if (auto& opt = v1::update_many_options::internal::collation(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _upsert{opts.upsert()},
      _write_concern{std::move(v1::update_many_options::internal::write_concern(opts))},
      _array_filters{[&]() -> decltype(_array_filters) {
          if (auto& opt = v1::update_many_options::internal::array_filters(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _hint{std::move(v1::update_many_options::internal::hint(opts))},
      _let{[&]() -> decltype(_let) {
          if (auto& opt = v1::update_many_options::internal::let(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _sort{[&]() -> decltype(_sort) {
          if (auto& opt = v1::update_many_options::internal::sort(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _comment{std::move(v1::update_many_options::internal::comment(opts))} {}

update::update(v1::update_one_options opts)
    : _bypass_document_validation{opts.bypass_document_validation()},
      _collation{[&]() -> decltype(_collation) {
          if (auto& opt = v1::update_one_options::internal::collation(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _upsert{opts.upsert()},
      _write_concern{std::move(v1::update_one_options::internal::write_concern(opts))},
      _array_filters{[&]() -> decltype(_array_filters) {
          if (auto& opt = v1::update_one_options::internal::array_filters(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _hint{std::move(v1::update_one_options::internal::hint(opts))},
      _let{[&]() -> decltype(_let) {
          if (auto& opt = v1::update_one_options::internal::let(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _sort{[&]() -> decltype(_sort) {
          if (auto& opt = v1::update_one_options::internal::sort(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _comment{std::move(v1::update_one_options::internal::comment(opts))} {}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
