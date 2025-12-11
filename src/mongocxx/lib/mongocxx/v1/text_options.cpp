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

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/hint.hpp>
#include <mongocxx/v1/read_preference.hpp>
#include <mongocxx/v1/text_options.hpp> // IWYU pragma: export

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class common_opts_impl {
   public:
    bsoncxx::v1::stdx::optional<int32_t> _min_query_length;
    bsoncxx::v1::stdx::optional<int32_t> _max_query_length;
    bsoncxx::v1::stdx::optional<int32_t> _max_length;

    static common_opts_impl& with(void* ptr) {
        return *static_cast<common_opts_impl*>(ptr);
    }

    static common_opts_impl const& with(void const* ptr) {
        return *static_cast<common_opts_impl const*>(ptr);
    }
};

class text_options::prefix::impl final : public common_opts_impl {};
class text_options::suffix::impl final : public common_opts_impl {};
class text_options::substring::impl final : public common_opts_impl {};

class text_options::impl {
   public:
    bsoncxx::v1::stdx::optional<bool> _case_sensitive;
    bsoncxx::v1::stdx::optional<bool> _diacritic_sensitive;
    bsoncxx::v1::stdx::optional<prefix> _prefix;
    bsoncxx::v1::stdx::optional<suffix> _suffix;
    bsoncxx::v1::stdx::optional<substring> _substring;

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
    delete static_cast<text_options::prefix::impl*>(_impl);
}

text_options::prefix::prefix(prefix&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

text_options::prefix& text_options::prefix::operator=(prefix&& other) noexcept {
    if (this != &other) {
        delete static_cast<text_options::prefix::impl*>(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

text_options::prefix::prefix(prefix const& other) : _impl{new impl{*static_cast<impl const*>(other._impl)}} {}

text_options::prefix& text_options::prefix::operator=(prefix const& other) {
    if (this != &other) {
        delete static_cast<text_options::prefix::impl*>(
            exchange(_impl, new impl{*static_cast<impl const*>(other._impl)}));
    }

    return *this;
}

text_options::prefix::prefix() : _impl{new impl{}} {}

text_options::suffix::~suffix() {
    delete static_cast<text_options::suffix::impl*>(_impl);
}

text_options::suffix::suffix(suffix&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

text_options::suffix& text_options::suffix::operator=(suffix&& other) noexcept {
    if (this != &other) {
        delete static_cast<text_options::suffix::impl*>(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

text_options::suffix::suffix(suffix const& other) : _impl{new impl{*static_cast<impl const*>(other._impl)}} {}

text_options::suffix& text_options::suffix::operator=(suffix const& other) {
    if (this != &other) {
        delete static_cast<text_options::suffix::impl*>(
            exchange(_impl, new impl{*static_cast<impl const*>(other._impl)}));
    }

    return *this;
}

text_options::suffix::suffix() : _impl{new impl{}} {}

text_options::substring::~substring() {
    delete static_cast<text_options::substring::impl*>(_impl);
}

text_options::substring::substring(substring&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

text_options::substring& text_options::substring::operator=(substring&& other) noexcept {
    if (this != &other) {
        delete static_cast<text_options::substring::impl*>(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

text_options::substring::substring(substring const& other) : _impl{new impl{*static_cast<impl const*>(other._impl)}} {}

text_options::substring& text_options::substring::operator=(substring const& other) {
    if (this != &other) {
        delete static_cast<text_options::substring::impl*>(
            exchange(_impl, new impl{*static_cast<impl const*>(other._impl)}));
    }

    return *this;
}

text_options::substring::substring() : _impl{new impl{}} {}

text_options& text_options::case_sensitive(bool value) {
    impl::with(this)->_case_sensitive = value;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> text_options::case_sensitive() const {
    return impl::with(this)->_case_sensitive;
}

text_options& text_options::diacritic_sensitive(bool value) {
    impl::with(this)->_diacritic_sensitive = value;
    return *this;
}

bsoncxx::v1::stdx::optional<bool> text_options::diacritic_sensitive() const {
    return impl::with(this)->_diacritic_sensitive;
}

text_options& text_options::prefix_opts(prefix value) {
    impl::with(this)->_prefix = std::move(value);
    return *this;
}

bsoncxx::v1::stdx::optional<text_options::prefix> const& text_options::prefix_opts() const {
    return impl::with(this)->_prefix;
}

text_options& text_options::suffix_opts(suffix value) {
    impl::with(this)->_suffix = std::move(value);
    return *this;
}

bsoncxx::v1::stdx::optional<text_options::suffix> const& text_options::suffix_opts() const {
    return impl::with(this)->_suffix;
}

text_options& text_options::substring_opts(substring value) {
    impl::with(this)->_substring = std::move(value);
    return *this;
}

bsoncxx::v1::stdx::optional<text_options::substring> const& text_options::substring_opts() const {
    return impl::with(this)->_substring;
}

bsoncxx::v1::stdx::optional<int32_t> text_options::prefix::str_max_query_length() const {
    return common_opts_impl::with(_impl)._max_query_length;
}

text_options::prefix& text_options::prefix::str_max_query_length(int32_t value) {
    common_opts_impl::with(_impl)._max_query_length = value;
    return *this;
}

bsoncxx::v1::stdx::optional<int32_t> text_options::prefix::str_min_query_length() const {
    return common_opts_impl::with(_impl)._min_query_length;
}

text_options::prefix& text_options::prefix::str_min_query_length(int32_t value) {
    common_opts_impl::with(_impl)._min_query_length = value;
    return *this;
}

bsoncxx::v1::stdx::optional<int32_t> text_options::suffix::str_max_query_length() const {
    return common_opts_impl::with(_impl)._max_query_length;
}

text_options::suffix& text_options::suffix::str_max_query_length(int32_t value) {
    common_opts_impl::with(_impl)._max_query_length = value;
    return *this;
}

bsoncxx::v1::stdx::optional<int32_t> text_options::suffix::str_min_query_length() const {
    return common_opts_impl::with(_impl)._min_query_length;
}

text_options::suffix& text_options::suffix::str_min_query_length(int32_t value) {
    common_opts_impl::with(_impl)._min_query_length = value;
    return *this;
}

bsoncxx::v1::stdx::optional<int32_t> text_options::substring::str_max_query_length() const {
    return common_opts_impl::with(_impl)._max_query_length;
}

text_options::substring& text_options::substring::str_max_query_length(int32_t value) {
    common_opts_impl::with(_impl)._max_query_length = value;
    return *this;
}

bsoncxx::v1::stdx::optional<int32_t> text_options::substring::str_min_query_length() const {
    return common_opts_impl::with(_impl)._min_query_length;
}

text_options::substring& text_options::substring::str_min_query_length(int32_t value) {
    common_opts_impl::with(_impl)._min_query_length = value;
    return *this;
}

text_options::substring& text_options::substring::str_max_length(int32_t value) {
    common_opts_impl::with(_impl)._max_length = value;
    return *this;
}

bsoncxx::v1::stdx::optional<int32_t> text_options::substring::str_max_length() const {
    return common_opts_impl::with(_impl)._max_length;
}

} // namespace v1
} // namespace mongocxx
