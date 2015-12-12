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

#pragma once

#include <mongocxx/config/prelude.hpp>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/stdx.hpp>
#include <memory>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class collection;

namespace options {

///
/// Class representing the optional arguments to a MongoDB create index operation.
///
/// @see http://docs.mongodb.org/manual/reference/method/db.collection.createIndex/#options
///
class MONGOCXX_API index {
   public:
    ///
    /// Base class representing the optional storage engine options for indexes.
    ///
    class base_storage_options {
       public:
        virtual ~base_storage_options();

       private:
        friend collection;
        MONGOCXX_PRIVATE virtual const int type() const = 0;
    };

    ///
    /// Class representing the optional WiredTiger storage engine options for indexes.
    ///
    class wiredtiger_storage_options final : public base_storage_options {
       public:
        ~wiredtiger_storage_options() override;

        ///
        /// Set the WiredTiger configuration string.
        ///
        /// @param config_string
        ///   The WiredTiger configuration string.
        ///
        void config_string(std::string config_string);

        ///
        /// The current config_string setting.
        ///
        /// @return The current config_string.
        ///
        const stdx::optional<std::string>& config_string() const;

       private:
        friend collection;
        MONGOCXX_PRIVATE const int type() const override;
        stdx::optional<std::string> _config_string;
    };

    index();

    ///
    /// Whether or not to build the index in the background so that building the index does not
    /// block other database activities. The default is to build indexes in the foreground
    ///
    /// @param background
    ///   Whether or not to build the index in the background.
    ///
    /// @see https://docs.mongodb.org/manual/tutorial/build-indexes-in-the-background/
    ///
    void background(bool background);

    ///
    /// The current background setting.
    ///
    /// @return The current background.
    ///
    const stdx::optional<bool>& background() const;

    ///
    /// Whether or not to create a unique index so that the collection will not accept insertion of
    /// documents where the index key or keys match an existing value in the index.
    ///
    /// @param unique
    ///   Whether or not to create a unique index.
    ///
    /// @see https://docs.mongodb.org/manual/core/index-unique/
    ///
    void unique(bool unique);

    ///
    /// The current unique setting.
    ///
    /// @return The current unique.
    ///
    const stdx::optional<bool>& unique() const;

    ///
    /// The name of the index.
    ///
    /// @param name
    ///   The name of the index.
    ///
    void name(std::string name);

    ///
    /// The current name setting.
    ///
    /// @return The current name.
    ///
    const stdx::optional<std::string>& name() const;

    ///
    /// Whether or not to create a sparse index. Sparse indexes only reference documents with the
    /// indexed fields.
    ///
    /// @param sparse
    ///   Whether or not to create a sparse index.
    ///
    /// @see https://docs.mongodb.org/manual/core/index-sparse/
    ///
    void sparse(bool sparse);

    ///
    /// The current sparse setting.
    ///
    /// @return The current sparse setting.
    ///
    const stdx::optional<bool>& sparse() const;

    ///
    /// Optionally used only in MongoDB 3.0.0 and higher. Specifies the storage engine options for
    /// the index.
    ///
    /// @param storage_options
    ///   The storage engine options for the index.
    ///
    void storage_options(std::unique_ptr<base_storage_options> storage_options);
    void storage_options(std::unique_ptr<wiredtiger_storage_options> storage_options);

    ///
    /// Set a value, in seconds, as a TTL to control how long MongoDB retains documents in this
    /// collection.
    ///
    /// @param expire_after_seconds
    ///   The amount of time, in seconds, to retain documents.
    ///
    /// @see https://docs.mongodb.org/manual/core/index-ttl/
    ///
    void expire_after_seconds(std::int32_t expire_after_seconds);

    ///
    /// The current expire_after_seconds setting.
    ///
    /// @return The current expire_after_seconds.
    ///
    const stdx::optional<std::int32_t>& expire_after_seconds() const;

    ///
    /// Sets the index version.
    ///
    /// @param version
    ///   The index version.
    ///
    void version(std::int32_t v);

    ///
    /// The current index version.
    ///
    /// @return The current index version.
    ///
    const stdx::optional<std::int32_t>& version() const;

    ///
    /// For text indexes, sets the weight document. The weight document contains field and weight
    /// pairs.
    ///
    /// @param weights
    ///   The weight document for text indexes.
    ///
    void weights(bsoncxx::document::view weights);

    ///
    /// The current weights setting.
    ///
    /// @return The current weights.
    ///
    const stdx::optional<bsoncxx::document::view>& weights() const;

