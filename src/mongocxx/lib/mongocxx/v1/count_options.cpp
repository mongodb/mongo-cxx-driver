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

#include <mongocxx/v1/count_options.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <mongocxx/v1/read_preference.hpp>

#include <bsoncxx/v1/types/value.hh>

#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/hint.hh>

#include <chrono>
#include <cstdint>
#include <stdexcept>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class count_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<mongocxx::v1::hint> _hint;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _comment;
    bsoncxx::v1::stdx::optional<std::int64_t> _limit;
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> _max_time;
    bsoncxx::v1::stdx::optional<std::int64_t> _skip;
    bsoncxx::v1::stdx::optional<mongocxx::v1::read_preference> _rp;

    static impl const& with(count_options const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(count_options const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(count_options& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(count_options* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

count_options::~count_options() {
    delete impl::with(_impl);
}

count_options::count_options(count_options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

count_options& count_options::operator=(count_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

count_options::count_options(count_options const& other) : _impl{new impl{impl::with(other)}} {}

count_options& count_options::operator=(count_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

count_options::count_options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

count_options& count_options::collation(bsoncxx::v1::document::value collation) {
    impl::with(this)->_collation = std::move(collation);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> count_options::collation() const {
    return impl::with(this)->_collation;
}

count_options& count_options::hint(v1::hint index_hint) {
    impl::with(this)->_hint = std::move(index_hint);
    return *this;
}

bsoncxx::v1::stdx::optional<v1::hint> count_options::hint() const {
    return impl::with(this)->_hint;
}

count_options& count_options::comment(bsoncxx::v1::types::value comment) {
    impl::with(this)->_comment = std::move(comment);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> count_options::comment() const {
    return impl::with(this)->_comment;
}

count_options& count_options::limit(std::int64_t limit) {
    impl::with(this)->_limit = limit;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int64_t> count_options::limit() const {
    return impl::with(this)->_limit;
}

count_options& count_options::max_time(std::chrono::milliseconds max_time) {
    impl::with(this)->_max_time = max_time;
    return *this;
}

bsoncxx::v1::stdx::optional<std::chrono::milliseconds> count_options::max_time() const {
    return impl::with(this)->_max_time;
}

count_options& count_options::skip(std::int64_t skip) {
    impl::with(this)->_skip = skip;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int64_t> count_options::skip() const {
    return impl::with(this)->_skip;
}

count_options& count_options::read_preference(v1::read_preference rp) {
    impl::with(this)->_rp = std::move(rp);
    return *this;
}

bsoncxx::v1::stdx::optional<mongocxx::v1::read_preference> const& count_options::internal::read_preference(
    count_options const& self) {
    return impl::with(self)._rp;
}

bsoncxx::v1::stdx::optional<v1::read_preference> count_options::read_preference() const {
    return impl::with(this)->_rp;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& count_options::internal::collation(count_options& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<mongocxx::v1::hint>& count_options::internal::hint(count_options& self) {
    return impl::with(self)._hint;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& count_options::internal::comment(count_options& self) {
    return impl::with(self)._comment;
}

bsoncxx::v1::stdx::optional<mongocxx::v1::read_preference>& count_options::internal::read_preference(
    count_options& self) {
    return impl::with(self)._rp;
}

void count_options::internal::append_to(count_options const& self, scoped_bson& doc) {
    if (auto const& collation = impl::with(self)._collation) {
        doc += scoped_bson{BCON_NEW("collation", BCON_DOCUMENT(scoped_bson_view{*collation}.bson()))};
    }

    if (auto const& max_time = impl::with(self)._max_time) {
        doc += scoped_bson{BCON_NEW("maxTimeMS", BCON_INT64(max_time->count()))};
    }

    if (auto const& hint = impl::with(self)._hint) {
        if (auto const& doc_opt = v1::hint::internal::doc(*hint)) {
            doc += scoped_bson{BCON_NEW("hint", BCON_DOCUMENT(scoped_bson_view{*doc_opt}.bson()))};
        }

        if (auto const& str_opt = v1::hint::internal::str(*hint)) {
            doc += scoped_bson{BCON_NEW("hint", BCON_UTF8(str_opt->c_str()))};
        }
    }

    if (auto const& comment = impl::with(self)._comment) {
        scoped_bson v;

        if (!BSON_APPEND_VALUE(
                v.inout_ptr(), "comment", &bsoncxx::v1::types::value::internal::get_bson_value(*comment))) {
            throw std::logic_error{"mongocxx::v1::count_options::internal::append_to: BSON_APPEND_VALUE failed"};
        }

        doc += v;
    }

    if (auto const& skip = impl::with(self)._skip) {
        doc += scoped_bson{BCON_NEW("skip", BCON_INT64(*skip))};
    }

    if (auto const& limit = impl::with(self)._limit) {
        doc += scoped_bson{BCON_NEW("limit", BCON_INT64(*limit))};
    }
}

} // namespace v1
} // namespace mongocxx
