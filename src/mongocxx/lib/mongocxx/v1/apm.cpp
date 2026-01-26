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

#include <mongocxx/v1/apm.hh>

//

#include <mongocxx/v1/config/export.hpp>

#include <mongocxx/v1/events/command_failed.hh>
#include <mongocxx/v1/events/command_started.hh>
#include <mongocxx/v1/events/command_succeeded.hh>
#include <mongocxx/v1/events/server_closed.hh>
#include <mongocxx/v1/events/server_description_changed.hh>
#include <mongocxx/v1/events/server_heartbeat_failed.hh>
#include <mongocxx/v1/events/server_heartbeat_started.hh>
#include <mongocxx/v1/events/server_heartbeat_succeeded.hh>
#include <mongocxx/v1/events/server_opening.hh>
#include <mongocxx/v1/events/topology_closed.hh>
#include <mongocxx/v1/events/topology_description_changed.hh>
#include <mongocxx/v1/events/topology_opening.hh>

#include <functional>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class apm::impl {
   public:
    template <typename T>
    using fn_type = std::function<void MONGOCXX_ABI_CDECL(T const&)>;

    fn_type<v1::events::command_started> _command_started;
    fn_type<v1::events::command_failed> _command_failed;
    fn_type<v1::events::command_succeeded> _command_succeeded;
    fn_type<v1::events::server_closed> _server_closed;
    fn_type<v1::events::server_description_changed> _server_description_changed;
    fn_type<v1::events::server_opening> _server_opening;
    fn_type<v1::events::topology_closed> _topology_closed;
    fn_type<v1::events::topology_description_changed> _topology_description_changed;
    fn_type<v1::events::topology_opening> _topology_opening;
    fn_type<v1::events::server_heartbeat_started> _server_heartbeat_started;
    fn_type<v1::events::server_heartbeat_failed> _server_heartbeat_failed;
    fn_type<v1::events::server_heartbeat_succeeded> _server_heartbeat_succeeded;

    static impl const& with(apm const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(apm const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(apm& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(apm* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

apm::~apm() {
    delete impl::with(this);
}

apm::apm(apm&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

apm& apm::operator=(apm&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

apm::apm(apm const& other) : _impl{new impl{impl::with(other)}} {}

apm& apm::operator=(apm const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

apm::apm() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

apm& apm::on_command_started(std::function<void MONGOCXX_ABI_CDECL(v1::events::command_started const&)> fn) {
    impl::with(this)->_command_started = std::move(fn);
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(v1::events::command_started const&)> apm::command_started() const {
    return impl::with(this)->_command_started;
}

apm& apm::on_command_failed(std::function<void MONGOCXX_ABI_CDECL(v1::events::command_failed const&)> fn) {
    impl::with(this)->_command_failed = std::move(fn);
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(v1::events::command_failed const&)> apm::command_failed() const {
    return impl::with(this)->_command_failed;
}

apm& apm::on_command_succeeded(std::function<void MONGOCXX_ABI_CDECL(v1::events::command_succeeded const&)> fn) {
    impl::with(this)->_command_succeeded = std::move(fn);
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(v1::events::command_succeeded const&)> apm::command_succeeded() const {
    return impl::with(this)->_command_succeeded;
}

apm& apm::on_server_opening(std::function<void MONGOCXX_ABI_CDECL(v1::events::server_opening const&)> fn) {
    impl::with(this)->_server_opening = std::move(fn);
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(v1::events::server_opening const&)> apm::server_opening() const {
    return impl::with(this)->_server_opening;
}

apm& apm::on_server_closed(std::function<void MONGOCXX_ABI_CDECL(v1::events::server_closed const&)> fn) {
    impl::with(this)->_server_closed = std::move(fn);
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(v1::events::server_closed const&)> apm::server_closed() const {
    return impl::with(this)->_server_closed;
}

apm& apm::on_server_description_changed(
    std::function<void MONGOCXX_ABI_CDECL(v1::events::server_description_changed const&)> fn) {
    impl::with(this)->_server_description_changed = std::move(fn);
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(v1::events::server_description_changed const&)> apm::server_description_changed()
    const {
    return impl::with(this)->_server_description_changed;
}

apm& apm::on_topology_opening(std::function<void MONGOCXX_ABI_CDECL(v1::events::topology_opening const&)> fn) {
    impl::with(this)->_topology_opening = std::move(fn);
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(v1::events::topology_opening const&)> apm::topology_opening() const {
    return impl::with(this)->_topology_opening;
}

apm& apm::on_topology_closed(std::function<void MONGOCXX_ABI_CDECL(v1::events::topology_closed const&)> fn) {
    impl::with(this)->_topology_closed = std::move(fn);
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(v1::events::topology_closed const&)> apm::topology_closed() const {
    return impl::with(this)->_topology_closed;
}

apm& apm::on_topology_description_changed(
    std::function<void MONGOCXX_ABI_CDECL(v1::events::topology_description_changed const&)> fn) {
    impl::with(this)->_topology_description_changed = std::move(fn);
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(v1::events::topology_description_changed const&)>
apm::topology_description_changed() const {
    return impl::with(this)->_topology_description_changed;
}

apm& apm::on_server_heartbeat_started(
    std::function<void MONGOCXX_ABI_CDECL(v1::events::server_heartbeat_started const&)> fn) {
    impl::with(this)->_server_heartbeat_started = std::move(fn);
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(v1::events::server_heartbeat_started const&)> apm::server_heartbeat_started()
    const {
    return impl::with(this)->_server_heartbeat_started;
}

apm& apm::on_server_heartbeat_failed(
    std::function<void MONGOCXX_ABI_CDECL(v1::events::server_heartbeat_failed const&)> fn) {
    impl::with(this)->_server_heartbeat_failed = std::move(fn);
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(v1::events::server_heartbeat_failed const&)> apm::server_heartbeat_failed()
    const {
    return impl::with(this)->_server_heartbeat_failed;
}

apm& apm::on_server_heartbeat_succeeded(
    std::function<void MONGOCXX_ABI_CDECL(v1::events::server_heartbeat_succeeded const&)> fn) {
    impl::with(this)->_server_heartbeat_succeeded = std::move(fn);
    return *this;
}

std::function<void MONGOCXX_ABI_CDECL(v1::events::server_heartbeat_succeeded const&)> apm::server_heartbeat_succeeded()
    const {
    return impl::with(this)->_server_heartbeat_succeeded;
}

auto apm::internal::command_started(apm const& self) -> fn_type<v1::events::command_started> const& {
    return impl::with(self)._command_started;
}

auto apm::internal::command_failed(apm const& self) -> fn_type<v1::events::command_failed> const& {
    return impl::with(self)._command_failed;
}

auto apm::internal::command_succeeded(apm const& self) -> fn_type<v1::events::command_succeeded> const& {
    return impl::with(self)._command_succeeded;
}

auto apm::internal::server_closed(apm const& self) -> fn_type<v1::events::server_closed> const& {
    return impl::with(self)._server_closed;
}

auto apm::internal::server_description_changed(apm const& self)
    -> fn_type<v1::events::server_description_changed> const& {
    return impl::with(self)._server_description_changed;
}

auto apm::internal::server_opening(apm const& self) -> fn_type<v1::events::server_opening> const& {
    return impl::with(self)._server_opening;
}

auto apm::internal::topology_closed(apm const& self) -> fn_type<v1::events::topology_closed> const& {
    return impl::with(self)._topology_closed;
}

auto apm::internal::topology_description_changed(apm const& self)
    -> fn_type<v1::events::topology_description_changed> const& {
    return impl::with(self)._topology_description_changed;
}

auto apm::internal::topology_opening(apm const& self) -> fn_type<v1::events::topology_opening> const& {
    return impl::with(self)._topology_opening;
}

auto apm::internal::server_heartbeat_started(apm const& self) -> fn_type<v1::events::server_heartbeat_started> const& {
    return impl::with(self)._server_heartbeat_started;
}

auto apm::internal::server_heartbeat_failed(apm const& self) -> fn_type<v1::events::server_heartbeat_failed> const& {
    return impl::with(self)._server_heartbeat_failed;
}

auto apm::internal::server_heartbeat_succeeded(apm const& self)
    -> fn_type<v1::events::server_heartbeat_succeeded> const& {
    return impl::with(self)._server_heartbeat_succeeded;
}

auto apm::internal::command_started(apm& self) -> fn_type<v1::events::command_started>& {
    return impl::with(self)._command_started;
}

auto apm::internal::command_failed(apm& self) -> fn_type<v1::events::command_failed>& {
    return impl::with(self)._command_failed;
}

auto apm::internal::command_succeeded(apm& self) -> fn_type<v1::events::command_succeeded>& {
    return impl::with(self)._command_succeeded;
}

auto apm::internal::server_closed(apm& self) -> fn_type<v1::events::server_closed>& {
    return impl::with(self)._server_closed;
}

auto apm::internal::server_description_changed(apm& self) -> fn_type<v1::events::server_description_changed>& {
    return impl::with(self)._server_description_changed;
}

auto apm::internal::server_opening(apm& self) -> fn_type<v1::events::server_opening>& {
    return impl::with(self)._server_opening;
}

auto apm::internal::topology_closed(apm& self) -> fn_type<v1::events::topology_closed>& {
    return impl::with(self)._topology_closed;
}

auto apm::internal::topology_description_changed(apm& self) -> fn_type<v1::events::topology_description_changed>& {
    return impl::with(self)._topology_description_changed;
}

auto apm::internal::topology_opening(apm& self) -> fn_type<v1::events::topology_opening>& {
    return impl::with(self)._topology_opening;
}

auto apm::internal::server_heartbeat_started(apm& self) -> fn_type<v1::events::server_heartbeat_started>& {
    return impl::with(self)._server_heartbeat_started;
}

auto apm::internal::server_heartbeat_failed(apm& self) -> fn_type<v1::events::server_heartbeat_failed>& {
    return impl::with(self)._server_heartbeat_failed;
}

auto apm::internal::server_heartbeat_succeeded(apm& self) -> fn_type<v1::events::server_heartbeat_succeeded>& {
    return impl::with(self)._server_heartbeat_succeeded;
}

namespace {

// An APM callback exiting via an exception is documented as being undefined behavior.
// For QoI, terminate the program before allowing the exception to bypass libmongoc code.
template <typename Fn>
void exception_guard(char const* source, Fn fn) noexcept {
    try {
        fn();
    } catch (...) {
        std::cerr << "fatal error: APM callback " << source << " exited via an exception" << std::endl;
        std::terminate();
    }
}

void command_started(mongoc_apm_command_started_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_command_started_get_context(v));
    auto const event = v1::events::command_started::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::command_started(context)(event); });
}

void command_failed(mongoc_apm_command_failed_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_command_failed_get_context(v));
    auto const event = v1::events::command_failed::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::command_failed(context)(event); });
}

void command_succeeded(mongoc_apm_command_succeeded_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_command_succeeded_get_context(v));
    auto const event = v1::events::command_succeeded::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::command_succeeded(context)(event); });
}

void server_opening(mongoc_apm_server_opening_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_server_opening_get_context(v));
    auto const event = v1::events::server_opening::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::server_opening(context)(event); });
}

void server_closed(mongoc_apm_server_closed_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_server_closed_get_context(v));
    auto const event = v1::events::server_closed::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::server_closed(context)(event); });
}

void server_description_changed(mongoc_apm_server_changed_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_server_changed_get_context(v));
    auto const event = v1::events::server_description_changed::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::server_description_changed(context)(event); });
}

void topology_opening(mongoc_apm_topology_opening_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_topology_opening_get_context(v));
    auto const event = v1::events::topology_opening::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::topology_opening(context)(event); });
}