    ///
    /// For text indexes, the language that determines the list of stop words and the rules for the
    /// stemmer and tokenizer.
    ///
    /// @param default_language
    ///   The default language used when creating text indexes.
    ///
    void default_language(std::string default_language);

    ///
    /// The current default_language setting.
    ///
    /// @return The current default_language.
    ///
    const stdx::optional<std::string>& default_language() const;

    ///
    /// For text indexes, the name of the field, in the collection’s documents, that contains the
    /// override language for the document.
    ///
    /// @param language_override
    ///   The name of the field that contains the override language for text indexes.
    ///
    void language_override(std::string language_override);

    ///
    /// The current name of the field that contains the override language for text indexes.
    ///
    /// @return The name of the field that contains the override language for text indexes.
    ///
    const stdx::optional<std::string>& language_override() const;

    ///
    /// Sets the document for the partial filter expression for partial indexes.
    ///
    /// @param partial_filter_expression
    ///   The partial filter expression document.
    ///
    void partial_filter_expression(bsoncxx::document::view partial_filter_expression);

    ///
    /// The current partial_filter_expression setting.
    ///
    /// @return The current partial_filter_expression.
    ///
    const stdx::optional<bsoncxx::document::view>& partial_filter_expression() const;

    ///
    /// For 2dsphere indexes, the 2dsphere index version number. Version can be either 1 or 2.
    ///
    /// @param twod_sphere_version
    ///   The 2dsphere index version number.
    ///
    void twod_sphere_version(std::uint8_t twod_sphere_version);

    ///
    /// The current twod_sphere_version setting.
    ///
    /// @return The current twod_sphere_version.
    ///
    const stdx::optional<std::uint8_t>& twod_sphere_version() const;

    ///
    /// For 2d indexes, the precision of the stored geohash value of the location data.
    ///
    /// @param twod_bits_precision
    ///   The precision of the stored geohash value.
    ///
    void twod_bits_precision(std::uint8_t twod_bits_precision);

    ///
    /// The current precision of the stored geohash value of the location data.
    ///
    /// @return The precision of the stored geohash value of the location data.
    ///
    const stdx::optional<std::uint8_t>& twod_bits_precision() const;

    ///
    /// For 2d indexes, the lower inclusive boundary for the longitude and latitude values.
    ///
    /// @param twod_location_min
    ///   The lower inclusive boundary.
    ///
    void twod_location_min(double twod_location_min);

    ///
    /// The current lower inclusive boundary for the longitude and latitude values.
    ///
    /// @return The lower inclusive boundary for the longitude and latitude values.
    ///
    const stdx::optional<double>& twod_location_min() const;

    ///
    /// For 2d indexes, the upper inclusive boundary for the longitude and latitude values.
    ///
    /// @param twod_location_max
    ///   The upper inclusive boundary.
    ///
    void twod_location_max(double twod_location_max);

    ///
    /// The current upper inclusive boundary for the longitude and latitude values.
    ///
    /// @return The upper inclusive boundary for the longitude and latitude values.
    ///
    const stdx::optional<double>& twod_location_max() const;

    ///
    /// For geoHaystack indexes, specify the number of units within which to group the location
    /// values; i.e. group in the same bucket those location values that are within the specified
    /// number of units to each other.
    ///
    /// @see https://docs.mongodb.org/manual/core/geohaystack/
    ///
    /// @param haystack_bucket_size
    ///   The geoHaystack bucket size.
    ///
    void haystack_bucket_size(double haystack_bucket_size);

    ///
    /// The current haystack_bucket_size setting.
    ///
    /// @return The current haystack_bucket_size.
    ///
    const stdx::optional<double>& haystack_bucket_size() const;

   private:
    friend collection;

    stdx::optional<bool> _background;
    stdx::optional<bool> _unique;
    stdx::optional<std::string> _name;
    stdx::optional<bool> _sparse;
    std::unique_ptr<base_storage_options> _storage_options;
    stdx::optional<std::int32_t> _expire_after_seconds;
    stdx::optional<std::int32_t> _version;
    stdx::optional<bsoncxx::document::view> _weights;
    stdx::optional<std::string> _default_language;
    stdx::optional<std::string> _language_override;
    stdx::optional<bsoncxx::document::view> _partial_filter_expression;
    stdx::optional<std::uint8_t> _twod_sphere_version;
    stdx::optional<std::uint8_t> _twod_bits_precision;
    stdx::optional<double> _twod_location_min;
    stdx::optional<double> _twod_location_max;
    stdx::optional<double> _haystack_bucket_size;

    ///
    /// The current storage_options setting.
    ///
    /// @return The current storage_options.
    ///
    const std::unique_ptr<base_storage_options>& storage_options() const;
};

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
