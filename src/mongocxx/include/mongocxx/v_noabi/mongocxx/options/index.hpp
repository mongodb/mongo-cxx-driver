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

#include <mongocxx/options/index-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/indexes.hpp> // IWYU pragma: export

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

#include <mongocxx/collection-fwd.hpp>

#include <bsoncxx/document/value.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
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
/// Used by MongoDB index creation operations.
///
/// @see
/// - https://www.mongodb.com/docs/manual/reference/command/createIndexes
///
class index {
   public:
    BSONCXX_PRIVATE_WARNINGS_PUSH();
    BSONCXX_PRIVATE_WARNINGS_DISABLE(MSVC(4251));
    BSONCXX_PRIVATE_WARNINGS_DISABLE(MSVC(4275));

    ///
    /// Base class representing the optional storage engine options for indexes.
    ///
    /// @deprecated Use @ref mongocxx::v_noabi::options::index::storage_engine instead.
    ///
    class MONGOCXX_ABI_EXPORT base_storage_options {
       public:
        virtual ~base_storage_options();

        base_storage_options(base_storage_options&&) = default;
        base_storage_options& operator=(base_storage_options&&) = default;
        base_storage_options(base_storage_options const&) = default;
        base_storage_options& operator=(base_storage_options const&) = default;

        base_storage_options() = default;

       private:
        friend v_noabi::options::index;

        virtual int type() const = 0;
    };

    ///
    /// The optional WiredTiger storage engine options for indexes.
    ///
    /// @deprecated Use @ref mongocxx::v_noabi::options::index::storage_engine instead.
    ///
    class MONGOCXX_ABI_EXPORT wiredtiger_storage_options final : public base_storage_options {
       public:
        ~wiredtiger_storage_options() override;

        wiredtiger_storage_options(wiredtiger_storage_options&&) = default;
        wiredtiger_storage_options& operator=(wiredtiger_storage_options&&) = default;
        wiredtiger_storage_options(wiredtiger_storage_options const&) = default;
        wiredtiger_storage_options& operator=(wiredtiger_storage_options const&) = default;

        wiredtiger_storage_options() = default;

        ///
        /// Set the WiredTiger configuration string.
        ///
        /// @param config_string
        ///   The WiredTiger configuration string.
        ///
        void config_string(bsoncxx::v_noabi::string::view_or_value config_string) {
            _config_string = std::move(config_string);
        }

        ///
        /// The current config_string setting.
        ///
        /// @return The current config_string.
        ///
        bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> const& config_string() const {
            return _config_string;
        }

       private:
        friend v_noabi::collection;

        MONGOCXX_ABI_NO_EXPORT int type() const override;

        bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> _config_string;
    };

    BSONCXX_PRIVATE_WARNINGS_POP();

    ///
    /// Default initialization.
    ///
    index() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /// @warning The fields owned by `opts` MUST outlive the views in this object.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() index(v1::indexes::options const& opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @note The `storage_options` and `haystack_bucket_size` fields are ignored.
    ///
    explicit operator v1::indexes::options() const {
        using bsoncxx::v_noabi::to_v1;

        v1::indexes::options ret;

        if (_background) {
            ret.background(*_background);
        }

        if (_unique) {
            ret.unique(*_unique);
        }

        if (_hidden) {
            ret.hidden(*_hidden);
        }

        if (_name) {
            ret.name(std::string{_name->view()});
        }

        if (_collation) {
            ret.collation(bsoncxx::v1::document::value{to_v1(*_collation)});
        }

        if (_sparse) {
            ret.sparse(*_sparse);
        }

        // _storage_options

        if (_storage_engine) {
            ret.storage_engine(bsoncxx::v1::document::value{to_v1(*_storage_engine)});
        }

        if (_expire_after) {
            ret.expire_after(*_expire_after);
        }

        if (_version) {
            ret.version(*_version);
        }

        if (_weights) {
            ret.weights(bsoncxx::v1::document::value{to_v1(*_weights)});
        }

        if (_default_language) {
            ret.default_language(std::string{_default_language->view()});
        }

        if (_language_override) {
            ret.language_override(std::string{_language_override->view()});
        }

        if (_partial_filter_expression) {
            ret.partial_filter_expression(bsoncxx::v1::document::value{to_v1(*_partial_filter_expression)});
        }

        if (_twod_sphere_version) {
            ret.twod_sphere_version(*_twod_sphere_version);
        }

        if (_twod_bits_precision) {
            ret.twod_bits_precision(*_twod_bits_precision);
        }

        if (_twod_location_min) {
            ret.twod_location_min(*_twod_location_min);
        }

        if (_twod_location_max) {
            ret.twod_location_max(*_twod_location_max);
        }

        // _haystack_bucket_size

        return ret;
    }

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
    index& background(bool background) {
        _background = std::move(background);
        return *this;
    }

