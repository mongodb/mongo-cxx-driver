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

#include <mongocxx/hint.hpp>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/sub_document.hpp>
#include <bsoncxx/builder/concatenate.hpp>
#include <bsoncxx/stdx/make_unique.hpp>

#include <mongocxx/config/private/prelude.hh>

using bsoncxx::builder::concatenate;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::sub_document;

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

hint::hint(bsoncxx::document::view_or_value index) : _index_doc(std::move(index)) {}

hint::hint(bsoncxx::string::view_or_value index) : _index_string(std::move(index)) {}

bsoncxx::types::value hint::to_value() const {
    if (_index_doc) {
        return bsoncxx::types::value{bsoncxx::types::b_document{_index_doc->view()}};
    }

    return bsoncxx::types::value{bsoncxx::types::b_utf8{*_index_string}};
}

bsoncxx::document::value hint::to_document_deprecated() const {
    auto doc = bsoncxx::builder::basic::document{};

    if (_index_doc) {
        doc.append(kvp("$hint", *_index_doc));
    } else {
        doc.append(kvp("$hint", *_index_string));
    }

    return doc.extract();
}

bsoncxx::document::value hint::to_document() const {
    return to_document_deprecated();
}

bool MONGOCXX_CALL operator==(const hint& index_hint, std::string index) {
    return ((index_hint._index_string) && (*(index_hint._index_string) == index));
}

bool MONGOCXX_CALL operator==(std::string index, const hint& index_hint) {
    return index_hint == index;
}

bool MONGOCXX_CALL operator!=(const hint& index_hint, std::string index) {
    return !(index_hint == index);
}

bool MONGOCXX_CALL operator!=(std::string index, const hint& index_hint) {
    return !(index_hint == index);
}

bool MONGOCXX_CALL operator==(const hint& index_hint, bsoncxx::document::view index) {
    return index_hint._index_doc && index_hint._index_doc->view() == index;
}

bool MONGOCXX_CALL operator==(bsoncxx::document::view index, const hint& index_hint) {
    return index_hint == index;
}

bool MONGOCXX_CALL operator!=(const hint& index_hint, bsoncxx::document::view index) {
    return !(index_hint == index);
}

bool MONGOCXX_CALL operator!=(bsoncxx::document::view index, const hint& index_hint) {
    return !(index_hint == index);
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
