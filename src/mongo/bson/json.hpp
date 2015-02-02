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

#include <iostream>
#include <iomanip>
#include <vector>
#include "bson/types.hpp"
#include "bson/builder.hpp"

extern "C" {
#include "bson/util/b64_ntop.h"
}

namespace bson {

class json_visitor {
   public:
    json_visitor(std::ostream& out, bool is_array, std::size_t padding)
        : out(out), stack({is_array}), padding(padding) {}

    void visit_key(const string_or_literal& value) {
        pad();

        if (!stack.back()) {
            out << "\"" << value.c_str() << "\""
                << " : ";
        }
    }

    void visit_value(const types::b_eod&) {}

    void visit_value(const types::b_double& value) { out << value.value; }

    void visit_value(const types::b_utf8& value) { out << "\"" << value.value.c_str() << "\""; }

    void visit_value(const types::b_document& value) {
        out << "{" << std::endl;
        stack.push_back(false);
        visit_children(value.value);
        pad();
        out << "}";
    }

    void visit_value(const types::b_array& value) {
        out << "[" << std::endl;
        stack.push_back(true);
        visit_children(value.value);
        pad();
        out << "]";
    }

    void visit_value(const types::b_binary& value) {

        std::size_t b64_len;

        b64_len = (value.size / 3 + 1) * 4 + 1;
        std::unique_ptr<char> b64(reinterpret_cast<char*>(operator new(b64_len)));
        b64_ntop(value.bytes, value.size, b64.get(), b64_len);

        out << "{" << std::endl;
        pad(1);
        out << "\"$type\" : " << value.sub_type << "," << std::endl;
        pad(1);
        out << "\"$binary\" : " << b64.get() << "," << std::endl;
        pad();
        out << "}";
    }

    void visit_value(const types::b_undefined&) {

        out << "{" << std::endl;
        pad(1);
        out << "\"$undefined\" : true" << std::endl;
        pad();
        out << "}";
    }

    void visit_value(const types::b_oid& value) {

        out << "{" << std::endl;
        pad(1);
        out << "\"$oid\" : \"" << value.value << "\"" << std::endl;
        pad();
        out << "}";
    }

    void visit_value(const types::b_bool& value) { out << (value.value ? "true" : "false"); }

    void visit_value(const types::b_date& value) {

        out << "{" << std::endl;
        pad(1);
        out << "\"$date\" : " << value.value << std::endl;
        pad();
        out << "}";
    }

    void visit_value(const types::b_null&) { out << "null"; }

    void visit_value(const types::b_regex& value) {

        out << "{" << std::endl;
        pad(1);
        out << "\"$regex\" : \"" << value.regex.c_str() << "\"," << std::endl;
        pad();
        out << "\"$options\" : \"" << value.options.c_str() << "\"" << std::endl;
        pad();
        out << "}";
    }

    void visit_value(const types::b_dbpointer& value) {

        out << "{" << std::endl;
        pad(1);
        out << "\"$ref\" : \"" << value.collection << "\"";

        if (value.value) {
            out << "," << std::endl;
            pad();
            out << "\"$id\" : \"" << value.value.to_string().c_str() << "\"" << std::endl;
        }

        pad();
        out << "}";
    }

    void visit_value(const types::b_code& value) { out << value.code.c_str(); }

    void visit_value(const types::b_symbol& value) { out << value.symbol.c_str(); }

    void visit_value(const types::b_codewscope& value) { out << value.code.c_str(); }

    void visit_value(const types::b_int32& value) { out << value.value; }

    void visit_value(const types::b_timestamp& value) {

        out << "{" << std::endl;
        pad(1);
        out << "\"$timestamp\" : {" << std::endl;
        pad(2);
        out << "\"$t\" : " << value.timestamp << "," << std::endl;
        pad(2);
        out << "\"$i\" : " << value.increment << std::endl;
        pad(1);
        out << "}";
        pad();
        out << "}";
    }

    void visit_value(const types::b_int64& value) { out << value.value; }

    void visit_value(const types::b_minkey&) {

        out << "{" << std::endl;
        pad(1);
        out << "\"$minKey\" : 1" << std::endl;
        pad();
        out << "}";
    }

    void visit_value(const types::b_maxkey&) {

        out << "{" << std::endl;
        pad(1);
        out << "\"$maxKey\" : 1" << std::endl;
        pad();
        out << "}";
    }

   private:
    std::ostream& out;
    std::vector<bool> stack;
    std::size_t padding;

    void pad(std::size_t extra = 0) {
        out << std::setw((stack.size() + -1 + extra + padding) * 4) << "" << std::setw(0);
    }

    void visit_children(const document::view& view) {
        bool first = true;
        for (auto&& x : view) {
            if (!first) {
                out << ", " << std::endl;
            }
            first = false;
            visit_key(x.key());
            switch (static_cast<int>(x.type())) {
#define MONGOCXX_ENUM(name, val)     \
    case val:                        \
        visit_value(x.get_##name()); \
        break;
#include "bson/enums/type.hpp"
#undef MONGOCXX_ENUM
            }
        }
        out << std::endl;
        stack.pop_back();
    }
};

}  // namespace bson

#include "driver/config/postlude.hpp"
