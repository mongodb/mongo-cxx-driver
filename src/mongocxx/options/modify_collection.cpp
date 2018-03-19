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

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/concatenate.hpp>
#include <bsoncxx/types.hpp>

#include <mongocxx/config/private/prelude.hh>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::concatenate;

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

modify_collection& modify_collection::no_padding(bool no_padding) {
    _no_padding = no_padding;
    return *this;
}

modify_collection& modify_collection::index(bsoncxx::document::view_or_value index_spec,
                                            std::chrono::seconds seconds) {
    _index.emplace(
        make_document(kvp("keyPattern", bsoncxx::types::b_document{std::move(index_spec)}),
                      kvp("expireAfterSeconds", bsoncxx::types::b_int64{seconds.count()})));
    return *this;
}

modify_collection& modify_collection::validation_criteria(class validation_criteria validation) {
    _validation = std::move(validation);
    return *this;
}

bsoncxx::document::value modify_collection::to_document() const {
    bsoncxx::builder::basic::document doc;

    if (_index) {
        doc.append(kvp("index", bsoncxx::types::b_document{_index->view()}));
    }

    if (_no_padding) {
        doc.append(kvp("noPadding", *_no_padding));
    }

    if (_validation) {
        doc.append(concatenate(_validation->to_document_deprecated()));
    }

    return doc.extract();
}

bool MONGOCXX_CALL operator==(const modify_collection& lhs, const modify_collection& rhs) {
    if (lhs._index && rhs._index) {
        if (lhs._index.value() != rhs._index.value()) {
            return false;
        }
    } else if (lhs._index || rhs._index) {
        return false;
    }

    if (lhs._no_padding != rhs._no_padding) {
        return false;
    }

    if (lhs._validation && rhs._validation) {
        return lhs._validation.value() == rhs._validation.value();
    } else if (lhs._validation || rhs._validation) {
        return false;
    }

    return true;
}
bool MONGOCXX_CALL operator!=(const modify_collection& lhs, const modify_collection& rhs) {
    return !(lhs == rhs);
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
