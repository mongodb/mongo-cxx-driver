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

index& index::background(bool background) {
    _background = background;
    return *this;
}

index& index::unique(bool unique) {
    _unique = unique;
    return *this;
}

index& index::name(bsoncxx::string::view_or_value name) {
    _name = std::move(name);
    return *this;
}

index& index::sparse(bool sparse) {
    _sparse = sparse;
    return *this;
}

index& index::storage_options(std::unique_ptr<index::base_storage_options> storage_options) {
    _storage_options = std::move(storage_options);
    return *this;
}

index& index::storage_options(std::unique_ptr<index::wiredtiger_storage_options> storage_options) {
    _storage_options = std::move(std::unique_ptr<index::base_storage_options>(
        static_cast<index::base_storage_options*>(storage_options.release())));
    return *this;
}

index& index::expire_after(std::chrono::seconds expire_after) {
    _expire_after = expire_after;
    return *this;
}

index& index::version(std::int32_t version) {
    _version = version;
    return *this;
}

index& index::weights(bsoncxx::document::view weights) {
    _weights = weights;
    return *this;
}

index& index::default_language(bsoncxx::string::view_or_value default_language) {
    _default_language = std::move(default_language);
    return *this;
}

index& index::language_override(bsoncxx::string::view_or_value language_override) {
    _language_override = std::move(language_override);
    return *this;
}

index& index::partial_filter_expression(bsoncxx::document::view partial_filter_expression) {
    _partial_filter_expression = partial_filter_expression;
    return *this;
}

index& index::twod_sphere_version(std::uint8_t twod_sphere_version) {
    _twod_sphere_version = twod_sphere_version;
    return *this;
}

index& index::twod_bits_precision(std::uint8_t twod_bits_precision) {
    _twod_bits_precision = twod_bits_precision;
    return *this;
}

index& index::twod_location_min(double twod_location_min) {
    _twod_location_min = twod_location_min;
    return *this;
}

index& index::twod_location_max(double twod_location_max) {
    _twod_location_max = twod_location_max;
    return *this;
}

index& index::haystack_bucket_size(double haystack_bucket_size) {
    _haystack_bucket_size = haystack_bucket_size;
    return *this;
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

const stdx::optional<std::chrono::seconds>& index::expire_after() const {
    return _expire_after;
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
