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

#include <mongocxx/options/modify_collection.hpp>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/types.hpp>

#include <mongocxx/config/private/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

using bsoncxx::builder::stream::concatenate;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

modify_collection& modify_collection::no_padding(bool no_padding) {
    _no_padding = no_padding;
    return *this;
}

modify_collection& modify_collection::index(bsoncxx::document::view_or_value index_spec,
                                            std::chrono::seconds seconds) {
    _index.emplace(document{} << "keyPattern" << bsoncxx::types::b_document{std::move(index_spec)}
                              << "expireAfterSeconds" << bsoncxx::types::b_int64{seconds.count()}
                              << finalize);
    return *this;
}

modify_collection& modify_collection::validation_criteria(class validation_criteria validation) {
    _validation = std::move(validation);
    return *this;
}

bsoncxx::document::value modify_collection::to_document() const {
    auto doc = document{};

    if (_index) {
        doc << "index" << bsoncxx::types::b_document{_index->view()};
    }

    if (_no_padding) {
        doc << "noPadding" << *_no_padding;
    }

    if (_validation) {
        doc << concatenate(_validation->to_document());
    }

    return doc.extract();
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
