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

#include <mongocxx/v1/get_stream_processor_stats_options.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class get_stream_processor_stats_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bool> _verbose;

    static impl const& with(get_stream_processor_stats_options const& other) {
        return *static_cast<impl const*>(other._impl);
    }
    static impl const* with(get_stream_processor_stats_options const* other) {
        return static_cast<impl const*>(other->_impl);
    }
    static impl& with(get_stream_processor_stats_options& other) {
        return *static_cast<impl*>(other._impl);
    }
    static impl* with(get_stream_processor_stats_options* other) {
        return static_cast<impl*>(other->_impl);
    }
    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory)

get_stream_processor_stats_options::~get_stream_processor_stats_options() {
    delete impl::with(_impl);
}

get_stream_processor_stats_options::get_stream_processor_stats_options(
    get_stream_processor_stats_options&& other) noexcept
    : _impl{exchange(other._impl, nullptr)} {}

get_stream_processor_stats_options& get_stream_processor_stats_options::operator=(
    get_stream_processor_stats_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }
    return *this;
}

get_stream_processor_stats_options::get_stream_processor_stats_options(get_stream_processor_stats_options const& other)
    : _impl{new impl{impl::with(other)}} {}

get_stream_processor_stats_options& get_stream_processor_stats_options::operator=(
    get_stream_processor_stats_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }
    return *this;
}

get_stream_processor_stats_options::get_stream_processor_stats_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

get_stream_processor_stats_options& get_stream_processor_stats_options::verbose(bool v) {
    impl::with(this)->_verbose = v;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> get_stream_processor_stats_options::verbose() const {
    return impl::with(this)->_verbose;
}

void get_stream_processor_stats_options::internal::append_options_to(
    get_stream_processor_stats_options const& self,
    scoped_bson& doc) {
    auto const& i = impl::with(self);

    scoped_bson opts_doc;
    if (i._verbose) {
        opts_doc += scoped_bson{BCON_NEW("verbose", BCON_BOOL(*i._verbose))};
    }
    doc += scoped_bson{BCON_NEW("options", BCON_DOCUMENT(opts_doc.bson()))};
}

} // namespace v1
} // namespace mongocxx
