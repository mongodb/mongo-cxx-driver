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

#include <mongocxx/v1/structured_logging.hh>

//

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/structured_log.hh>

#include <array>
#include <cstddef>
#include <iostream>
#include <utility>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class structured_logging::impl {
   public:
    v1::structured_log_handler _handler;
    bsoncxx::v1::stdx::optional<v1::structured_log_level> _all_components_level;
    // Indexed by `static_cast<std::size_t>(structured_log_component)`.
    std::array<bsoncxx::v1::stdx::optional<v1::structured_log_level>, 4> _component_levels;
    bsoncxx::v1::stdx::optional<std::size_t> _max_document_length;
    bool _max_levels_from_env = false;
    bool _max_document_length_from_env = false;

    static impl const& with(structured_logging const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl& with(structured_logging& self) {
        return *static_cast<impl*>(self._impl);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

structured_logging::~structured_logging() {
    delete static_cast<impl*>(_impl);
}

structured_logging::structured_logging(structured_logging&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

structured_logging& structured_logging::operator=(structured_logging&& other) noexcept {
    if (this != &other) {
        delete static_cast<impl*>(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

structured_logging::structured_logging(structured_logging const& other) : _impl{new impl{impl::with(other)}} {}

structured_logging& structured_logging::operator=(structured_logging const& other) {
    if (this != &other) {
        delete static_cast<impl*>(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

structured_logging::structured_logging() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

structured_logging& structured_logging::handler(v1::structured_log_handler handler) {
    impl::with(*this)._handler = std::move(handler);
    return *this;
}

v1::structured_log_handler structured_logging::handler() const {
    return impl::with(*this)._handler;
}

structured_logging& structured_logging::max_level_for_component(
    v1::structured_log_component component,
    v1::structured_log_level level) {
    impl::with(*this)._component_levels[static_cast<std::size_t>(component)] = level;
    return *this;
}

bsoncxx::v1::stdx::optional<v1::structured_log_level> structured_logging::max_level_for_component(
    v1::structured_log_component component) const {
    return impl::with(*this)._component_levels[static_cast<std::size_t>(component)];
}

structured_logging& structured_logging::max_level_for_all_components(v1::structured_log_level level) {
    impl::with(*this)._all_components_level = level;
    return *this;
}

bsoncxx::v1::stdx::optional<v1::structured_log_level> structured_logging::max_level_for_all_components() const {
    return impl::with(*this)._all_components_level;
}

structured_logging& structured_logging::max_levels_from_env() {
    impl::with(*this)._max_levels_from_env = true;
    return *this;
}

structured_logging& structured_logging::max_document_length(std::size_t max_document_length) {
    impl::with(*this)._max_document_length = max_document_length;
    return *this;
}

bsoncxx::v1::stdx::optional<std::size_t> structured_logging::max_document_length() const {
    return impl::with(*this)._max_document_length;
}

structured_logging& structured_logging::max_document_length_from_env() {
    impl::with(*this)._max_document_length_from_env = true;
    return *this;
}

namespace {

// A handler exiting via an exception is undefined behavior in libmongoc. For QoI, terminate the
// program before allowing the exception to bypass libmongoc code.
template <typename Fn>
void exception_guard(char const* source, Fn fn) noexcept {
    try {
        fn();
    } catch (...) {
        std::cerr << "fatal error: structured log handler " << source << " exited via an exception" << std::endl;
        std::terminate();
    }
}

// The C callback registered with mongoc; recovers the structured_logging from user_data.
void handle_structured_log(mongoc_structured_log_entry_t const* entry, void* user_data) noexcept {
    auto const& self = *static_cast<structured_logging const*>(user_data);
    auto const view = structured_log_entry::internal::make(entry);
    exception_guard(__func__, [&] { structured_logging::internal::handler(self)(view); });
}

} // namespace

v1::structured_log_handler const& structured_logging::internal::handler(structured_logging const& self) {
    return impl::with(self)._handler;
}

mongoc_structured_log_opts_t* structured_logging::internal::make_opts(structured_logging& self) {
    auto const opts = libmongoc::structured_log_opts_new();
    auto& i = impl::with(self);

    if (i._all_components_level) {
        libmongoc::structured_log_opts_set_max_level_for_all_components(
            opts, static_cast<mongoc_structured_log_level_t>(*i._all_components_level));
    }

    for (std::size_t idx = 0; idx < i._component_levels.size(); ++idx) {
        if (auto const& level = i._component_levels[idx]) {
            libmongoc::structured_log_opts_set_max_level_for_component(
                opts,
                static_cast<mongoc_structured_log_component_t>(idx),
                static_cast<mongoc_structured_log_level_t>(*level));
        }
    }

    // Applied after the programmatic settings so the environment takes precedence.
    if (i._max_levels_from_env) {
        libmongoc::structured_log_opts_set_max_levels_from_env(opts);
    }

    if (i._max_document_length) {
        libmongoc::structured_log_opts_set_max_document_length(opts, *i._max_document_length);
    }

    if (i._max_document_length_from_env) {
        libmongoc::structured_log_opts_set_max_document_length_from_env(opts);
    }

    // An empty handler disables structured logging (NULL func).
    if (i._handler) {
        libmongoc::structured_log_opts_set_handler(opts, handle_structured_log, &self);
    } else {
        libmongoc::structured_log_opts_set_handler(opts, nullptr, nullptr);
    }

    return opts;
}

void structured_logging::internal::apply_to(mongoc_client_t* client, structured_logging& self) {
    auto const opts = make_opts(self);
    libmongoc::client_set_structured_log_opts(client, opts);
    libmongoc::structured_log_opts_destroy(opts);
}

void structured_logging::internal::apply_to(mongoc_client_pool_t* pool, structured_logging& self) {
    auto const opts = make_opts(self);
    libmongoc::client_pool_set_structured_log_opts(pool, opts);
    libmongoc::structured_log_opts_destroy(opts);
}

} // namespace v1
} // namespace mongocxx
