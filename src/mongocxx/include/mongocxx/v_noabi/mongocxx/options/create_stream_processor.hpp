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

#pragma once

#include <mongocxx/options/create_stream_processor-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/create_stream_processor_options.hpp> // IWYU pragma: export

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/view_or_value.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Options for creating a stream processor.
///
class create_stream_processor {
   public:
    create_stream_processor() = default;

    /* explicit(false) */ create_stream_processor(v1::create_stream_processor_options opts)
        : _opts{std::move(opts)} {}

    explicit operator v1::create_stream_processor_options() const {
        return _opts;
    }

    create_stream_processor& dlq(bsoncxx::v_noabi::document::view_or_value v) {
        _opts.dlq(bsoncxx::v1::document::value{v.view()});
        return *this;
    }

    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> dlq() const {
        return _opts.dlq();
    }

    create_stream_processor& stream_meta_field_name(bsoncxx::v_noabi::string::view_or_value v) {
        _opts.stream_meta_field_name(std::string{v.view()});
        return *this;
    }

    bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> stream_meta_field_name() const {
        return _opts.stream_meta_field_name();
    }

    create_stream_processor& tier(bsoncxx::v_noabi::string::view_or_value v) {
        _opts.tier(std::string{v.view()});
        return *this;
    }

    bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> tier() const {
        return _opts.tier();
    }

    create_stream_processor& failover(bool v) {
        _opts.failover(v);
        return *this;
    }

    bsoncxx::v1::stdx::optional<bool> failover() const {
        return _opts.failover();
    }

   private:
    v1::create_stream_processor_options _opts;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::create_stream_processor.
///
/// @par Includes
/// - @ref mongocxx/v1/create_stream_processor_options.hpp
///