void topology_closed(mongoc_apm_topology_closed_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_topology_closed_get_context(v));
    auto const event = v1::events::topology_closed::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::topology_closed(context)(event); });
}

void topology_description_changed(mongoc_apm_topology_changed_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_topology_changed_get_context(v));
    auto const event = v1::events::topology_description_changed::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::topology_description_changed(context)(event); });
}

void server_heartbeat_started(mongoc_apm_server_heartbeat_started_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_server_heartbeat_started_get_context(v));
    auto const event = v1::events::server_heartbeat_started::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::server_heartbeat_started(context)(event); });
}

void server_heartbeat_failed(mongoc_apm_server_heartbeat_failed_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_server_heartbeat_failed_get_context(v));
    auto const event = v1::events::server_heartbeat_failed::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::server_heartbeat_failed(context)(event); });
}

void server_heartbeat_succeeded(mongoc_apm_server_heartbeat_succeeded_t const* v) noexcept {
    auto const& context = *static_cast<v1::apm*>(libmongoc::apm_server_heartbeat_succeeded_get_context(v));
    auto const event = v1::events::server_heartbeat_succeeded::internal::make(v);
    exception_guard(__func__, [&] { v1::apm::internal::server_heartbeat_succeeded(context)(event); });
}

class apm_callbacks {
   public:
    mongoc_apm_callbacks_t* _callbacks = libmongoc::apm_callbacks_new();

