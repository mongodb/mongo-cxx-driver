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

#include <mongocxx/v1/write_concern.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <chrono>
#include <cstdint>
#include <cstring>
#include <string>
#include <tuple>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

namespace {

mongoc_write_concern_t* to_mongoc(void* ptr) {
    return static_cast<mongoc_write_concern_t*>(ptr);
}

} // namespace

write_concern::~write_concern() {
    libmongoc::write_concern_destroy(to_mongoc(_impl));
}

write_concern::write_concern(write_concern&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

write_concern& write_concern::operator=(write_concern&& other) noexcept {
    if (this != &other) {
        libmongoc::write_concern_destroy(to_mongoc(exchange(_impl, exchange(other._impl, nullptr))));
    }
    return *this;
}

write_concern::write_concern(write_concern const& other)
    : _impl{libmongoc::write_concern_copy(to_mongoc(other._impl))} {}

write_concern& write_concern::operator=(write_concern const& other) {
    if (this != &other) {
        libmongoc::write_concern_destroy(
            to_mongoc(exchange(_impl, libmongoc::write_concern_copy(to_mongoc(other._impl)))));
    }
    return *this;
}

write_concern::write_concern() : _impl{libmongoc::write_concern_new()} {}

write_concern& write_concern::acknowledge_level(level v) {
    switch (v) {
        case level::k_default:
            libmongoc::write_concern_set_w(to_mongoc(_impl), MONGOC_WRITE_CONCERN_W_DEFAULT);
            break;
        case level::k_majority:
            libmongoc::write_concern_set_w(to_mongoc(_impl), MONGOC_WRITE_CONCERN_W_MAJORITY);
            break;
        case level::k_unacknowledged:
            libmongoc::write_concern_set_w(to_mongoc(_impl), MONGOC_WRITE_CONCERN_W_UNACKNOWLEDGED);
            break;
        case level::k_acknowledged:
            libmongoc::write_concern_set_w(to_mongoc(_impl), 1); // MONGOC_WRITE_CONCERN_W_ACKNOWLEDGED
            break;

        default:
        case level::k_tag:
        case level::k_unknown:
            // Precondition violation: undocumented but well-defined behavior.
            libmongoc::write_concern_set_w(to_mongoc(_impl), MONGOC_WRITE_CONCERN_W_DEFAULT);
            break;
    }

    return *this;
}

write_concern::level write_concern::acknowledge_level() const {
    auto const w = libmongoc::write_concern_get_w(to_mongoc(_impl));

    switch (w) {
        case MONGOC_WRITE_CONCERN_W_DEFAULT:
            return level::k_default;
        case MONGOC_WRITE_CONCERN_W_MAJORITY:
            return level::k_majority;
        case MONGOC_WRITE_CONCERN_W_UNACKNOWLEDGED:
            return level::k_unacknowledged;
        case MONGOC_WRITE_CONCERN_W_TAG:
            return level::k_tag;
        default:
            return w >= 1 ? level::k_acknowledged : level::k_unknown;
    }
}

write_concern& write_concern::timeout(std::chrono::milliseconds v) {
    libmongoc::write_concern_set_wtimeout_int64(to_mongoc(_impl), v.count());
    return *this;
}

std::chrono::milliseconds write_concern::timeout() const {
    return std::chrono::milliseconds{libmongoc::write_concern_get_wtimeout_int64(to_mongoc(_impl))};
}

write_concern& write_concern::nodes(std::int32_t v) {
    // Precondition violation: undocumented but well-defined behavior.
    if (v < 0) {
        libmongoc::write_concern_set_w(to_mongoc(_impl), MONGOC_WRITE_CONCERN_W_DEFAULT);
    } else {
        libmongoc::write_concern_set_w(to_mongoc(_impl), v);
    }
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> write_concern::nodes() const {
    bsoncxx::v1::stdx::optional<std::int32_t> ret;

    auto const w = libmongoc::write_concern_get_w(to_mongoc(_impl));
    if (w >= 0) {
        ret.emplace(w);
    }

    return ret;
}

write_concern& write_concern::tag(bsoncxx::v1::stdx::string_view v) {
    libmongoc::write_concern_set_wtag(to_mongoc(_impl), std::string{v}.c_str());
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> write_concern::tag() const {
    bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> ret;

    if (auto const str = libmongoc::write_concern_get_wtag(to_mongoc(_impl))) {
        ret.emplace(str);
    }

    return ret;
}

write_concern& write_concern::journal(bool j) {
    libmongoc::write_concern_set_journal(to_mongoc(_impl), j);
    return *this;
}

bsoncxx::v1::stdx::optional<bool> write_concern::journal() const {
    bsoncxx::v1::stdx::optional<bool> ret;

    if (libmongoc::write_concern_journal_is_set(to_mongoc(_impl))) {
        ret.emplace(libmongoc::write_concern_get_journal(to_mongoc(_impl)));
    }

    return ret;
}

bool write_concern::is_acknowledged() const {
    return libmongoc::write_concern_is_acknowledged(to_mongoc(_impl));
}

bsoncxx::v1::document::value write_concern::to_document() const {
    scoped_bson doc;

    switch (this->acknowledge_level()) {
        case level::k_default:
            break;

        case level::k_majority:
            doc += scoped_bson{BCON_NEW("w", BCON_UTF8("majority"))};
            break;

        case level::k_tag:
            doc += scoped_bson{BCON_NEW("w", BCON_UTF8(libmongoc::write_concern_get_wtag(to_mongoc(_impl))))};
            break;

        case level::k_unacknowledged:
            doc += scoped_bson{BCON_NEW("w", BCON_INT32(0))};
            break;

        case level::k_acknowledged:
            doc += scoped_bson{BCON_NEW("w", BCON_INT32(*this->nodes()))};
            break;

        case level::k_unknown:
            // Precondition violation: undocumented but well-defined behavior.
            break;
    }

    if (auto const opt = this->journal()) {
        doc += scoped_bson{BCON_NEW("j", BCON_BOOL(*opt))};
    }

    {
        auto const t = this->timeout().count();

        if (t > 0) {
            // Backward compatibility: only use "$numberLong" when necessary.
            if (t <= std::int64_t{INT32_MAX}) {
                doc += scoped_bson{BCON_NEW("wtimeout", BCON_INT32(static_cast<std::int32_t>(t)))};
            } else {
                doc += scoped_bson{BCON_NEW("wtimeout", BCON_INT64(t))};
            }
        }
    }

    return std::move(doc).value();
}

bool operator==(write_concern const& lhs, write_concern const& rhs) {
    // Lexicographic comparison of data members.
    return std::make_tuple(lhs.acknowledge_level(), lhs.timeout(), lhs.nodes(), lhs.tag(), lhs.journal()) ==
           std::make_tuple(rhs.acknowledge_level(), rhs.timeout(), rhs.nodes(), rhs.tag(), rhs.journal());
}

write_concern::write_concern(void* impl) : _impl{impl} {}

write_concern write_concern::internal::make(mongoc_write_concern_t* rc) {
    return {rc};
}

mongoc_write_concern_t const* write_concern::internal::as_mongoc(write_concern const& self) {
    return to_mongoc(self._impl);
}

} // namespace v1
} // namespace mongocxx