    ///
    /// The current background setting.
    ///
    /// @return The current background.
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& background() const {
        return _background;
    }

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
    index& unique(bool unique) {
        _unique = std::move(unique);
        return *this;
    }

    ///
    /// The current unique setting.
    ///
    /// @return The current unique.
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& unique() const {
        return _unique;
    }

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
    index& hidden(bool hidden) {
        _hidden = std::move(hidden);
        return *this;
    }

    ///
    /// The current hidden setting.
    ///
    /// @return The current hidden.
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& hidden() const {
        return _hidden;
    }

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
    index& name(bsoncxx::v_noabi::string::view_or_value name) {
        _name = std::move(name);
        return *this;
    }

    ///
    /// The current name setting.
    ///
    /// @return The current name.
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> const& name() const {
        return _name;
    }

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
    index& collation(bsoncxx::v_noabi::document::view collation) {
        _collation = std::move(collation);
        return *this;
    }

    ///
    /// Retrieves the current collation for this index.
    ///
    /// @return
    ///   The current collation.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/collation/
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> const& collation() const {
        return _collation;
    }

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
    index& sparse(bool sparse) {
        _sparse = std::move(sparse);
        return *this;
    }

    ///
    /// The current sparse setting.
    ///
    /// @return The current sparse setting.
    ///
    bsoncxx::v_noabi::stdx::optional<bool> const& sparse() const {
        return _sparse;
    }

    ///
    /// Specifies the storage engine options for the index.
    ///
    /// @important This option is overridden by `storage_engine` when set.
    ///
    /// @param storage_options
    ///   The storage engine options for the index.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    /// @deprecated Use @ref mongocxx::v_noabi::options::index::storage_engine instead.
    ///
    MONGOCXX_DEPRECATED index& storage_options(std::unique_ptr<base_storage_options> storage_options) {
        _storage_options = std::move(storage_options);
        return *this;
    }

    ///
    /// Specifies the WiredTiger-specific storage engine options for the index.
    ///
    /// @important This option is overridden by `storage_engine` when set.
    ///
    /// @param storage_options
    ///   The storage engine options for the index.
    ///
    /// @deprecated Use @ref mongocxx::v_noabi::options::index::storage_engine instead.
    ///
    MONGOCXX_DEPRECATED index& storage_options(std::unique_ptr<wiredtiger_storage_options> storage_options) {
        _storage_options = std::move(storage_options);
        return *this;
    }

    ///
    /// Specifies the storage engine options for the index.
    ///
    /// @important This option overrides `storage_options` when set.
    ///
    /// The document must have the form `{ <storage-engine-name>: <options> }`, e.g.:
    /// ```json
    /// { "wiredTiger": {"configString": "block_compressor=zlib"} }
    /// ```
    ///
    /// @param storage_engine
    ///   The storage engine options for the index.
    ///
    /// @see
    /// - [Specifying Storage Engine Options (MongoDB Manual)](https://www.mongodb.com/docs/manual/reference/method/db.createCollection/#std-label-create-collection-storage-engine-options)
    /// - [Storage Engines for Self-Managed Deployments (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/storage-engines/)
    ///
    index& storage_engine(bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> storage_engine) {
        _storage_engine = std::move(storage_engine);
        return *this;
    }

    ///
    /// The current storage engine options.
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> const& storage_engine() const {
        return _storage_engine;
    }

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
    index& expire_after(std::chrono::seconds seconds) {
        _expire_after = std::move(seconds);
        return *this;
    }

    ///
    /// The current expire_after setting.
    ///
    /// @return The current expire_after value.
    ///
    bsoncxx::v_noabi::stdx::optional<std::chrono::seconds> const& expire_after() const {
        return _expire_after;
    }

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
    index& version(std::int32_t v) {
        _version = std::move(v);
        return *this;
    }

    ///
    /// The current index version.
    ///
    /// @return The current index version.
    ///
    bsoncxx::v_noabi::stdx::optional<std::int32_t> const& version() const {
        return _version;
    }

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
    index& weights(bsoncxx::v_noabi::document::view weights) {
        _weights = std::move(weights);
        return *this;
    }

    ///
    /// The current weights setting.
    ///
    /// @return The current weights.
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> const& weights() const {
        return _weights;
    }

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
    index& default_language(bsoncxx::v_noabi::string::view_or_value default_language) {
        _default_language = std::move(default_language);
        return *this;
    }

