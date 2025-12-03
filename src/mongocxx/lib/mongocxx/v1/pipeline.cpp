// Copyright 2009-present MongoDB, Inc.
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

#include <mongocxx/v1/pipeline.hh>

//

#include <bsoncxx/v1/array/view.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <array>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <string>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class pipeline::impl {
   private:
    static_assert(INT32_MAX == std::int32_t{2147483647}, "");

    std::array<char, sizeof("2147483647")> _idx = {};

    std::int32_t _count = 0;
    scoped_bson _doc;

   public:
    scoped_bson const& doc() const {
        return _doc;
    }

    char const* idx() {
        (void)std::snprintf(_idx.data(), _idx.size(), "%" PRId32, _count);
        return _idx.data();
    }

    void append(bsoncxx::v1::document::view doc) {
        _doc += scoped_bson{BCON_NEW(this->idx(), BCON_DOCUMENT(scoped_bson_view{doc}.bson()))};
        ++_count;
    }

    void append(char const* name, bsoncxx::v1::document::view doc) {
        _doc += scoped_bson{BCON_NEW(this->idx(), "{", name, BCON_DOCUMENT(scoped_bson_view{doc}.bson()), "}")};
        ++_count;
    }

    void append(char const* name, bsoncxx::v1::stdx::string_view v) {
        _doc += scoped_bson{BCON_NEW(this->idx(), "{", name, BCON_UTF8(std::string{v}.c_str()), "}")};
        ++_count;
    }

    void append(char const* name, std::int32_t v) {
        _doc += scoped_bson{BCON_NEW(this->idx(), "{", name, BCON_INT32(v), "}")};
        ++_count;
    }

    void append(char const* name, std::int64_t v) {
        _doc += scoped_bson{BCON_NEW(this->idx(), "{", name, BCON_INT64(v), "}")};
        ++_count;
    }

    static impl const& with(pipeline const& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl const* with(pipeline const* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl& with(pipeline& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(pipeline* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

pipeline::~pipeline() {
    delete impl::with(this);
}

pipeline::pipeline(pipeline&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

pipeline& pipeline::operator=(pipeline&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

pipeline::pipeline(pipeline const& other) : _impl{new impl{impl::with(other)}} {}

pipeline& pipeline::operator=(pipeline const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

pipeline::pipeline() : _impl{new impl{}} {}

bsoncxx::v1::array::view pipeline::view_array() const {
    return impl::with(this)->doc().array_view();
}

pipeline& pipeline::append_stage(bsoncxx::v1::document::view v) {
    impl::with(this)->append(v);
    return *this;
}

pipeline& pipeline::append_stages(bsoncxx::v1::array::view v) {
    auto& impl = impl::with(*this);

    for (auto const& e : v) {
        impl.append(e.get_document().value);
    }

    return *this;
}

pipeline& pipeline::add_fields(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$addFields", v);
    return *this;
}

pipeline& pipeline::bucket(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$bucket", v);
    return *this;
}

pipeline& pipeline::bucket_auto(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$bucketAuto", v);
    return *this;
}

pipeline& pipeline::coll_stats(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$collStats", v);
    return *this;
}

pipeline& pipeline::coll_stats() {
    impl::with(this)->append("$collStats", bsoncxx::v1::document::view{});
    return *this;
}

pipeline& pipeline::count(bsoncxx::v1::stdx::string_view v) {
    impl::with(this)->append("$count", v);
    return *this;
}

pipeline& pipeline::current_op(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$currentOp", v);
    return *this;
}

pipeline& pipeline::facet(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$facet", v);
    return *this;
}

pipeline& pipeline::geo_near(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$geoNear", v);
    return *this;
}

pipeline& pipeline::graph_lookup(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$graphLookup", v);
    return *this;
}

pipeline& pipeline::group(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$group", v);
    return *this;
}

pipeline& pipeline::index_stats() {
    impl::with(this)->append("$indexStats", bsoncxx::v1::document::view{});
    return *this;
}

pipeline& pipeline::limit(std::int32_t v) {
    impl::with(this)->append("$limit", v);
    return *this;
}

pipeline& pipeline::limit(std::int64_t v) {
    impl::with(this)->append("$limit", v);
    return *this;
}

pipeline& pipeline::list_local_sessions(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$listLocalSessions", v);
    return *this;
}

pipeline& pipeline::list_sessions(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$listSessions", v);
    return *this;
}

pipeline& pipeline::lookup(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$lookup", v);
    return *this;
}

pipeline& pipeline::match(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$match", v);
    return *this;
}

pipeline& pipeline::merge(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$merge", v);
    return *this;
}

pipeline& pipeline::out(bsoncxx::v1::stdx::string_view v) {
    impl::with(this)->append("$out", v);
    return *this;
}

pipeline& pipeline::project(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$project", v);
    return *this;
}

pipeline& pipeline::redact(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$redact", v);
    return *this;
}

pipeline& pipeline::replace_root(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$replaceRoot", v);
    return *this;
}

pipeline& pipeline::sample(std::int32_t v) {
    impl::with(this)->append("$sample", scoped_bson{BCON_NEW("size", BCON_INT32(v))}.view());
    return *this;
}

pipeline& pipeline::skip(std::int32_t v) {
    impl::with(this)->append("$skip", v);
    return *this;
}

pipeline& pipeline::skip(std::int64_t v) {
    impl::with(this)->append("$skip", v);
    return *this;
}

pipeline& pipeline::sort(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$sort", v);
    return *this;
}

pipeline& pipeline::sort_by_count(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$sortByCount", v);
    return *this;
}

pipeline& pipeline::sort_by_count(bsoncxx::v1::stdx::string_view v) {
    impl::with(this)->append("$sortByCount", v);
    return *this;
}

pipeline& pipeline::unwind(bsoncxx::v1::document::view v) {
    impl::with(this)->append("$unwind", v);
    return *this;
}

pipeline& pipeline::unwind(bsoncxx::v1::stdx::string_view v) {
    impl::with(this)->append("$unwind", v);
    return *this;
}

scoped_bson const& pipeline::internal::doc(pipeline const& self) {
    return impl::with(self).doc();
}

} // namespace v1
} // namespace mongocxx
