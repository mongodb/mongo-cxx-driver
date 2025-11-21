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

#include <mongocxx/v1/exception.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/document/view.hpp>

#include <system_error>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/export.hh>

namespace mongocxx {
namespace v1 {

class exception::internal {
   public:
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(exception) make(std::error_code ec);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(exception)
    make(int code, std::error_category const& category, char const* message);
    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(exception) make(int code, std::error_category const& category);

    static MONGOCXX_ABI_EXPORT_CDECL_TESTING(void) set_error_labels(exception& self, bsoncxx::v1::document::view v);
};

[[noreturn]] MONGOCXX_ABI_EXPORT_CDECL_TESTING(void) throw_exception(bson_error_t const& error);

[[noreturn]] MONGOCXX_ABI_EXPORT_CDECL_TESTING(void) throw_exception(
    bson_error_t const& error,
    bsoncxx::v1::document::value raw);

[[noreturn]] inline void throw_exception(bson_error_t const& error, bsoncxx::v1::document::view raw) {
    throw_exception(error, bsoncxx::v1::document::value{raw});
}

} // namespace v1
} // namespace mongocxx
