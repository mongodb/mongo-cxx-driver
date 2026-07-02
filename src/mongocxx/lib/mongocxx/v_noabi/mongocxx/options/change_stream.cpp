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

#include <mongocxx/options/change_stream.hh>

//

#include <mongocxx/v1/change_stream.hh>

#include <cstdint>
#include <limits>
#include <utility>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types.hpp>

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

change_stream::change_stream(v1::change_stream::options opts)
    : _full_document{std::move(v1::change_stream::options::internal::full_document(opts))},
      _full_document_before_change{std::move(v1::change_stream::options::internal::full_document_before_change(opts))},
      _batch_size{opts.batch_size()},
      _comment{std::move(v1::change_stream::options::internal::comment(opts))},
      _collation{[&]() -> decltype(_collation) {
          if (auto& opt = v1::change_stream::options::internal::collation(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _resume_after{[&]() -> decltype(_resume_after) {
          if (auto& opt = v1::change_stream::options::internal::resume_after(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _start_after{[&]() -> decltype(_start_after) {
          if (auto& opt = v1::change_stream::options::internal::start_after(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _max_await_time{opts.max_await_time()},
      _start_at_operation_time{[&]() -> decltype(_start_at_operation_time) {
          if (auto const opt = opts.start_at_operation_time()) {
              return bsoncxx::v_noabi::from_v1(*opt);
          }
          return {};
      }()} {}

namespace {

template <typename T>
void append_if(
    bsoncxx::v_noabi::builder::basic::document& doc,
    bsoncxx::v_noabi::stdx::string_view key,
    bsoncxx::v_noabi::stdx::optional<T> const& opt) {
    if (opt) {
        doc.append(bsoncxx::v_noabi::builder::basic::kvp(key, opt.value()));
    }
}

} // namespace

bsoncxx::v_noabi::document::value change_stream::internal::to_document(change_stream const& self) {
    namespace builder = bsoncxx::v_noabi::builder::basic;

    // Construct new bson rep each time since values may change after this is called.
    builder::document out{};

    append_if(out, "fullDocument", self._full_document);
    append_if(out, "fullDocumentBeforeChange", self._full_document_before_change);
    append_if(out, "resumeAfter", self._resume_after);
    append_if(out, "startAfter", self._start_after);
    append_if(out, "batchSize", self._batch_size);
    append_if(out, "collation", self._collation);
    append_if(out, "comment", self._comment);
    append_if(out, "startAtOperationTime", self._start_at_operation_time);

    if (auto const& opt = self._max_await_time) {
        auto const count = opt->count();

        if ((count < 0) || (count >= std::numeric_limits<std::uint32_t>::max())) {
            throw v_noabi::logic_error{v_noabi::error_code::k_invalid_parameter};
        }

        out.append(builder::kvp("maxAwaitTimeMS", static_cast<std::int64_t>(count)));
    }

    return out.extract();
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
