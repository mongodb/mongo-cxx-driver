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

#include <mongocxx/pipeline.hpp>

#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/private/pipeline.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

using namespace bsoncxx::builder::stream;
using namespace bsoncxx::types;

pipeline::pipeline() : _impl(bsoncxx::stdx::make_unique<impl>()) {
}

pipeline::pipeline(pipeline&&) noexcept = default;
pipeline& pipeline::operator=(pipeline&&) noexcept = default;
pipeline::~pipeline() = default;

pipeline& pipeline::group(bsoncxx::document::view group) {
    _impl->sink() << open_document << "$group" << b_document{group} << close_document;
    return *this;
}

pipeline& pipeline::limit(std::int32_t limit) {
    _impl->sink() << open_document << "$limit" << limit << close_document;
    return *this;
}

pipeline& pipeline::match(bsoncxx::document::view criteria) {
    _impl->sink() << open_document << "$match" << b_document{criteria} << close_document;
    return *this;
}

pipeline& pipeline::out(std::string collection_name) {
    _impl->sink() << open_document << "$out" << collection_name << close_document;
    return *this;
}

pipeline& pipeline::project(bsoncxx::document::view projection) {
    _impl->sink() << open_document << "$project" << b_document{projection} << close_document;
    return *this;
}

pipeline& pipeline::redact(bsoncxx::document::view restrictions) {
    _impl->sink() << open_document << "$redact" << b_document{restrictions} << close_document;
    return *this;
}

pipeline& pipeline::skip(std::int32_t skip) {
    _impl->sink() << open_document << "$skip" << skip << close_document;
    return *this;
}

pipeline& pipeline::sort(bsoncxx::document::view sort) {
    _impl->sink() << open_document << "$sort" << b_document{sort} << close_document;
    return *this;
}

pipeline& pipeline::unwind(std::string field_name) {
    _impl->sink() << open_document << "$unwind" << field_name << close_document;
    return *this;
}

bsoncxx::document::view pipeline::view() const {
    return _impl->view();
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
