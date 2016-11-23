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

#include <mongocxx/options/create_collection.hpp>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/types.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

create_collection& create_collection::auto_index_id(bool auto_index_id) {
    _auto_index_id = auto_index_id;
    return *this;
}

create_collection& create_collection::capped(bool capped) {
    _capped = capped;
    return *this;
}

create_collection& create_collection::collation(bsoncxx::document::view_or_value collation) {
    _collation = std::move(collation);
    return *this;
}

create_collection& create_collection::max(int max_documents) {
    _max_documents = max_documents;
    return *this;
}

create_collection& create_collection::no_padding(bool no_padding) {
    _no_padding = no_padding;
    return *this;
}

create_collection& create_collection::size(int max_size) {
    _max_size = max_size;
    return *this;
}

create_collection& create_collection::storage_engine(
    bsoncxx::document::view_or_value storage_engine_opts) {
    _storage_engine_opts = std::move(storage_engine_opts);
    return *this;
}

create_collection& create_collection::validation_criteria(class validation_criteria validation) {
    _validation = std::move(validation);
    return *this;
}

const stdx::optional<bool>& create_collection::auto_index_id() const {
    return _auto_index_id;
}

const stdx::optional<bool>& create_collection::capped() const {
    return _capped;
}

const stdx::optional<bsoncxx::document::view_or_value>& create_collection::collation() const {
    return _collation;
}

const stdx::optional<int>& create_collection::max() const {
    return _max_documents;
}

const stdx::optional<bool>& create_collection::no_padding() const {
    return _no_padding;
}

const stdx::optional<int>& create_collection::size() const {
    return _max_size;
}

const stdx::optional<bsoncxx::document::view_or_value>& create_collection::storage_engine() const {
    return _storage_engine_opts;
}

const stdx::optional<class validation_criteria>& create_collection::validation_criteria() const {
    return _validation;
}

bsoncxx::document::value create_collection::to_document() const {
    auto doc = bsoncxx::builder::stream::document{};

    if (_auto_index_id) {
        doc << "autoIndexId" << *_auto_index_id;
    }

    if (_capped) {
        doc << "capped" << *_capped;
    }

    if (_collation) {
        doc << "collation" << bsoncxx::types::b_document{*_collation};
    }

    if (_max_documents) {
        doc << "max" << *_max_documents;
    }

    if (_max_size) {
        doc << "size" << *_max_size;
    }

    if (_no_padding) {
        doc << "flags" << (*_no_padding ? 0x10 : 0x00);
    }

    if (_storage_engine_opts) {
        doc << "storageEngine" << bsoncxx::types::b_document{*_storage_engine_opts};
    }

    if (_validation) {
        doc << bsoncxx::builder::stream::concatenate(_validation->to_document());
    }

    return doc.extract();
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
