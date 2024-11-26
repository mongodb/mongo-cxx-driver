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

#pragma once

#include <chrono>
#include <memory>

#include <mongocxx/collection-fwd.hpp>
#include <mongocxx/options/index-fwd.hpp>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/view_or_value.hpp>

#include <bsoncxx/config/prelude.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Class representing the optional arguments to a MongoDB create index operation.
///
/// @see
/// - https://www.mongodb.com/docs/manual/reference/command/createIndexes
///
class index {
   public:
    BSONCXX_PUSH_WARNINGS();
    BSONCXX_DISABLE_WARNING(MSVC(4251));
    BSONCXX_DISABLE_WARNING(MSVC(4275));

    ///
    /// Base class representing the optional storage engine options for indexes.
    ///
    class MONGOCXX_ABI_EXPORT base_storage_options {
       public:
        virtual ~base_storage_options();

        base_storage_options(base_storage_options&&) = default;
        base_storage_options& operator=(base_storage_options&&) = default;
        base_storage_options(const base_storage_options&) = default;
        base_storage_options& operator=(const base_storage_options&) = default;

        base_storage_options() = default;

       private:
        friend ::mongocxx::v_noabi::options::index;

        virtual int type() const = 0;
    };

    ///
    /// Class representing the optional WiredTiger storage engine options for indexes.
    ///
    class MONGOCXX_ABI_EXPORT wiredtiger_storage_options final : public base_storage_options {
       public:
        ~wiredtiger_storage_options() override;

        wiredtiger_storage_options(wiredtiger_storage_options&&) = default;
        wiredtiger_storage_options& operator=(wiredtiger_storage_options&&) = default;
        wiredtiger_storage_options(const wiredtiger_storage_options&) = default;
        wiredtiger_storage_options& operator=(const wiredtiger_storage_options&) = default;

        wiredtiger_storage_options() = default;

        ///
        /// Set the WiredTiger configuration string.
        ///
        /// @param config_string
        ///   The WiredTiger configuration string.
        ///
        void config_string(bsoncxx::v_noabi::string::view_or_value config_string);

        ///
        /// The current config_string setting.
        ///
        /// @return The current config_string.
        ///
        const bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value>&
        config_string() const;

       private:
        friend ::mongocxx::v_noabi::collection;

        MONGOCXX_ABI_NO_EXPORT int type() const override;

        bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> _config_string;
    };

    BSONCXX_POP_WARNINGS();

    MONGOCXX_ABI_EXPORT_CDECL() index();

    ///
    /// Whether or not to build the index in the background so that building the index does not
    /// block other database activities. The default is to build indexes in the foreground
    ///
    /// @param background
    ///   Whether or not to build the index in the background.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/tutorial/build-indexes-in-the-background/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index&) background(bool background);

    ///
    /// The current background setting.
    ///
    /// @return The current background.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<bool>&) background() const;

    ///
    /// Whether or not to create a unique index so that the collection will not accept insertion of
    /// documents where the index key or keys match an existing value in the index.
    ///
    /// @param unique
    ///   Whether or not to create a unique index.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/index-unique/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index&) unique(bool unique);

    ///
    /// The current unique setting.
    ///
    /// @return The current unique.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<bool>&) unique() const;

    ///
    /// Whether or not the index is hidden from the query planner. A hidden index is not evaluated
    /// as part of query plan selection.
    ///
    /// @param hidden
    ///   Whether or not to create a hidden index.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/index-hidden/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index&) hidden(bool hidden);

    ///
    /// The current hidden setting.
    ///
    /// @return The current hidden.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<bool>&) hidden() const;

    ///
    /// The name of the index.
    ///
    /// @param name
    ///   The name of the index.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index&) name(bsoncxx::v_noabi::string::view_or_value name);

