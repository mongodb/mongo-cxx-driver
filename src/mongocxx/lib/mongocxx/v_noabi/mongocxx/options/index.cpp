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

#include <mongocxx/options/index.hh>

//

#include <mongocxx/v1/indexes.hh>

#include <cstdint>
#include <limits>
#include <utility>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view_or_value.hpp>

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>

#include <mongocxx/scoped_bson.hh>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/make_unique.hh>

namespace mongocxx {
namespace v_noabi {
namespace options {

index::index(v1::indexes::options const& opts)
    : _background{opts.background()},
      _unique{opts.unique()},
      _hidden{opts.hidden()},
      _name{opts.name()},
      _collation{opts.collation()},
      _sparse{opts.sparse()},
      _storage_options{},
      _storage_engine{opts.storage_engine()},
      _expire_after{opts.expire_after()},
      _version{opts.version()},
      _weights{opts.weights()},
      _default_language{opts.default_language()},
      _language_override{opts.language_override()},
      _partial_filter_expression{opts.partial_filter_expression()},
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
    scoped_bson ret;

    if (_name) {
        ret += scoped_bson{BCON_NEW("name", BCON_UTF8(_name->terminated().data()))};
    }

    if (_background) {
        ret += scoped_bson{BCON_NEW("background", BCON_BOOL(*_background))};
    }

    if (_unique) {
        ret += scoped_bson{BCON_NEW("unique", BCON_BOOL(*_unique))};
    }

    if (_hidden) {
        ret += scoped_bson{BCON_NEW("hidden", BCON_BOOL(*_hidden))};
    }

    if (_partial_filter_expression) {
        ret += scoped_bson{BCON_NEW(
            "partialFilterExpression", BCON_DOCUMENT(to_scoped_bson_view(*_partial_filter_expression).bson()))};
    }

    if (_sparse) {
        ret += scoped_bson{BCON_NEW("sparse", BCON_BOOL(*_sparse))};
    }

    if (_expire_after) {
        auto const count = _expire_after->count();

        if (count < 0 || count > std::numeric_limits<std::int32_t>::max()) {
            throw v_noabi::logic_error{v_noabi::error_code::k_invalid_parameter};
        }

        ret += scoped_bson{BCON_NEW("expireAfterSeconds", BCON_INT32(static_cast<std::int32_t>(count)))};
    }

    if (_weights) {
        ret += scoped_bson{BCON_NEW("weights", BCON_DOCUMENT(to_scoped_bson_view(*_weights).bson()))};
    }

    if (_default_language) {
        ret += scoped_bson{BCON_NEW("default_language", BCON_UTF8(_default_language->terminated().data()))};
    }

    if (_language_override) {
        ret += scoped_bson{BCON_NEW("language_override", BCON_UTF8(_language_override->terminated().data()))};
    }

    if (_twod_sphere_version) {
        ret += scoped_bson{BCON_NEW("2dsphereIndexVersion", BCON_INT32(*_twod_sphere_version))};
    }

    if (_twod_bits_precision) {
        ret += scoped_bson{BCON_NEW("bits", BCON_INT32(*_twod_bits_precision))};
    }

    if (_twod_location_min) {
        ret += scoped_bson{BCON_NEW("min", BCON_DOUBLE(*_twod_location_min))};
    }

    if (_twod_location_max) {
        ret += scoped_bson{BCON_NEW("max", BCON_DOUBLE(*_twod_location_max))};
    }

    if (_haystack_bucket_size) {
        ret += scoped_bson{BCON_NEW("bucketSize", BCON_DOUBLE(*_haystack_bucket_size))};
    }

    if (_collation) {
        ret += scoped_bson{BCON_NEW("collation", BCON_DOCUMENT(to_scoped_bson_view(*_collation).bson()))};
    }

    if (_storage_engine) {
        ret += scoped_bson{BCON_NEW("storageEngine", BCON_DOCUMENT(to_scoped_bson_view(*_storage_engine).bson()))};
    } else if (_storage_options) {
        if (auto const wt_options =
                dynamic_cast<options::index::wiredtiger_storage_options const*>(_storage_options.get())) {
            scoped_bson v;

            if (auto const& opt = wt_options->config_string()) {
                v += scoped_bson{BCON_NEW(
                    "storageEngine",
                    "{",
                    "wiredTiger",
                    "{",
                    "configString",
                    BCON_UTF8(opt->terminated().data()),
                    "}",
                    "}")};
            } else {
                v +=
                    scoped_bson{BCON_NEW("storageEngine", "{", "wiredTiger", "{", "configString", BCON_NULL, "}", "}")};
            }

            ret += v;
        }
    }

    return bsoncxx::v_noabi::from_v1(std::move(ret).value());
}

index::base_storage_options::~base_storage_options() = default;

index::wiredtiger_storage_options::~wiredtiger_storage_options() = default;

int index::wiredtiger_storage_options::type() const {
    return static_cast<int>(mongoc_index_storage_opt_type_t::MONGOC_INDEX_STORAGE_OPT_WIREDTIGER);
}

std::unique_ptr<index::base_storage_options> const& index::internal::storage_options(index const& self) {
    return self._storage_options;
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
