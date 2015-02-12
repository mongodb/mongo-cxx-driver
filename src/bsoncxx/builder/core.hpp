// Copyright 2014 MongoDB Inc.
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

#include <bsoncxx/config/prelude.hpp>

#include <memory>
#include <stdexcept>

#include <bsoncxx/array/value.hpp>
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/types.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {

class BSONCXX_API core {

   public:
    class BSONCXX_PRIVATE impl;

    // TODO this should fit with the larger bson exception heirarchy
    class invalid_state : public std::runtime_error {};

    explicit core(bool is_array);
    core(core&& rhs);
    core& operator=(core&& rhs);
    ~core();

    void key_literal(const char *key, std::size_t len);
    void key_owning(std::string key);

    void open_document();
    void open_array();
    void close_document();
    void close_array();

    void concatenate(const document::view& view);

    void append(const types::b_double& value);
    void append(const types::b_utf8& value);
    void append(const types::b_document& value);
    void append(const types::b_array& value);
    void append(const types::b_binary& value);
    void append(const types::b_undefined& value);
    void append(const types::b_oid& value);
    void append(const types::b_bool& value);
    void append(const types::b_date& value);
    void append(const types::b_null& value);
    void append(const types::b_regex& value);
    void append(const types::b_dbpointer& value);
    void append(const types::b_code& value);
    void append(const types::b_symbol& value);
    void append(const types::b_codewscope& value);
    void append(const types::b_int32& value);
    void append(const types::b_timestamp& value);
    void append(const types::b_int64& value);
    void append(const types::b_minkey& value);
    void append(const types::b_maxkey& value);

    void append(const types::value& value);

    template <std::size_t n>
    void append(const char (&v)[n]) {
        append(types::b_utf8{v});
    }

    void append(std::string str);
    void append(bool value);
    void append(double value);
    void append(std::int32_t value);
    void append(std::int64_t value);
    void append(const oid& value);

    document::view view_document() const;
    array::view view_array() const;
    document::value extract_document();
    array::value extract_array();

    void clear();

   private:
    std::unique_ptr<impl> _impl;
};

}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
