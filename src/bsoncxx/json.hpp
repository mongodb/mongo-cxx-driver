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

#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>

#include <bsoncxx/b64_ntop.h>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

class json_visitor {
   public:
    BSONCXX_INLINE json_visitor(std::ostream& out, bool is_array, std::size_t padding)
        : out(out), stack({is_array}), padding(padding) {}

    BSONCXX_INLINE void visit_key(stdx::string_view value) {
        pad();

        if (!stack.back()) {
            out << "\"" << value.data() << "\""
                << " : ";
        }
    }

    BSONCXX_INLINE void visit_value(const types::b_eod&) {}

    BSONCXX_INLINE void visit_value(const types::b_double& value) { out << value.value; }

    BSONCXX_INLINE void visit_value(const types::b_utf8& value) { out << "\"" << value.value.data() << "\""; }

    BSONCXX_INLINE void visit_value(const types::b_document& value) {
        out << "{" << std::endl;
        stack.push_back(false);
        visit_children(value.value);
        pad();
        out << "}";
    }

    BSONCXX_INLINE void visit_value(const types::b_array& value) {
        out << "[" << std::endl;
        stack.push_back(true);
        visit_children(value.value);
        pad();
        out << "]";
    }

    BSONCXX_INLINE void visit_value(const types::b_binary& value) {

        std::size_t b64_len;

        b64_len = (value.size / 3 + 1) * 4 + 1;
        std::unique_ptr<char> b64(reinterpret_cast<char*>(operator new(b64_len)));
        b64::ntop(value.bytes, value.size, b64.get(), b64_len);

        out << "{" << std::endl;
        pad(1);
        out << "\"$type\" : " << to_string(value.sub_type) << "," << std::endl;
        pad(1);
        out << "\"$binary\" : " << b64.get() << "," << std::endl;
        pad();
        out << "}";
    }

    BSONCXX_INLINE void visit_value(const types::b_undefined&) {

        out << "{" << std::endl;
        pad(1);
        out << "\"$undefined\" : true" << std::endl;
        pad();
        out << "}";
    }

    BSONCXX_INLINE void visit_value(const types::b_oid& value) {

        out << "{" << std::endl;
        pad(1);
        out << "\"$oid\" : \"" << value.value << "\"" << std::endl;
        pad();
        out << "}";
    }

    BSONCXX_INLINE void visit_value(const types::b_bool& value) {
        out << (value.value ? "true" : "false");
    }

    BSONCXX_INLINE void visit_value(const types::b_date& value) {

        out << "{" << std::endl;
        pad(1);
        out << "\"$date\" : " << value.value << std::endl;
        pad();
        out << "}";
    }

    BSONCXX_INLINE void visit_value(const types::b_null&) { out << "null"; }

    BSONCXX_INLINE void visit_value(const types::b_regex& value) {

        out << "{" << std::endl;
        pad(1);
        out << "\"$regex\" : \"" << value.regex.data() << "\"," << std::endl;
        pad();
        out << "\"$options\" : \"" << value.options.data() << "\"" << std::endl;
        pad();
        out << "}";
    }

    BSONCXX_INLINE void visit_value(const types::b_dbpointer& value) {

        out << "{" << std::endl;
        pad(1);
        out << "\"$ref\" : \"" << value.collection.data() << "\"";

        if (value.value) {
            out << "," << std::endl;
            pad();
            out << "\"$id\" : \"" << value.value.to_string().data() << "\"" << std::endl;
        }

        pad();
        out << "}";
    }

    BSONCXX_INLINE void visit_value(const types::b_code& value) { out << value.code.data(); }

    BSONCXX_INLINE void visit_value(const types::b_symbol& value) { out << value.symbol.data(); }

    BSONCXX_INLINE void visit_value(const types::b_codewscope& value) { out << value.code.data(); }

    BSONCXX_INLINE void visit_value(const types::b_int32& value) { out << value.value; }

    BSONCXX_INLINE void visit_value(const types::b_timestamp& value) {

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

    BSONCXX_INLINE void visit_value(const types::b_int64& value) { out << value.value; }

    BSONCXX_INLINE void visit_value(const types::b_minkey&) {

        out << "{" << std::endl;
        pad(1);
        out << "\"$minKey\" : 1" << std::endl;
        pad();
        out << "}";
    }

    BSONCXX_INLINE void visit_value(const types::b_maxkey&) {

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

    BSONCXX_INLINE void pad(std::size_t extra = 0) {
        out << std::setw((stack.size() + -1 + extra + padding) * 4) << "" << std::setw(0);
    }

    BSONCXX_INLINE void visit_children(const document::view& view) {
        bool first = true;
        for (auto&& x : view) {
            if (!first) {
                out << ", " << std::endl;
            }
            first = false;
            visit_key(x.key());
            switch (static_cast<int>(x.type())) {
#define BSONCXX_ENUM(name, val)     \
    case val:                        \
        visit_value(x.get_##name()); \
        break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
            }
        }
        out << std::endl;
        stack.pop_back();
    }
};

///
/// Converts a BSON document to a JSON string.
///
/// @param view
///   A valid BSON document.
///
/// @returns A JSON string.
///
inline BSONCXX_INLINE std::string to_json(document::view view) {
    std::stringstream ss;

    json_visitor v(ss, false, 0);
    v.visit_value(types::b_document{view});

    return ss.str();
}

///
/// Converts an element (key-value pair) to a JSON key-value pair.
/// This is not actually a valid JSON document, but it can be useful for debugging.
///
/// @param element
///   A BSON element.
///
/// @returns A JSON key-value pair.
///
inline BSONCXX_INLINE std::string to_json(document::element element) {
    std::stringstream ss;

    json_visitor v(ss, false, 0);

    switch ((int)element.type()) {
#define BSONCXX_ENUM(name, val)           \
    case val:                                \
        v.visit_key(element.key());          \
        v.visit_value(element.get_##name()); \
        break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
    }

    return ss.str();
}

///
/// Converts a BSON value to its JSON string representation.
/// This is not actually a valid JSON document, but it can be useful for debugging.
///
/// @param element
///   A BSON element.
///
/// @returns A JSON value.
///
inline BSONCXX_INLINE std::string to_json(types::value value) {
    std::stringstream ss;

    json_visitor v(ss, false, 0);

    switch ((int)value.type()) {
#define BSONCXX_ENUM(name, val)           \
    case val:                                \
        v.visit_value(value.get_##name()); \
        break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
    }

    return ss.str();
}

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
