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

#include <mongocxx/v1/create_stream_processor_options.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <string>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class create_stream_processor_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _dlq;
    bsoncxx::v1::stdx::optional<std::string> _stream_meta_field_name;
    bsoncxx::v1::stdx::optional<std::string> _tier;
    bsoncxx::v1::stdx::optional<bool> _failover;

    static impl const& with(create_stream_processor_options const& other) {
        return *static_cast<impl const*>(other._impl);
    }
    static impl const* with(create_stream_processor_options const* other) {
        return static_cast<impl const*>(other->_impl);
    }
    static impl& with(create_stream_processor_options& other) {
        return *static_cast<impl*>(other._impl);
    }
    static impl* with(create_stream_processor_options* other) {
        return static_cast<impl*>(other->_impl);
    }
    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory)

create_stream_processor_options::~create_stream_processor_options() {
    delete impl::with(_impl);
}

create_stream_processor_options::create_stream_processor_options(create_stream_processor_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

create_stream_processor_options& create_stream_processor_options::operator=(
    create_stream_processor_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }
    return *this;
}

create_stream_processor_options::create_stream_processor_options(create_stream_processor_options const& other)
    : _impl{new impl{impl::with(other)}} {}

create_stream_processor_options& create_stream_processor_options::operator=(
    create_stream_processor_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }
    return *this;
}

create_stream_processor_options::create_stream_processor_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

create_stream_processor_options& create_stream_processor_options::dlq(bsoncxx::v1::document::value v) {
    impl::with(this)->_dlq = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> create_stream_processor_options::dlq() const {
    return impl::with(this)->_dlq;
}

create_stream_processor_options& create_stream_processor_options::stream_meta_field_name(std::string v) {
    impl::with(this)->_stream_meta_field_name = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> create_stream_processor_options::stream_meta_field_name()
    const {
    if (auto const& v = impl::with(this)->_stream_meta_field_name) {
        return bsoncxx::v1::stdx::string_view{*v};
    }
    return bsoncxx::v1::stdx::nullopt;
}

create_stream_processor_options& create_stream_processor_options::tier(std::string v) {
    impl::with(this)->_tier = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> create_stream_processor_options::tier() const {
    if (auto const& v = impl::with(this)->_tier) {
        return bsoncxx::v1::stdx::string_view{*v};
    }
    return bsoncxx::v1::stdx::nullopt;
}

create_stream_processor_options& create_stream_processor_options::failover(bool v) {
    impl::with(this)->_failover = v;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> create_stream_processor_options::failover() const {
    return impl::with(this)->_failover;
}

void create_stream_processor_options::internal::append_options_to(
    create_stream_processor_options const& self,
    scoped_bson& doc) {
    auto const& i = impl::with(self);

    scoped_bson opts_doc;
    if (i._dlq) {
        scoped_bson_view dlq_view{i._dlq->view()};
        opts_doc += scoped_bson{BCON_NEW("dlq", BCON_DOCUMENT(dlq_view.bson()))};
    }
    if (i._stream_meta_field_name) {
        opts_doc += scoped_bson{BCON_NEW("streamMetaFieldName", BCON_UTF8(i._stream_meta_field_name->c_str()))};
    }
    if (i._tier) {
        opts_doc += scoped_bson{BCON_NEW("tier", BCON_UTF8(i._tier->c_str()))};
    }
    if (i._failover) {
        opts_doc += scoped_bson{BCON_NEW("failover", BCON_BOOL(*i._failover))};
    }
    doc += scoped_bson{BCON_NEW("options", BCON_DOCUMENT(opts_doc.bson()))};
}

} // namespace v1
} // namespace mongocxx
