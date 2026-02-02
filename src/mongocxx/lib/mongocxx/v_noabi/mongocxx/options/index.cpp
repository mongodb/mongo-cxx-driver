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

#include <mongocxx/options/index.hpp>

//

#include <mongocxx/v1/indexes.hh>

#include <cstdint>
#include <limits>
#include <utility>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>

#include <mongocxx/scoped_bson.hh>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/make_unique.hh>

namespace mongocxx {
namespace v_noabi {
namespace options {

index::index(v1::indexes::options opts)
    : _background{opts.background()},
      _unique{opts.unique()},
      _hidden{opts.hidden()},
      _name{std::move(v1::indexes::options::internal::name(opts))},
      _collation{[&]() -> decltype(_collation) {
          if (auto& opt = v1::indexes::options::internal::collation(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }

          return {};
      }()},
      _sparse{opts.sparse()},
      _storage_options{},
      _storage_engine{std::move(v1::indexes::options::internal::storage_engine(opts))},
      _expire_after{opts.expire_after()},
      _version{opts.version()},
      _weights{[&]() -> decltype(_weights) {
          if (auto& opt = v1::indexes::options::internal::weights(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }

          return {};
      }()},
      _default_language{v1::indexes::options::internal::default_language(opts)},
      _language_override{v1::indexes::options::internal::language_override(opts)},
      _partial_filter_expression{[&]() -> decltype(_partial_filter_expression) {
          if (auto& opt = v1::indexes::options::internal::partial_filter_expression(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }

          return {};
      }()},
      _twod_sphere_version{opts.twod_sphere_version()},
      _twod_bits_precision{opts.twod_bits_precision()},
      _twod_location_min{opts.twod_location_min()},
      _twod_location_max{opts.twod_location_max()},
      _haystack_bucket_size{} {}

// CDRIVER-5946: mongoc_index_storage_opt_type_t was removed in mongoc 2.0.
enum struct mongoc_index_storage_opt_type_t {
    MONGOC_INDEX_STORAGE_OPT_MMAPV1,
    MONGOC_INDEX_STORAGE_OPT_WIREDTIGER,
};

index::operator bsoncxx::v_noabi::document::view_or_value() {
    scoped_bson doc;

    if (_name) {
        doc += scoped_bson{BCON_NEW("name", BCON_UTF8(_name->terminated().data()))};
    }

    if (_background) {
        doc += scoped_bson{BCON_NEW("background", BCON_BOOL(*_background))};
    }

    if (_unique) {
        doc += scoped_bson{BCON_NEW("unique", BCON_BOOL(*_unique))};
    }

    if (_hidden) {
        doc += scoped_bson{BCON_NEW("hidden", BCON_BOOL(*_hidden))};
    }

    if (_partial_filter_expression) {
        doc += scoped_bson{BCON_NEW(
            "partialFilterExpression", BCON_DOCUMENT(to_scoped_bson_view(*_partial_filter_expression).bson()))};
    }

    if (_sparse) {
        doc += scoped_bson{BCON_NEW("sparse", BCON_BOOL(*_sparse))};
    }

    if (_expire_after) {
        auto const count = _expire_after->count();

        if (count < 0 || (count > std::numeric_limits<std::int32_t>::max())) {
            throw logic_error{error_code::k_invalid_parameter};
        }

        doc += scoped_bson{BCON_NEW("expireAfterSeconds", BCON_INT32(static_cast<std::int32_t>(count)))};
    }

    if (_weights) {
        doc += scoped_bson{BCON_NEW("weights", BCON_DOCUMENT(to_scoped_bson_view(*_weights).bson()))};
    }

    if (_default_language) {
        doc += scoped_bson{BCON_NEW("default_language", BCON_UTF8(_default_language->terminated().data()))};
    }

    if (_language_override) {
        doc += scoped_bson{BCON_NEW("language_override", BCON_UTF8(_language_override->terminated().data()))};
    }

    if (_twod_sphere_version) {
        doc += scoped_bson{BCON_NEW("2dsphereIndexVersion", BCON_INT32(std::int32_t{*_twod_sphere_version}))};
    }

    if (_twod_bits_precision) {
        doc += scoped_bson{BCON_NEW("bits", BCON_INT32(std::int32_t{*_twod_bits_precision}))};
    }

    if (_twod_location_min) {
        doc += scoped_bson{BCON_NEW("min", BCON_DOUBLE(*_twod_location_min))};
    }

    if (_twod_location_max) {
        doc += scoped_bson{BCON_NEW("max", BCON_DOUBLE(*_twod_location_max))};
    }

    if (_haystack_bucket_size) {
        doc += scoped_bson{BCON_NEW("bucketSize", BCON_DOUBLE(*_haystack_bucket_size))};
    }

    if (_collation) {
        doc += scoped_bson{BCON_NEW("collation", BCON_DOCUMENT(to_scoped_bson_view(*_collation).bson()))};
    }

    if (_storage_engine) {
        doc += scoped_bson{BCON_NEW("storageEngine", BCON_DOCUMENT(to_scoped_bson_view(*_storage_engine).bson()))};
    } else if (_storage_options) {
        if (auto const wt_options =
                dynamic_cast<options::index::wiredtiger_storage_options const*>(_storage_options.get())) {
            scoped_bson storage_doc;
            if (auto const& opt = wt_options->config_string()) {
                storage_doc =
                    scoped_bson{BCON_NEW("wiredTiger", "{", "configString", BCON_UTF8(opt->terminated().data()), "}")};
            } else {
                storage_doc = scoped_bson{BCON_NEW("wiredTiger", "{", "configString", BCON_NULL, "}")};
            }
            doc += scoped_bson{BCON_NEW("storageEngine", BCON_DOCUMENT(storage_doc.bson()))};
        }
    }

    return bsoncxx::v_noabi::from_v1(std::move(doc).value());
}

index::base_storage_options::~base_storage_options() = default;

index::wiredtiger_storage_options::~wiredtiger_storage_options() = default;

int index::wiredtiger_storage_options::type() const {
    return static_cast<int>(mongoc_index_storage_opt_type_t::MONGOC_INDEX_STORAGE_OPT_WIREDTIGER);
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
