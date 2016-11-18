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

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>

#include <mongocxx/config/private/prelude.hh>

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

const stdx::optional<bsoncxx::document::view_or_value>& create_view::collation() const {
    return _collation;
}

const stdx::optional<pipeline>& create_view::pipeline() const {
    return _pipeline;
}

bsoncxx::document::value create_view::to_document() const {
    auto doc = bsoncxx::builder::stream::document{};

    if (_collation) {
        doc << "collation" << bsoncxx::types::b_document{*_collation};
    }

    if (_pipeline) {
        doc << "pipeline" << _pipeline->view_array();
    }

    return doc.extract();
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
