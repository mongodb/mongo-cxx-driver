// Copyright 2016 MongoDB Inc.
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

#include <mongocxx/options/private/rewriter.hh>

#include <bsoncxx/string/to_string.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

namespace {

stdx::optional<std::int32_t> element_to_int32_t(bsoncxx::document::element ele) {
    switch (ele.type()) {
        case bsoncxx::type::k_int32: {
            return ele.get_int32().value;
        }
        case bsoncxx::type::k_int64: {
            std::int64_t val_int64 = ele.get_int64().value;
            if (val_int64 < std::numeric_limits<std::int32_t>::min() ||
                val_int64 > std::numeric_limits<std::int32_t>::max()) {
                return stdx::nullopt;
            }
            return static_cast<std::int32_t>(val_int64);
        }
        case bsoncxx::type::k_double: {
            double val_double = ele.get_double().value;
            if (val_double < std::numeric_limits<std::int32_t>::min() ||
                val_double > std::numeric_limits<std::int32_t>::max()) {
                return stdx::nullopt;
            }
            return static_cast<std::int32_t>(val_double);
        }
        default: { return stdx::nullopt; }
    }
}

void convert_comment_modifier(find* options, bsoncxx::document::element ele) {
    if (ele.type() != bsoncxx::type::k_utf8) {
        throw logic_error{error_code::k_invalid_parameter, "string type required for $comment"};
    }
    if (!options->comment()) {
        options->comment(bsoncxx::string::to_string(ele.get_utf8().value));
    }
}

void convert_hint_modifier(find* options, bsoncxx::document::element ele) {
    hint hint{bsoncxx::document::view{}};
    switch (ele.type()) {
        case bsoncxx::type::k_document: {
            hint = mongocxx::hint{bsoncxx::document::value{ele.get_document().value}};
            break;
        }
        case bsoncxx::type::k_utf8: {
            hint = mongocxx::hint{bsoncxx::string::to_string(ele.get_utf8().value)};
            break;
        }
        default: {
            throw logic_error{error_code::k_invalid_parameter,
                              "document or string type required for $hint"};
        }
    }
    if (!options->hint()) {
        options->hint(hint);
    }
}

void convert_max_modifier(find* options, bsoncxx::document::element ele) {
    if (ele.type() != bsoncxx::type::k_document) {
        throw logic_error{error_code::k_invalid_parameter, "document type required for $max"};
    }
    if (!options->max()) {
        options->max(bsoncxx::document::value{ele.get_document().value});
    }
}

void convert_max_scan_modifier(find* options, bsoncxx::document::element ele) {
    auto max_scan = element_to_int32_t(ele);
    if (!max_scan) {
        throw logic_error{error_code::k_invalid_parameter,
                          "value for $maxScan must be numeric and fit in a signed 32-bit integer"};
    }
    if (!options->max_scan_deprecated()) {
        options->max_scan_deprecated(*max_scan);
    }
}

void convert_max_time_ms_modifier(find* options, bsoncxx::document::element ele) {
    auto max_time = element_to_int32_t(ele);
    if (!max_time) {
        throw logic_error{
            error_code::k_invalid_parameter,
            "value for $maxTimeMS must be numeric and fit in a signed 32-bit integer"};
    }
    if (!options->max_time()) {
        options->max_time(std::chrono::milliseconds{*max_time});
    }
}

void convert_min_modifier(find* options, bsoncxx::document::element ele) {
    if (ele.type() != bsoncxx::type::k_document) {
        throw logic_error{error_code::k_invalid_parameter, "document type required for $min"};
    }
    if (!options->min()) {
        options->min(bsoncxx::document::value{ele.get_document().value});
    }
}

void convert_orderby_modifier(find* options, bsoncxx::document::element ele) {
    if (ele.type() != bsoncxx::type::k_document) {
        throw logic_error{error_code::k_invalid_parameter, "document type required for $orderby"};
    }
    if (!options->sort()) {
        options->sort(bsoncxx::document::value{ele.get_document().value});
    }
}

void convert_return_key_modifier(find* options, bsoncxx::document::element ele) {
    if (ele.type() != bsoncxx::type::k_bool) {
        throw logic_error{error_code::k_invalid_parameter, "bool type required for $returnKey"};
    }
    if (!options->return_key()) {
        options->return_key(ele.get_bool().value);
    }
}

void convert_show_disk_loc_modifier(find* options, bsoncxx::document::element ele) {
    if (ele.type() != bsoncxx::type::k_bool) {
        throw logic_error{error_code::k_invalid_parameter, "bool type required for $showDiskLoc"};
    }
    if (!options->show_record_id()) {
        options->show_record_id(ele.get_bool().value);
    }
}

void convert_snapshot_modifier(find* options, bsoncxx::document::element ele) {
    if (ele.type() != bsoncxx::type::k_bool) {
        throw logic_error{error_code::k_invalid_parameter, "bool type required for $snapshot"};
    }
    if (!options->snapshot_deprecated()) {
        options->snapshot_deprecated(ele.get_bool().value);
    }
}

}  // namespace

find rewriter::rewrite_find_modifiers(const find& options) {
    if (!options.modifiers_deprecated()) {
        return options;
    }

    find converted_options{options};

    for (auto&& ele : converted_options.modifiers_deprecated()->view()) {
        if (ele.key() == stdx::string_view("$comment")) {
            convert_comment_modifier(&converted_options, ele);
        } else if (ele.key() == stdx::string_view("$explain")) {
            throw logic_error{error_code::k_invalid_parameter, "$explain modifier unsupported"};
        } else if (ele.key() == stdx::string_view("$hint")) {
            convert_hint_modifier(&converted_options, ele);
        } else if (ele.key() == stdx::string_view("$max")) {
            convert_max_modifier(&converted_options, ele);
        } else if (ele.key() == stdx::string_view("$maxScan")) {
            convert_max_scan_modifier(&converted_options, ele);
        } else if (ele.key() == stdx::string_view("$maxTimeMS")) {
            convert_max_time_ms_modifier(&converted_options, ele);
        } else if (ele.key() == stdx::string_view("$min")) {
            convert_min_modifier(&converted_options, ele);
        } else if (ele.key() == stdx::string_view("$orderby")) {
            convert_orderby_modifier(&converted_options, ele);
        } else if (ele.key() == stdx::string_view("$query")) {
            throw logic_error{error_code::k_invalid_parameter, "$query modifier unsupported"};
        } else if (ele.key() == stdx::string_view("$returnKey")) {
            convert_return_key_modifier(&converted_options, ele);
        } else if (ele.key() == stdx::string_view("$showDiskLoc")) {
            convert_show_disk_loc_modifier(&converted_options, ele);
        } else if (ele.key() == stdx::string_view("$snapshot")) {
            convert_snapshot_modifier(&converted_options, ele);
        } else {
            throw logic_error{error_code::k_invalid_parameter,
                              std::string{"unrecognized key in modifiers: "} +
                                  bsoncxx::string::to_string(ele.key())};
        }
    }

    converted_options.modifiers_clear();

    return converted_options;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
