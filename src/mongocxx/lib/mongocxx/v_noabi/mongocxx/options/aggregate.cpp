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

#include <mongocxx/options/aggregate.hh>

//

#include <bsoncxx/v1/types/value.hh>

#include <mongocxx/v1/aggregate_options.hh>

#include <cstdint>
#include <stdexcept>
#include <utility>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <mongocxx/scoped_bson.hh>

#include <bsoncxx/private/bson.hh>

namespace mongocxx {
namespace v_noabi {
namespace options {

aggregate::aggregate(v1::aggregate_options opts)
    : _allow_disk_use{opts.allow_disk_use()},
      _batch_size{opts.batch_size()},
      _collation{[&]() -> decltype(_collation) {
          if (auto& opt = v1::aggregate_options::internal::collation(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _let{[&]() -> decltype(_let) {
          if (auto& opt = v1::aggregate_options::internal::let(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _max_time{opts.max_time()},
      _read_preference{std::move(v1::aggregate_options::internal::read_preference(opts))},
      _bypass_document_validation{opts.bypass_document_validation()},
      _hint{std::move(v1::aggregate_options::internal::hint(opts))},
      _write_concern{std::move(v1::aggregate_options::internal::write_concern(opts))},
      _read_concern{std::move(v1::aggregate_options::internal::read_concern(opts))},
      _comment{std::move(v1::aggregate_options::internal::comment(opts))} {}

void aggregate::internal::append_to(aggregate const& self, scoped_bson& doc) {
    if (auto const& opt = self.allow_disk_use()) {
        doc += scoped_bson{BCON_NEW("allowDiskUse", BCON_BOOL(*opt))};
    }

    if (auto const& opt = self.collation()) {
        doc += scoped_bson{BCON_NEW("collation", BCON_DOCUMENT(to_scoped_bson_view(*opt).bson()))};
    }

    if (auto const& opt = self.let()) {
        doc += scoped_bson{BCON_NEW("let", BCON_DOCUMENT(to_scoped_bson_view(*opt).bson()))};
    }

    if (auto const& opt = self.max_time()) {
        doc += scoped_bson{BCON_NEW("maxTimeMS", BCON_INT64(std::int64_t{opt->count()}))};
    }

    if (auto const& opt = self.bypass_document_validation()) {
        doc += scoped_bson{BCON_NEW("bypassDocumentValidation", BCON_BOOL(*opt))};
    }

    if (auto const& opt = self.hint()) {
        scoped_bson v;

        if (!BSON_APPEND_VALUE(
                v.inout_ptr(),
                "hint",
                &bsoncxx::v1::types::value::internal::get_bson_value(
                    bsoncxx::v1::types::value{bsoncxx::v_noabi::to_v1(opt->to_value())}))) {
            throw std::logic_error{"mongocxx::v1::aggregate_options::internal::to_document: BSON_APPEND_VALUE failed"};
        }

        doc += v;
    }

    if (auto const& opt = self.read_concern()) {
        doc += scoped_bson{BCON_NEW("readConcern", BCON_DOCUMENT(to_scoped_bson(opt->to_document()).bson()))};
    }

    if (auto const& opt = self.write_concern()) {
        doc += scoped_bson{BCON_NEW("writeConcern", BCON_DOCUMENT(to_scoped_bson(opt->to_document()).bson()))};
    }

    if (auto const& opt = self.batch_size()) {
        doc += scoped_bson{BCON_NEW("batchSize", BCON_INT32(*opt))};
    }

    if (auto const& opt = self.comment()) {
        scoped_bson v;

        if (!BSON_APPEND_VALUE(
                v.inout_ptr(),
                "comment",
                &bsoncxx::v1::types::value::internal::get_bson_value(
                    bsoncxx::v1::types::value{bsoncxx::v_noabi::to_v1(*opt)}))) {
            throw std::logic_error{"mongocxx::v1::aggregate_options::internal::to_document: BSON_APPEND_VALUE failed"};
        }

        doc += v;
    }
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
