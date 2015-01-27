// Copyright 2014 MongoDB Inc.
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

#include "bson/libbson.hpp"

namespace bson {
namespace libbson {

static void doc_to_bson_t(const bson::document::view& doc, bson_t* bson) {
    bson_init_static(bson, doc.get_buf(), doc.get_len());
}

static void optional_doc_to_bson_t(const mongo::driver::optional<bson::document::view>& doc,
                                   bson_t* bson) {
    if (doc) {
        doc_to_bson_t(*doc, bson);
    }
}

scoped_bson_t::scoped_bson_t(const mongo::driver::optional<bson::document::view>& doc)
    : _is_initialized(doc) {
    optional_doc_to_bson_t(doc, &_bson);
}

scoped_bson_t::scoped_bson_t(const bson::document::view& doc) : _is_initialized(true) {
    doc_to_bson_t(doc, &_bson);
}

void scoped_bson_t::init_from_static(const mongo::driver::optional<bson::document::view>& doc) {
    _is_initialized = static_cast<bool>(doc);
    optional_doc_to_bson_t(doc, &_bson);
}

void scoped_bson_t::init_from_static(const bson::document::view& doc) {
    _is_initialized = true;
    doc_to_bson_t(doc, &_bson);
}

void scoped_bson_t::init() {
    _is_initialized = true;
    bson_init(&_bson);
}

void scoped_bson_t::flag_init() { _is_initialized = true; }

scoped_bson_t::scoped_bson_t() : _is_initialized(false) {}

scoped_bson_t::~scoped_bson_t() {
    if (_is_initialized) {
        bson_destroy(&_bson);
    }
}

bson_t* scoped_bson_t::bson() { return _is_initialized ? &_bson : nullptr; }

bson::document::view scoped_bson_t::view() {
    return _is_initialized ? bson::document::view(bson_get_data(bson()), bson()->len)
                           : bson::document::view();
}

bson::document::value scoped_bson_t::steal() {
    if (!_is_initialized) {
        return bson::document::view();
    }

    std::uint32_t length;
    std::uint8_t* buff = bson_destroy_with_steal(bson(), true, &length);

    return bson::document::value(buff, length, bson_free);
}

}  // namespace libbson
}  // namespace bson
