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

#include <mongocxx/stream_processors-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/stream_processors.hpp> // IWYU pragma: export

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/string/view_or_value.hpp>

#include <mongocxx/options/create_stream_processor.hpp>
#include <mongocxx/stream_processor.hpp>
#include <mongocxx/stream_processor_info.hpp>

#include <mongocxx/config/prelude.hpp>

#include <utility>

namespace mongocxx {
namespace v_noabi {

///
/// A handle for managing stream processors in a stream processing workspace.
///
class stream_processors {
   public:
    /* explicit(false) */ stream_processors(v1::stream_processors procs) : _procs{std::move(procs)} {}

    explicit operator v1::stream_processors() && {
        return std::move(_procs);
    }

    void create(bsoncxx::v_noabi::string::view_or_value name,
                bsoncxx::v_noabi::array::view pipeline,
                v_noabi::options::create_stream_processor const& options = {}) {
        _procs.create(std::string{name.view()}, pipeline, v1::create_stream_processor_options{options});
    }

    v_noabi::stream_processor get(bsoncxx::v_noabi::string::view_or_value name) {
        return from_v1(_procs.get(std::string{name.view()}));
    }

    v_noabi::stream_processor_info get_info(bsoncxx::v_noabi::string::view_or_value name) {
        return from_v1(_procs.get_info(std::string{name.view()}));
    }

   private:
    v1::stream_processors _procs;
};

inline v_noabi::stream_processors from_v1(v1::stream_processors v) {
    return {std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::stream_processors.
///
/// @par Includes
/// - @ref mongocxx/v1/stream_processors.hpp
///