    ///
    /// The current name setting.
    ///
    /// @return The current name.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(
        const bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value>&)
    name() const;

    ///
    /// Sets the collation for this index.
    ///
    /// @param collation
    ///   The new collation.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/collation/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index&) collation(bsoncxx::v_noabi::document::view collation);

    ///
    /// Retrieves the current collation for this index.
    ///
    /// @return
    ///   The current collation.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/collation/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(
        const bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view>&)
    collation() const;

    ///
    /// Whether or not to create a sparse index. Sparse indexes only reference documents with the
    /// indexed fields.
    ///
    /// @param sparse
    ///   Whether or not to create a sparse index.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/index-sparse/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index&) sparse(bool sparse);

    ///
    /// The current sparse setting.
    ///
    /// @return The current sparse setting.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<bool>&) sparse() const;

    ///
    /// Optionally used only in MongoDB 3.0.0 and higher. Specifies the storage engine options for
    /// the index.
    ///
    /// @param storage_options
    ///   The storage engine options for the index.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index&)
    storage_options(std::unique_ptr<base_storage_options> storage_options);

    ///
    /// Optionally used only in MongoDB 3.0.0 and higher. Specifies the WiredTiger-specific storage
    /// engine options for the index.
    ///
    /// @param storage_options
    ///   The storage engine options for the index.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index&)
    storage_options(std::unique_ptr<wiredtiger_storage_options> storage_options);

    ///
    /// Set a value, in seconds, as a TTL to control how long MongoDB retains documents in this
    /// collection.
    ///
    /// @param seconds
    ///   The amount of time, in seconds, to retain documents.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/index-ttl/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index&) expire_after(std::chrono::seconds seconds);

    ///
    /// The current expire_after setting.
    ///
    /// @return The current expire_after value.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<std::chrono::seconds>&)
    expire_after() const;

    ///
    /// Sets the index version.
    ///
    /// @param v
    ///   The index version.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index&) version(std::int32_t v);

    ///
    /// The current index version.
    ///
    /// @return The current index version.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<std::int32_t>&)
    version() const;

    ///
    /// For text indexes, sets the weight document. The weight document contains field and weight
    /// pairs.
    ///
    /// @param weights
    ///   The weight document for text indexes.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index&) weights(bsoncxx::v_noabi::document::view weights);

    ///
    /// The current weights setting.
    ///
    /// @return The current weights.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(
        const bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view>&)
    weights() const;

    ///
    /// For text indexes, the language that determines the list of stop words and the rules for the
    /// stemmer and tokenizer.
    ///
    /// @param default_language
    ///   The default language used when creating text indexes.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index&)
    default_language(bsoncxx::v_noabi::string::view_or_value default_language);

    ///
    /// The current default_language setting.
    ///
    /// @return The current default_language.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(
        const bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value>&)
    default_language() const;

    ///
    /// For text indexes, the name of the field, in the collection’s documents, that contains the
    /// override language for the document.
    ///
    /// @param language_override
    ///   The name of the field that contains the override language for text indexes.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index&)
    language_override(bsoncxx::v_noabi::string::view_or_value language_override);

    ///
    /// The current name of the field that contains the override language for text indexes.
    ///
    /// @return The name of the field that contains the override language for text indexes.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(
        const bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value>&)
    language_override() const;

    ///
    /// Sets the document for the partial filter expression for partial indexes.
    ///
    /// @param partial_filter_expression
    ///   The partial filter expression document.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index&)
    partial_filter_expression(bsoncxx::v_noabi::document::view partial_filter_expression);

    ///
    /// The current partial_filter_expression setting.
    ///
    /// @return The current partial_filter_expression.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(
        const bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view>&)
    partial_filter_expression() const;

    ///
    /// For 2dsphere indexes, the 2dsphere index version number. Version can be either 1 or 2.
    ///
    /// @param twod_sphere_version
    ///   The 2dsphere index version number.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index&) twod_sphere_version(std::uint8_t twod_sphere_version);

    ///
    /// The current twod_sphere_version setting.
    ///
    /// @return The current twod_sphere_version.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<std::uint8_t>&)
    twod_sphere_version() const;

    ///
    /// For 2d indexes, the precision of the stored geohash value of the location data.
    ///
    /// @param twod_bits_precision
    ///   The precision of the stored geohash value.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index&) twod_bits_precision(std::uint8_t twod_bits_precision);

    ///
    /// The current precision of the stored geohash value of the location data.
    ///
    /// @return The precision of the stored geohash value of the location data.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<std::uint8_t>&)
    twod_bits_precision() const;

    ///
    /// For 2d indexes, the lower inclusive boundary for the longitude and latitude values.
    ///
    /// @param twod_location_min
    ///   The lower inclusive boundary.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index&) twod_location_min(double twod_location_min);

    ///
    /// The current lower inclusive boundary for the longitude and latitude values.
    ///
    /// @return The lower inclusive boundary for the longitude and latitude values.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<double>&)
    twod_location_min() const;

    ///
    /// For 2d indexes, the upper inclusive boundary for the longitude and latitude values.
    ///
    /// @param twod_location_max
    ///   The upper inclusive boundary.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(index&) twod_location_max(double twod_location_max);

    ///
    /// The current upper inclusive boundary for the longitude and latitude values.
    ///
    /// @return The upper inclusive boundary for the longitude and latitude values.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<double>&)
    twod_location_max() const;

    ///
    /// For geoHaystack indexes, specify the number of units within which to group the location
    /// values; i.e. group in the same bucket those location values that are within the specified
    /// number of units to each other.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/geohaystack/
    ///
    /// @param haystack_bucket_size
    ///   The geoHaystack bucket size.
    ///
    /// @deprecated
    ///   This option is deprecated.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_DEPRECATED MONGOCXX_ABI_EXPORT_CDECL(index&)
        haystack_bucket_size(double haystack_bucket_size);
    MONGOCXX_ABI_EXPORT_CDECL(index&) haystack_bucket_size_deprecated(double haystack_bucket_size);

    ///
    /// The current haystack_bucket_size setting.
    ///
    /// @return The current haystack_bucket_size.
    ///
    /// @deprecated
    ///   This method is deprecated.
    ///
    MONGOCXX_DEPRECATED MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<double>&)
        haystack_bucket_size() const;
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<double>&)
    haystack_bucket_size_deprecated() const;

    ///
    /// Conversion operator that provides a view of the options in document form.
    ///
    /// @exception mongocxx::v_noabi::logic_error if an invalid expireAfterSeconds field is
    /// provided.
    ///
    /// @return A view of the current builder contents.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() operator bsoncxx::v_noabi::document::view_or_value();

   private:
    friend ::mongocxx::v_noabi::collection;

    bsoncxx::v_noabi::stdx::optional<bool> _background;
    bsoncxx::v_noabi::stdx::optional<bool> _unique;
    bsoncxx::v_noabi::stdx::optional<bool> _hidden;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> _name;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> _collation;
    bsoncxx::v_noabi::stdx::optional<bool> _sparse;
    std::unique_ptr<base_storage_options> _storage_options;
    bsoncxx::v_noabi::stdx::optional<std::chrono::seconds> _expire_after;
    bsoncxx::v_noabi::stdx::optional<std::int32_t> _version;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> _weights;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> _default_language;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> _language_override;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> _partial_filter_expression;
    bsoncxx::v_noabi::stdx::optional<std::uint8_t> _twod_sphere_version;
    bsoncxx::v_noabi::stdx::optional<std::uint8_t> _twod_bits_precision;
    bsoncxx::v_noabi::stdx::optional<double> _twod_location_min;
    bsoncxx::v_noabi::stdx::optional<double> _twod_location_max;
    bsoncxx::v_noabi::stdx::optional<double> _haystack_bucket_size;

    //
    // Return the current storage_options setting.
    //
    const std::unique_ptr<base_storage_options>& storage_options() const;
};

}  // namespace options
}  // namespace v_noabi
}  // namespace mongocxx

#include <bsoncxx/config/postlude.hpp>

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::index.
///
