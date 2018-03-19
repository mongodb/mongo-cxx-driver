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

#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/sub_document.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/private/pipeline.hh>
#include <mongocxx/stdx.hpp>

#include <mongocxx/config/private/prelude.hh>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::sub_document;

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

pipeline::pipeline() : _impl(stdx::make_unique<impl>()) {}

pipeline::pipeline(pipeline&&) noexcept = default;
pipeline& pipeline::operator=(pipeline&&) noexcept = default;
pipeline::~pipeline() = default;

pipeline& pipeline::add_fields(bsoncxx::document::view_or_value fields_to_add) {
    _impl->sink().append([fields_to_add](sub_document sub_doc) {
        sub_doc.append(kvp("$addFields", fields_to_add));
    });

    return *this;
}

pipeline& pipeline::bucket(bsoncxx::document::view_or_value bucket_args) {
    _impl->sink().append(
        [bucket_args](sub_document sub_doc) { sub_doc.append(kvp("$bucket", bucket_args)); });

    return *this;
}

pipeline& pipeline::bucket_auto(bsoncxx::document::view_or_value bucket_auto_args) {
    _impl->sink().append([bucket_auto_args](sub_document sub_doc) {
        sub_doc.append(kvp("$bucketAuto", bucket_auto_args));
    });

    return *this;
}

pipeline& pipeline::coll_stats(bsoncxx::document::view_or_value coll_stats_args) {
    _impl->sink().append([coll_stats_args](sub_document sub_doc) {
        sub_doc.append(kvp("$collStats", coll_stats_args));
    });

    return *this;
}

pipeline& pipeline::count(std::string field) {
    _impl->sink().append([field](sub_document sub_doc) { sub_doc.append(kvp("$count", field)); });

    return *this;
}

pipeline& pipeline::facet(bsoncxx::document::view_or_value facet_args) {
    _impl->sink().append(
        [facet_args](sub_document sub_doc) { sub_doc.append(kvp("$facet", facet_args)); });

    return *this;
}

pipeline& pipeline::geo_near(bsoncxx::document::view_or_value geo_near_args) {
    _impl->sink().append(
        [geo_near_args](sub_document sub_doc) { sub_doc.append(kvp("$geoNear", geo_near_args)); });

    return *this;
}

pipeline& pipeline::graph_lookup(bsoncxx::document::view_or_value graph_lookup_args) {
    _impl->sink().append([graph_lookup_args](sub_document sub_doc) {
        sub_doc.append(kvp("$graphLookup", graph_lookup_args));
    });

    return *this;
}

pipeline& pipeline::group(bsoncxx::document::view_or_value group_args) {
    _impl->sink().append(
        [group_args](sub_document sub_doc) { sub_doc.append(kvp("$group", group_args)); });

    return *this;
}

pipeline& pipeline::index_stats() {
    _impl->sink().append([](sub_document sub_doc) {
        sub_doc.append(kvp("$indexStats", bsoncxx::document::view{}));
    });

    return *this;
}

pipeline& pipeline::limit(std::int32_t limit) {
    _impl->sink().append([limit](sub_document sub_doc) { sub_doc.append(kvp("$limit", limit)); });

    return *this;
}

pipeline& pipeline::lookup(bsoncxx::document::view_or_value lookup_args) {
    _impl->sink().append(
        [lookup_args](sub_document sub_doc) { sub_doc.append(kvp("$lookup", lookup_args)); });

    return *this;
}

pipeline& pipeline::match(bsoncxx::document::view_or_value filter) {
    _impl->sink().append([filter](sub_document sub_doc) { sub_doc.append(kvp("$match", filter)); });

    return *this;
}

pipeline& pipeline::out(std::string collection_name) {
    _impl->sink().append(
        [collection_name](sub_document sub_doc) { sub_doc.append(kvp("$out", collection_name)); });

    return *this;
}

pipeline& pipeline::project(bsoncxx::document::view_or_value projection) {
    _impl->sink().append(
        [projection](sub_document sub_doc) { sub_doc.append(kvp("$project", projection)); });

    return *this;
}

pipeline& pipeline::redact(bsoncxx::document::view_or_value restrictions) {
    _impl->sink().append(
        [restrictions](sub_document sub_doc) { sub_doc.append(kvp("$redact", restrictions)); });

    return *this;
}

pipeline& pipeline::replace_root(bsoncxx::document::view_or_value replace_root_args) {
    _impl->sink().append([replace_root_args](sub_document sub_doc) {
        sub_doc.append(kvp("$replaceRoot", replace_root_args));
    });

    return *this;
}

pipeline& pipeline::sample(std::int32_t size) {
    _impl->sink().append([size](sub_document sub_doc1) {
        sub_doc1.append(
            kvp("$sample", [size](sub_document sub_doc2) { sub_doc2.append(kvp("size", size)); }));
    });

    return *this;
}

pipeline& pipeline::skip(std::int32_t docs_to_skip) {
    _impl->sink().append(
        [docs_to_skip](sub_document sub_doc) { sub_doc.append(kvp("$skip", docs_to_skip)); });

    return *this;
}

pipeline& pipeline::sort(bsoncxx::document::view_or_value ordering) {
    _impl->sink().append(
        [ordering](sub_document sub_doc) { sub_doc.append(kvp("$sort", ordering)); });

    return *this;
}

pipeline& pipeline::sort_by_count(bsoncxx::document::view_or_value field_expression) {
    _impl->sink().append([field_expression](sub_document sub_doc) {
        sub_doc.append(kvp("$sortByCount", field_expression));
    });

    return *this;
}

pipeline& pipeline::sort_by_count(std::string field_expression) {
    _impl->sink().append([field_expression](sub_document sub_doc) {
        sub_doc.append(kvp("$sortByCount", field_expression));
    });

    return *this;
}

pipeline& pipeline::unwind(bsoncxx::document::view_or_value unwind_args) {
    _impl->sink().append(
        [unwind_args](sub_document sub_doc) { sub_doc.append(kvp("$unwind", unwind_args)); });

    return *this;
}

pipeline& pipeline::unwind(std::string field_name) {
    _impl->sink().append(
        [field_name](sub_document sub_doc) { sub_doc.append(kvp("$unwind", field_name)); });

    return *this;
}

bsoncxx::document::view pipeline::view_deprecated() const {
    return _impl->view();
}

bsoncxx::document::view pipeline::view() const {
    return view_deprecated();
}

bsoncxx::array::view pipeline::view_array() const {
    return _impl->view_array();
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
