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

#include "driver/config/prelude.hpp"

#include <memory>

#include "bson/document.hpp"
#include "bson/types.hpp"
#include "bson/string_or_literal.hpp"

namespace bson {
namespace builder {

class LIBMONGOCXX_EXPORT concrete {

   public:
    class impl;

    class invalid_state : public std::runtime_error {};

    concrete(bool is_array);
    concrete(concrete&& rhs);
    concrete& operator=(concrete&& rhs);
    ~concrete();

    void key_append(string_or_literal key);

    void open_doc_append();
    void open_array_append();
    void close_doc_append();
    void close_array_append();
    void concat_append(const document::view& view);

    void value_append(const types::b_double& value);
    void value_append(const types::b_utf8& value);
    void value_append(const types::b_document& value);
    void value_append(const types::b_array& value);
    void value_append(const types::b_binary& value);
    void value_append(const types::b_undefined& value);
    void value_append(const types::b_oid& value);
    void value_append(const types::b_bool& value);
    void value_append(const types::b_date& value);
    void value_append(const types::b_null& value);
    void value_append(const types::b_regex& value);
    void value_append(const types::b_dbpointer& value);
    void value_append(const types::b_code& value);
    void value_append(const types::b_symbol& value);
    void value_append(const types::b_codewscope& value);
    void value_append(const types::b_int32& value);
    void value_append(const types::b_timestamp& value);
    void value_append(const types::b_int64& value);
    void value_append(const types::b_minkey& value);
    void value_append(const types::b_maxkey& value);

    void value_append(const document::element& value);

    void value_append(string_or_literal value);

    template <std::size_t n>
    void value_append(const char (&v)[n]) {
        value_append(string_or_literal{v, n - 1});
    }

    void value_append(bool value);
    void value_append(double value);
    void value_append(std::int32_t value);
    void value_append(std::int64_t value);
    void value_append(const oid& value);

    document::view view() const;
    operator document::view() const;
    document::value extract();

    void clear();

   private:
    std::unique_ptr<impl> _impl;
};

}  // namespace builder
}  // namespace bson

#include "driver/config/postlude.hpp"
