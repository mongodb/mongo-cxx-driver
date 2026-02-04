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

#include <mongocxx/v1/text_options.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>

#include <cstdint>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

namespace {

class common_fields {
   public:
    bsoncxx::v1::stdx::optional<std::int32_t> _min_query_length;
    bsoncxx::v1::stdx::optional<std::int32_t> _max_query_length;
    bsoncxx::v1::stdx::optional<std::int32_t> _max_length;

    static common_fields* with(void* ptr) {
        return static_cast<common_fields*>(ptr);
    }
};

} // namespace

class text_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bool> _case_sensitive;
    bsoncxx::v1::stdx::optional<bool> _diacritic_sensitive;
    bsoncxx::v1::stdx::optional<prefix> _prefix_opts;
    bsoncxx::v1::stdx::optional<suffix> _suffix_opts;
    bsoncxx::v1::stdx::optional<substring> _substring_opts;

    static impl const& with(text_options const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(text_options const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(text_options& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(text_options* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

text_options::~text_options() {
    delete impl::with(_impl);
}

text_options::text_options(text_options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

text_options& text_options::operator=(text_options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

text_options::text_options(text_options const& other) : _impl{new impl{impl::with(other)}} {}

text_options& text_options::operator=(text_options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

text_options::text_options() : _impl{new impl{}} {}

text_options::prefix::~prefix() {
    delete common_fields::with(_impl);
    _impl = nullptr; // warning: Attempt to free released memory [cplusplus.NewDelete]
}

text_options::prefix::prefix(prefix&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

text_options::prefix& text_options::prefix::operator=(prefix&& other) noexcept {
    if (this != &other) {
        delete common_fields::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

text_options::prefix::prefix(prefix const& other) : _impl{new common_fields{*common_fields::with(other._impl)}} {}

text_options::prefix& text_options::prefix::operator=(prefix const& other) {
    if (this != &other) {
        delete common_fields::with(exchange(_impl, new common_fields{*common_fields::with(other._impl)}));
    }

    return *this;
}

text_options::prefix::prefix() : _impl{new common_fields{}} {}

text_options::suffix::~suffix() {
    delete common_fields::with(_impl);
    _impl = nullptr; // warning: Attempt to free released memory [cplusplus.NewDelete]
}

text_options::suffix::suffix(suffix&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

text_options::suffix& text_options::suffix::operator=(suffix&& other) noexcept {
    if (this != &other) {
        delete common_fields::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

text_options::suffix::suffix(suffix const& other) : _impl{new common_fields{*common_fields::with(other._impl)}} {}

text_options::suffix& text_options::suffix::operator=(suffix const& other) {
    if (this != &other) {
        delete common_fields::with(exchange(_impl, new common_fields{*common_fields::with(other._impl)}));
    }

    return *this;
}

text_options::suffix::suffix() : _impl{new common_fields{}} {}

text_options::substring::~substring() {
    delete common_fields::with(_impl);
    _impl = nullptr; // warning: Attempt to free released memory [cplusplus.NewDelete]
}

text_options::substring::substring(substring&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

text_options::substring& text_options::substring::operator=(substring&& other) noexcept {
    if (this != &other) {
        delete common_fields::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

text_options::substring::substring(substring const& other)
    : _impl{new common_fields{*common_fields::with(other._impl)}} {}

text_options::substring& text_options::substring::operator=(substring const& other) {
    if (this != &other) {
        delete common_fields::with(exchange(_impl, new common_fields{*common_fields::with(other._impl)}));
    }

    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

text_options::substring::substring() : _impl{new common_fields{}} {}

text_options& text_options::case_sensitive(bool v) {
    impl::with(this)->_case_sensitive = v;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> text_options::case_sensitive() const {
    return impl::with(this)->_case_sensitive;
}

text_options& text_options::diacritic_sensitive(bool v) {
    impl::with(this)->_diacritic_sensitive = v;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> text_options::diacritic_sensitive() const {
    return impl::with(this)->_diacritic_sensitive;
}

text_options& text_options::prefix_opts(prefix v) {
    impl::with(this)->_prefix_opts = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<text_options::prefix> text_options::prefix_opts() const {
    return impl::with(this)->_prefix_opts;
}

text_options& text_options::suffix_opts(suffix v) {
    impl::with(this)->_suffix_opts = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<text_options::suffix> text_options::suffix_opts() const {
    return impl::with(this)->_suffix_opts;
}

text_options& text_options::substring_opts(substring v) {
    impl::with(this)->_substring_opts = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<text_options::substring> text_options::substring_opts() const {
    return impl::with(this)->_substring_opts;
}

bsoncxx::v1::stdx::optional<std::int32_t> text_options::prefix::str_max_query_length() const {
    return common_fields::with(_impl)->_max_query_length;
}

text_options::prefix& text_options::prefix::str_max_query_length(std::int32_t v) {
    common_fields::with(_impl)->_max_query_length = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> text_options::prefix::str_min_query_length() const {
    return common_fields::with(_impl)->_min_query_length;
}

text_options::prefix& text_options::prefix::str_min_query_length(std::int32_t v) {
    common_fields::with(_impl)->_min_query_length = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> text_options::suffix::str_max_query_length() const {
    return common_fields::with(_impl)->_max_query_length;
}

text_options::suffix& text_options::suffix::str_max_query_length(std::int32_t v) {
    common_fields::with(_impl)->_max_query_length = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> text_options::suffix::str_min_query_length() const {
    return common_fields::with(_impl)->_min_query_length;
}

text_options::suffix& text_options::suffix::str_min_query_length(std::int32_t v) {
    common_fields::with(_impl)->_min_query_length = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> text_options::substring::str_max_query_length() const {
    return common_fields::with(_impl)->_max_query_length;
}

text_options::substring& text_options::substring::str_max_query_length(std::int32_t v) {
    common_fields::with(_impl)->_max_query_length = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> text_options::substring::str_min_query_length() const {
    return common_fields::with(_impl)->_min_query_length;
}

text_options::substring& text_options::substring::str_min_query_length(std::int32_t v) {
    common_fields::with(_impl)->_min_query_length = v;
    return *this;
}

text_options::substring& text_options::substring::str_max_length(std::int32_t v) {
    common_fields::with(_impl)->_max_length = v;
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> text_options::substring::str_max_length() const {
    return common_fields::with(_impl)->_max_length;
}

bsoncxx::v1::stdx::optional<text_options::prefix> const& text_options::internal::prefix_opts(text_options const& self) {
    return impl::with(self)._prefix_opts;
}

bsoncxx::v1::stdx::optional<text_options::suffix> const& text_options::internal::suffix_opts(text_options const& self) {
    return impl::with(self)._suffix_opts;
}

bsoncxx::v1::stdx::optional<text_options::substring> const& text_options::internal::substring_opts(
    text_options const& self) {
    return impl::with(self)._substring_opts;
}

} // namespace v1
} // namespace mongocxx
