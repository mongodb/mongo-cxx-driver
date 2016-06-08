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

#include <mongocxx/config/private/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

create_collection& create_collection::capped(bool capped) {
    _capped = capped;
    return *this;
}

create_collection& create_collection::auto_index_id(bool auto_index_id) {
    _auto_index_id = auto_index_id;
    return *this;
}

create_collection& create_collection::size(int max_size) {
    _max_size = max_size;
    return *this;
}

create_collection& create_collection::max(int max_documents) {
    _max_documents = max_documents;
    return *this;
}

create_collection& create_collection::storage_engine(
    bsoncxx::document::view_or_value storage_engine_opts) {
    _storage_engine_opts = std::move(storage_engine_opts);
    return *this;
}

create_collection& create_collection::no_padding(bool no_padding) {
    _no_padding = no_padding;
    return *this;
}

create_collection& create_collection::validation_criteria(class validation_criteria validation) {
    _validation = std::move(validation);
    return *this;
}

bsoncxx::document::value create_collection::to_document() const {
    auto doc = bsoncxx::builder::stream::document{};

    if (_capped) {
        doc << "capped" << *_capped;
    }

    if (_auto_index_id) {
        doc << "autoIndexId" << *_auto_index_id;
    }

    if (_max_size) {
        doc << "size" << *_max_size;
    }

    if (_max_documents) {
        doc << "max" << *_max_documents;
    }

    if (_storage_engine_opts) {
        doc << "storageEngine" << bsoncxx::types::b_document{*_storage_engine_opts};
    }

    if (_no_padding) {
        doc << "noPadding" << *_no_padding;
    }

    if (_validation) {
        doc << bsoncxx::builder::stream::concatenate(_validation->to_document());
    }

    return doc.extract();
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
