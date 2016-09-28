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

#include <mongocxx/options/find.hpp>

#include <bsoncxx/types.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/private/read_preference.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

find& find::allow_partial_results(bool allow_partial) {
    _allow_partial_results = allow_partial;
    return *this;
}

find& find::batch_size(std::int32_t batch_size) {
    _batch_size = batch_size;
    return *this;
}

find& find::collation(bsoncxx::document::view_or_value collation) {
    _collation = std::move(collation);
    return *this;
}

find& find::comment(bsoncxx::string::view_or_value comment) {
    _comment = std::move(comment);
    return *this;
}

find& find::cursor_type(cursor::type cursor_type) {
    _cursor_type = cursor_type;
    return *this;
}

find& find::hint(class hint index_hint) {
    _hint = std::move(index_hint);
    return *this;
}

find& find::limit(std::int32_t limit) {
    _limit = limit;
    return *this;
}

find& find::max(bsoncxx::document::view_or_value max) {
    _max = std::move(max);
    return *this;
}

find& find::max_await_time(std::chrono::milliseconds max_await_time) {
    _max_await_time = std::move(max_await_time);
    return *this;
}

find& find::max_scan(std::int32_t max_scan) {
    _max_scan = max_scan;
    return *this;
}

find& find::max_time(std::chrono::milliseconds max_time) {
    _max_time = std::move(max_time);
    return *this;
}

find& find::min(bsoncxx::document::view_or_value min) {
    _min = std::move(min);
    return *this;
}

find& find::modifiers(bsoncxx::document::view_or_value modifiers) {
    _modifiers = std::move(modifiers);
    return *this;
}

find& find::no_cursor_timeout(bool no_cursor_timeout) {
    _no_cursor_timeout = no_cursor_timeout;
    return *this;
}

find& find::projection(bsoncxx::document::view_or_value projection) {
    _projection = std::move(projection);
    return *this;
}

find& find::read_preference(class read_preference rp) {
    _read_preference = std::move(rp);
    return *this;
}

find& find::return_key(bool return_key) {
    _return_key = return_key;
    return *this;
}

find& find::show_record_id(bool show_record_id) {
    _show_record_id = show_record_id;
    return *this;
}

find& find::skip(std::int32_t skip) {
    _skip = skip;
    return *this;
}

find& find::snapshot(bool snapshot) {
    _snapshot = snapshot;
    return *this;
}

find& find::sort(bsoncxx::document::view_or_value ordering) {
    _ordering = std::move(ordering);
    return *this;
}

const stdx::optional<bool>& find::allow_partial_results() const {
    return _allow_partial_results;
}

const stdx::optional<std::int32_t>& find::batch_size() const {
    return _batch_size;
}

const stdx::optional<bsoncxx::document::view_or_value>& find::collation() const {
    return _collation;
}

const stdx::optional<bsoncxx::string::view_or_value>& find::comment() const {
    return _comment;
}

const stdx::optional<cursor::type>& find::cursor_type() const {
    return _cursor_type;
}

const stdx::optional<class hint>& find::hint() const {
    return _hint;
}

const stdx::optional<std::int32_t>& find::limit() const {
    return _limit;
}

const stdx::optional<bsoncxx::document::view_or_value>& find::max() const {
    return _max;
}

const stdx::optional<std::chrono::milliseconds>& find::max_await_time() const {
    return _max_await_time;
}

const stdx::optional<std::int32_t>& find::max_scan() const {
    return _max_scan;
}

const stdx::optional<std::chrono::milliseconds>& find::max_time() const {
    return _max_time;
}

const stdx::optional<bsoncxx::document::view_or_value>& find::min() const {
    return _min;
}

const stdx::optional<bsoncxx::document::view_or_value>& find::modifiers() const {
    return _modifiers;
}

const stdx::optional<bool>& find::no_cursor_timeout() const {
    return _no_cursor_timeout;
}

const stdx::optional<bsoncxx::document::view_or_value>& find::projection() const {
    return _projection;
}

const stdx::optional<bool>& find::return_key() const {
    return _return_key;
}

const stdx::optional<bool>& find::show_record_id() const {
    return _show_record_id;
}

const stdx::optional<std::int32_t>& find::skip() const {
    return _skip;
}

const stdx::optional<bool>& find::snapshot() const {
    return _snapshot;
}

const stdx::optional<bsoncxx::document::view_or_value>& find::sort() const {
    return _ordering;
}

const stdx::optional<class read_preference>& find::read_preference() const {
    return _read_preference;
}

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
        options->comment(std::string{ele.get_utf8().value.to_string()});
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
            hint = mongocxx::hint{std::string{ele.get_utf8().value.to_string()}};
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
    if (!options->max_scan()) {
        options->max_scan(*max_scan);
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
    if (!options->snapshot()) {
        options->snapshot(ele.get_bool().value);
    }
}

}  // namespace

find find::convert_all_modifiers() const {
    find converted_options(*this);

    if (!converted_options.modifiers()) {
        return *this;
    }

    for (auto&& ele : converted_options.modifiers()->view()) {
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
            throw logic_error{
                error_code::k_invalid_parameter,
                std::string{"unrecognized key in modifiers: "} + ele.key().to_string()};
        }
    }

    converted_options._modifiers = stdx::nullopt;
    return converted_options;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
