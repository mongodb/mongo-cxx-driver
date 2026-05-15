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

#include <mongocxx/v1/failover_options.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <string>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class failover_options::impl {
   public:
    std::string _region;
    bsoncxx::v1::stdx::optional<std::string> _mode;
    bsoncxx::v1::stdx::optional<bool> _dry_run;

    static impl const& with(failover_options const& other) {
        return *static_cast<impl const*>(other._impl);
    }
    static impl const* with(failover_options const* other) {
        return static_cast<impl const*>(other->_impl);
    }
    static impl& with(failover_options& other) {
        return *static_cast<impl*>(other._impl);
    }
    static impl* with(failover_options* other) {
        return static_cast<impl*>(other->_impl);
    }
    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory)

failover_options::~failover_options() {
    delete impl::with(_impl);
}

failover_options::failover_options(failover_options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

failover_options& failover_options::operator=(failover_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }
    return *this;
}

failover_options::failover_options(failover_options const& other) : _impl{new impl{impl::with(other)}} {}

failover_options& failover_options::operator=(failover_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }
    return *this;
}

failover_options::failover_options(std::string region) : _impl{new impl{}} {
    impl::with(this)->_region = std::move(region);
}

// NOLINTEND(cppcoreguidelines-owning-memory)

failover_options& failover_options::region(std::string v) {
    impl::with(this)->_region = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::string_view failover_options::region() const {
    return impl::with(this)->_region;
}

failover_options& failover_options::mode(std::string v) {
    impl::with(this)->_mode = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> failover_options::mode() const {
    if (auto const& v = impl::with(this)->_mode) {
        return bsoncxx::v1::stdx::string_view{*v};
    }
    return bsoncxx::v1::stdx::nullopt;
}

failover_options& failover_options::dry_run(bool v) {
    impl::with(this)->_dry_run = v;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> failover_options::dry_run() const {
    return impl::with(this)->_dry_run;
}

void failover_options::internal::append_to(failover_options const& self, scoped_bson& doc) {
    auto const& i = impl::with(self);

    doc += scoped_bson{BCON_NEW("region", BCON_UTF8(i._region.c_str()))};
    if (i._mode) {
        doc += scoped_bson{BCON_NEW("mode", BCON_UTF8(i._mode->c_str()))};
    }
    if (i._dry_run) {
        doc += scoped_bson{BCON_NEW("dryRun", BCON_BOOL(*i._dry_run))};
    }
}

} // namespace v1
} // namespace mongocxx
