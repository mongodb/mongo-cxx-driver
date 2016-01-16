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

#include <bsoncxx/stdx/make_unique.hpp>

#include <mongocxx/config/private/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

hint::hint(bsoncxx::document::view_or_value index) : _index_doc(std::move(index)) {
}

hint::hint(bsoncxx::string::view_or_value index) : _index_string(std::move(index)) {
}

bsoncxx::document::value hint::to_document() const {
    auto doc = bsoncxx::builder::stream::document{};

    if (_index_doc) {
        doc << "$hint" << bsoncxx::builder::stream::open_document
            << bsoncxx::builder::stream::concatenate(*_index_doc)
            << bsoncxx::builder::stream::close_document;
    } else {
        doc << "$hint" << *_index_string;
    }

    return doc.extract();
}

bool operator==(const hint& index_hint, std::string index) {
    return ((index_hint._index_string) && (*(index_hint._index_string) == index));
}

bool operator==(std::string index, const hint& index_hint) {
    return index_hint == index;
}

bool operator!=(const hint& index_hint, std::string index) {
    return !(index_hint == index);
}

bool operator!=(std::string index, const hint& index_hint) {
    return !(index_hint == index);
}

bool operator==(const hint& index_hint, bsoncxx::document::view index) {
    return index_hint._index_doc && index_hint._index_doc->view() == index;
}

bool operator==(bsoncxx::document::view index, const hint& index_hint) {
    return index_hint == index;
}

bool operator!=(const hint& index_hint, bsoncxx::document::view index) {
    return !(index_hint == index);
}

bool operator!=(bsoncxx::document::view index, const hint& index_hint) {
    return !(index_hint == index);
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
