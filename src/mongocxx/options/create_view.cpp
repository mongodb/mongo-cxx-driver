// Copyright 2016 MongoDB Inc.
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

#include <mongocxx/options/create_view.hpp>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/types.hpp>

#include <mongocxx/config/private/prelude.hh>

using bsoncxx::builder::basic::kvp;

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

create_view& create_view::collation(bsoncxx::document::view_or_value collation) {
    _collation = std::move(collation);
    return *this;
}

create_view& create_view::pipeline(class pipeline pipeline) {
    _pipeline = std::move(pipeline);
    return *this;
}

create_view& create_view::write_concern(mongocxx::write_concern write_concern) {
    _write_concern = std::move(write_concern);
    return *this;
}

const stdx::optional<bsoncxx::document::view_or_value>& create_view::collation() const {
    return _collation;
}

const stdx::optional<pipeline>& create_view::pipeline() const {
    return _pipeline;
}

const stdx::optional<class write_concern>& create_view::write_concern() const {
    return _write_concern;
}

bsoncxx::document::value create_view::to_document_deprecated() const {
    auto doc = bsoncxx::builder::basic::document{};

    if (_collation) {
        doc.append(kvp("collation", bsoncxx::types::b_document{*_collation}));
    }

    if (_pipeline) {
        doc.append(kvp("pipeline", _pipeline->view_array()));
    }

    return doc.extract();
}

bsoncxx::document::value create_view::to_document() const {
    return to_document_deprecated();
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