    ///
    /// The current default_language setting.
    ///
    /// @return The current default_language.
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> const& default_language() const {
        return _default_language;
    }

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
    index& language_override(bsoncxx::v_noabi::string::view_or_value language_override) {
        _language_override = std::move(language_override);
        return *this;
    }

    ///
    /// The current name of the field that contains the override language for text indexes.
    ///
    /// @return The name of the field that contains the override language for text indexes.
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> const& language_override() const {
        return _language_override;
    }

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
    index& partial_filter_expression(bsoncxx::v_noabi::document::view partial_filter_expression) {
        _partial_filter_expression = std::move(partial_filter_expression);
        return *this;
    }

    ///
    /// The current partial_filter_expression setting.
    ///
    /// @return The current partial_filter_expression.
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> const& partial_filter_expression() const {
        return _partial_filter_expression;
    }

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
    index& twod_sphere_version(std::uint8_t twod_sphere_version) {
        _twod_sphere_version = std::move(twod_sphere_version);
        return *this;
    }

    ///
    /// The current twod_sphere_version setting.
    ///
    /// @return The current twod_sphere_version.
    ///
    bsoncxx::v_noabi::stdx::optional<std::uint8_t> const& twod_sphere_version() const {
        return _twod_sphere_version;
    }

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
    index& twod_bits_precision(std::uint8_t twod_bits_precision) {
        _twod_bits_precision = std::move(twod_bits_precision);
        return *this;
    }

    ///
    /// The current precision of the stored geohash value of the location data.
    ///
    /// @return The precision of the stored geohash value of the location data.
    ///
    bsoncxx::v_noabi::stdx::optional<std::uint8_t> const& twod_bits_precision() const {
        return _twod_bits_precision;
    }

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
    index& twod_location_min(double twod_location_min) {
        _twod_location_min = std::move(twod_location_min);
        return *this;
    }

    ///
    /// The current lower inclusive boundary for the longitude and latitude values.
    ///
    /// @return The lower inclusive boundary for the longitude and latitude values.
    ///
    bsoncxx::v_noabi::stdx::optional<double> const& twod_location_min() const {
        return _twod_location_min;
    }

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
    index& twod_location_max(double twod_location_max) {
        _twod_location_max = std::move(twod_location_max);
        return *this;
    }

    ///
    /// The current upper inclusive boundary for the longitude and latitude values.
    ///
    /// @return The upper inclusive boundary for the longitude and latitude values.
    ///
    bsoncxx::v_noabi::stdx::optional<double> const& twod_location_max() const {
        return _twod_location_max;
    }

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
    MONGOCXX_DEPRECATED index& haystack_bucket_size(double haystack_bucket_size) {
        return this->haystack_bucket_size_deprecated(haystack_bucket_size);
    }

    index& haystack_bucket_size_deprecated(double haystack_bucket_size) {
        _haystack_bucket_size = haystack_bucket_size;
        return *this;
    }

    ///
    /// The current haystack_bucket_size setting.
    ///
    /// @return The current haystack_bucket_size.
    ///
    /// @deprecated
    ///   This method is deprecated.
    ///
    MONGOCXX_DEPRECATED bsoncxx::v_noabi::stdx::optional<double> const& haystack_bucket_size() const {
        return this->haystack_bucket_size_deprecated();
    }

    bsoncxx::v_noabi::stdx::optional<double> const& haystack_bucket_size_deprecated() const {
        return _haystack_bucket_size;
    }

    ///
    /// Conversion operator that provides a view of the options in document form.
    ///
    /// @exception mongocxx::v_noabi::logic_error if an invalid expireAfterSeconds field is
    /// provided.
    ///
    /// @return A view of the current builder contents.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() operator bsoncxx::v_noabi::document::view_or_value();

    class internal;

   private:
    bsoncxx::v_noabi::stdx::optional<bool> _background;
    bsoncxx::v_noabi::stdx::optional<bool> _unique;
    bsoncxx::v_noabi::stdx::optional<bool> _hidden;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::string::view_or_value> _name;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> _collation;
    bsoncxx::v_noabi::stdx::optional<bool> _sparse;
    std::unique_ptr<base_storage_options> _storage_options;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> _storage_engine;
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
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::options::index from_v1(v1::indexes::options const& v) {
    return {v};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::indexes::options to_v1(v_noabi::options::index const& v) {
    return v1::indexes::options{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <bsoncxx/config/postlude.hpp>

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::index.
///
/// @par Includes
/// - @ref mongocxx/v1/indexes.hpp
///
