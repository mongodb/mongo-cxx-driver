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

#include <mongocxx/v1/stream_processors.hh>

//

#include <bsoncxx/v1/array/view.hpp>
#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/document/view.hpp>

#include <mongocxx/v1/database.hpp>

#include <mongocxx/v1/create_stream_processor_options.hh>
#include <mongocxx/v1/stream_processor.hh>
#include <mongocxx/v1/stream_processor_info.hh>

#include <string>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class stream_processors::impl {
   public:
    v1::database _admin_db;

    static impl const& with(stream_processors const& other) {
        return *static_cast<impl const*>(other._impl);
    }
    static impl const* with(stream_processors const* other) {
        return static_cast<impl const*>(other->_impl);
    }
    static impl& with(stream_processors& other) {
        return *static_cast<impl*>(other._impl);
    }
    static impl* with(stream_processors* other) {
        return static_cast<impl*>(other->_impl);
    }
    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory)

stream_processors::~stream_processors() {
    delete impl::with(_impl);
}

stream_processors::stream_processors(stream_processors&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

stream_processors& stream_processors::operator=(stream_processors&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }
    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

void stream_processors::create(
    std::string name,
    bsoncxx::v1::array::view pipeline,
    v1::create_stream_processor_options const& options) {
    scoped_bson cmd;
    cmd += scoped_bson{BCON_NEW("createStreamProcessor", BCON_UTF8(name.c_str()))};
    scoped_bson_view pipeline_as_doc{static_cast<bsoncxx::v1::document::view>(pipeline)};
    cmd += scoped_bson{BCON_NEW("pipeline", BCON_ARRAY(pipeline_as_doc.bson()))};
    v1::create_stream_processor_options::internal::append_options_to(options, cmd);
    impl::with(this)->_admin_db.run_command(cmd.view());
}

v1::stream_processor stream_processors::get(std::string name) {
    return v1::stream_processor::internal::make(std::move(name), impl::with(this)->_admin_db);
}

v1::stream_processor_info stream_processors::get_info(std::string const& name) {
    scoped_bson cmd;
    cmd += scoped_bson{BCON_NEW("getStreamProcessor", BCON_UTF8(name.c_str()))};
    auto reply = impl::with(this)->_admin_db.run_command(cmd.view());
    return v1::stream_processor_info{reply.view()};
}

stream_processors stream_processors::internal::make(v1::database admin_db) {
    stream_processors result;
    // NOLINTBEGIN(cppcoreguidelines-owning-memory)
    result._impl = new impl{std::move(admin_db)};
    // NOLINTEND(cppcoreguidelines-owning-memory)
    return result;
}

} // namespace v1
} // namespace mongocxx
