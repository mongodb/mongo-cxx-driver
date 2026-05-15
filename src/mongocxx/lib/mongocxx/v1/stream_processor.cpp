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

#include <mongocxx/v1/stream_processor.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/element/view.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/id.hpp>

#include <mongocxx/v1/database.hpp>

#include <mongocxx/v1/get_stream_processor_samples_options.hh>
#include <mongocxx/v1/get_stream_processor_samples_result.hh>
#include <mongocxx/v1/get_stream_processor_stats_options.hh>
#include <mongocxx/v1/start_stream_processor_options.hh>

#include <cstdint>
#include <string>
#include <vector>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class stream_processor::impl {
   public:
    std::string _name;
    v1::database _admin_db;

    static impl const& with(stream_processor const& other) {
        return *static_cast<impl const*>(other._impl);
    }
    static impl const* with(stream_processor const* other) {
        return static_cast<impl const*>(other->_impl);
    }
    static impl& with(stream_processor& other) {
        return *static_cast<impl*>(other._impl);
    }
    static impl* with(stream_processor* other) {
        return static_cast<impl*>(other->_impl);
    }
    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory)

stream_processor::~stream_processor() {
    delete impl::with(_impl);
}

stream_processor::stream_processor(stream_processor&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

stream_processor& stream_processor::operator=(stream_processor&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }
    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

bsoncxx::v1::stdx::string_view stream_processor::name() const {
    return impl::with(this)->_name;
}

void stream_processor::start(v1::start_stream_processor_options const& options) {
    scoped_bson cmd;
    cmd += scoped_bson{BCON_NEW("startStreamProcessor", BCON_UTF8(impl::with(this)->_name.c_str()))};
    v1::start_stream_processor_options::internal::append_to(options, cmd);
    impl::with(this)->_admin_db.run_command(cmd.view());
}

void stream_processor::stop() {
    scoped_bson cmd;
    cmd += scoped_bson{BCON_NEW("stopStreamProcessor", BCON_UTF8(impl::with(this)->_name.c_str()))};
    impl::with(this)->_admin_db.run_command(cmd.view());
}

void stream_processor::drop() {
    scoped_bson cmd;
    cmd += scoped_bson{BCON_NEW("dropStreamProcessor", BCON_UTF8(impl::with(this)->_name.c_str()))};
    impl::with(this)->_admin_db.run_command(cmd.view());
}

bsoncxx::v1::document::value stream_processor::stats(v1::get_stream_processor_stats_options const& options) {
    scoped_bson cmd;
    cmd += scoped_bson{BCON_NEW("getStreamProcessorStats", BCON_UTF8(impl::with(this)->_name.c_str()))};
    v1::get_stream_processor_stats_options::internal::append_options_to(options, cmd);
    return impl::with(this)->_admin_db.run_command(cmd.view());
}

v1::get_stream_processor_samples_result stream_processor::get_stream_processor_samples(
    v1::get_stream_processor_samples_options const& options) {
    auto const& name = impl::with(this)->_name;
    auto& db = impl::with(this)->_admin_db;

    auto cursor_id_opt = options.cursor_id();
    bool is_initial = !cursor_id_opt || *cursor_id_opt == 0;

    std::int64_t active_cursor_id{};

    if (is_initial) {
        scoped_bson start_cmd;
        start_cmd += scoped_bson{BCON_NEW("startSampleStreamProcessor", BCON_UTF8(name.c_str()))};
        if (auto lim = options.limit()) {
            start_cmd += scoped_bson{BCON_NEW("limit", BCON_INT32(*lim))};
        }
        auto start_reply = db.run_command(start_cmd.view());
        if (auto cursor_el = start_reply.view()["cursorId"];
            cursor_el && cursor_el.type_id() == bsoncxx::v1::types::id::k_int64) {
            active_cursor_id = cursor_el.get_int64().value;
        }
    } else {
        active_cursor_id = *cursor_id_opt;
    }

    scoped_bson get_more_cmd;
    get_more_cmd += scoped_bson{BCON_NEW("getMoreSampleStreamProcessor", BCON_UTF8(name.c_str()))};
    get_more_cmd += scoped_bson{BCON_NEW("cursorId", BCON_INT64(active_cursor_id))};
    if (!is_initial) {
        if (auto bs = options.batch_size()) {
            get_more_cmd += scoped_bson{BCON_NEW("batchSize", BCON_INT32(*bs))};
        }
    }

    auto get_more_reply = db.run_command(get_more_cmd.view());
    auto reply_view = get_more_reply.view();

    std::int64_t next_cursor_id{0};
    if (auto el = reply_view["cursorId"]; el && el.type_id() == bsoncxx::v1::types::id::k_int64) {
        next_cursor_id = el.get_int64().value;
    }

    std::vector<bsoncxx::v1::document::value> documents;
    if (auto el = reply_view["messages"]; el && el.type_id() == bsoncxx::v1::types::id::k_array) {
        for (auto const& msg : el.get_array().value) {
            if (msg.type_id() == bsoncxx::v1::types::id::k_document) {
                documents.emplace_back(bsoncxx::v1::document::value{msg.get_document().value});
            }
        }
    }

    return v1::get_stream_processor_samples_result{next_cursor_id, std::move(documents)};
}

stream_processor stream_processor::internal::make(std::string name, v1::database admin_db) {
    stream_processor result;
    // NOLINTBEGIN(cppcoreguidelines-owning-memory)
    result._impl = new impl{std::move(name), std::move(admin_db)};
    // NOLINTEND(cppcoreguidelines-owning-memory)
    return result;
}

} // namespace v1
} // namespace mongocxx