    ~apm_callbacks() {
        libmongoc::apm_callbacks_destroy(_callbacks);
    }

    apm_callbacks(apm_callbacks&& other) = delete;
    apm_callbacks& operator=(apm_callbacks&& other) = delete;
    apm_callbacks(apm_callbacks const& other) = delete;
    apm_callbacks& operator=(apm_callbacks const& other) = delete;

    explicit apm_callbacks(v1::apm const& apm) {
        if (v1::apm::internal::command_started(apm)) {
            libmongoc::apm_set_command_started_cb(_callbacks, command_started);
        }

        if (v1::apm::internal::command_failed(apm)) {
            libmongoc::apm_set_command_failed_cb(_callbacks, command_failed);
        }

        if (v1::apm::internal::command_succeeded(apm)) {
            libmongoc::apm_set_command_succeeded_cb(_callbacks, command_succeeded);
        }

        if (v1::apm::internal::server_opening(apm)) {
            libmongoc::apm_set_server_opening_cb(_callbacks, server_opening);
        }

        if (v1::apm::internal::server_closed(apm)) {
            libmongoc::apm_set_server_closed_cb(_callbacks, server_closed);
        }

        if (v1::apm::internal::server_description_changed(apm)) {
            libmongoc::apm_set_server_changed_cb(_callbacks, server_description_changed);
        }

        if (v1::apm::internal::topology_opening(apm)) {
            libmongoc::apm_set_topology_opening_cb(_callbacks, topology_opening);
        }

        if (v1::apm::internal::topology_closed(apm)) {
            libmongoc::apm_set_topology_closed_cb(_callbacks, topology_closed);
        }

        if (v1::apm::internal::topology_description_changed(apm)) {
            libmongoc::apm_set_topology_changed_cb(_callbacks, topology_description_changed);
        }

        if (v1::apm::internal::server_heartbeat_started(apm)) {
            libmongoc::apm_set_server_heartbeat_started_cb(_callbacks, server_heartbeat_started);
        }

        if (v1::apm::internal::server_heartbeat_failed(apm)) {
            libmongoc::apm_set_server_heartbeat_failed_cb(_callbacks, server_heartbeat_failed);
        }

        if (v1::apm::internal::server_heartbeat_succeeded(apm)) {
            libmongoc::apm_set_server_heartbeat_succeeded_cb(_callbacks, server_heartbeat_succeeded);
        }
    }
};

} // namespace

void apm::internal::set_apm_callbacks(mongoc_client_t* client, v1::apm& apm) {
    libmongoc::client_set_apm_callbacks(client, apm_callbacks{apm}._callbacks, &apm);
}

} // namespace v1
} // namespace mongocxx
