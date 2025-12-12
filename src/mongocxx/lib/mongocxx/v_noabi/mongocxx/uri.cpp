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

#include <mongocxx/uri.hh>

//

#include <mongocxx/v1/exception.hpp>

#include <mongocxx/v1/uri.hh>

#include <string>

#include <bsoncxx/string/view_or_value.hpp>

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/logic_error.hpp>

#include <mongocxx/mongoc_error.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {

// NOLINTNEXTLINE(cert-err58-cpp): v_noabi backward compatibility.
std::string const uri::k_default_uri = "mongodb://localhost:27017";

uri::uri(bsoncxx::v_noabi::string::view_or_value uri_string) try : _uri{uri_string.view()} {
} catch (v1::exception const& ex) {
    throw_exception<v_noabi::logic_error>(ex);
}

void uri::server_selection_try_once(bool val) try {
    _uri.server_selection_try_once(val);
} catch (v1::exception const&) {
    throw v_noabi::exception{v_noabi::error_code::k_invalid_uri, "failed to set 'serverSelectionTryOnce' option"};
}

mongoc_uri_t const* uri::internal::as_mongoc(uri const& self) {
    return v1::uri::internal::as_mongoc(self._uri);
}

} // namespace v_noabi
} // namespace mongocxx
