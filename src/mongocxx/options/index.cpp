// Copyright 2015 MongoDB Inc.
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

#include <mongocxx/options/index.hpp>

#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/private/libmongoc.hpp>

#include <mongocxx/config/private/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

index::index() = default;

void index::background(bool background) {
    _background = background;
}

void index::unique(bool unique) {
    _unique = unique;
}

void index::name(bsoncxx::string::view_or_value name) {
    _name = std::move(name);
}

void index::sparse(bool sparse) {
    _sparse = sparse;
}

void index::storage_options(std::unique_ptr<index::base_storage_options> storage_options) {
    _storage_options = std::move(storage_options);
}

void index::storage_options(std::unique_ptr<index::wiredtiger_storage_options> storage_options) {
    _storage_options = std::move(std::unique_ptr<index::base_storage_options>(
        static_cast<index::base_storage_options*>(storage_options.release())));
}

void index::expire_after_seconds(std::int32_t expire_after_seconds) {
    _expire_after_seconds = expire_after_seconds;
}

void index::version(std::int32_t version) {
    _version = version;
}

void index::weights(bsoncxx::document::view weights) {
    _weights = weights;
}

void index::default_language(bsoncxx::string::view_or_value default_language) {
    _default_language = std::move(default_language);
}

void index::language_override(bsoncxx::string::view_or_value language_override) {
    _language_override = std::move(language_override);
}

void index::partial_filter_expression(bsoncxx::document::view partial_filter_expression) {
    _partial_filter_expression = partial_filter_expression;
}

void index::twod_sphere_version(std::uint8_t twod_sphere_version) {
    _twod_sphere_version = twod_sphere_version;
}

void index::twod_bits_precision(std::uint8_t twod_bits_precision) {
    _twod_bits_precision = twod_bits_precision;
}

void index::twod_location_min(double twod_location_min) {
    _twod_location_min = twod_location_min;
}

void index::twod_location_max(double twod_location_max) {
    _twod_location_max = twod_location_max;
}

void index::haystack_bucket_size(double haystack_bucket_size) {
    _haystack_bucket_size = haystack_bucket_size;
}

const stdx::optional<bool>& index::background() const {
    return _background;
}

const stdx::optional<bool>& index::unique() const {
    return _unique;
}

const stdx::optional<bsoncxx::string::view_or_value>& index::name() const {
    return _name;
}

const stdx::optional<bool>& index::sparse() const {
    return _sparse;
}

const std::unique_ptr<index::base_storage_options>& index::storage_options() const {
    return _storage_options;
}

const stdx::optional<std::int32_t>& index::expire_after_seconds() const {
    return _expire_after_seconds;
}

const stdx::optional<std::int32_t>& index::version() const {
    return _version;
}

const stdx::optional<bsoncxx::document::view>& index::weights() const {
    return _weights;
}

const stdx::optional<bsoncxx::string::view_or_value>& index::default_language() const {
    return _default_language;
}

const stdx::optional<bsoncxx::string::view_or_value>& index::language_override() const {
    return _language_override;
}

const stdx::optional<bsoncxx::document::view>& index::partial_filter_expression() const {
    return _partial_filter_expression;
}

const stdx::optional<std::uint8_t>& index::twod_sphere_version() const {
    return _twod_sphere_version;
}

const stdx::optional<std::uint8_t>& index::twod_bits_precision() const {
    return _twod_bits_precision;
}

const stdx::optional<double>& index::twod_location_min() const {
    return _twod_location_min;
}

const stdx::optional<double>& index::twod_location_max() const {
    return _twod_location_max;
}

const stdx::optional<double>& index::haystack_bucket_size() const {
    return _haystack_bucket_size;
}

index::base_storage_options::~base_storage_options() = default;

index::wiredtiger_storage_options::~wiredtiger_storage_options() = default;

void index::wiredtiger_storage_options::config_string(
    bsoncxx::string::view_or_value config_string) {
    _config_string = std::move(config_string);
}

const stdx::optional<bsoncxx::string::view_or_value>&
index::wiredtiger_storage_options::config_string() const {
    return _config_string;
}

int index::wiredtiger_storage_options::type() const {
    return mongoc_index_storage_opt_type_t::MONGOC_INDEX_STORAGE_OPT_WIREDTIGER;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
