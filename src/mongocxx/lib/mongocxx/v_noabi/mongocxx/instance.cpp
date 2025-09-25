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

#include <mongocxx/instance.hpp>

//

#include <mongocxx/v1/exception.hpp>
#include <mongocxx/v1/instance.hpp>
#include <mongocxx/v1/logger.hpp>

#include <atomic>
#include <stdexcept>
#include <utility>

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/logger.hpp>

#include <bsoncxx/private/make_unique.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {

namespace {

// To support mongocxx::v_noabi::instance::current().
std::atomic<instance*> current_instance{nullptr};

// Sentinel value denoting the current instance has been destroyed.
instance* sentinel() {
    alignas(instance) static unsigned char value[sizeof(instance)];
    return reinterpret_cast<instance*>(value);
}

void custom_log_handler(
    mongoc_log_level_t log_level,
    char const* domain,
    char const* message,
    void* user_data) noexcept {
    (*static_cast<v_noabi::logger*>(user_data))(
        static_cast<v_noabi::log_level>(log_level),
        bsoncxx::v1::stdx::string_view(domain),
        bsoncxx::v1::stdx::string_view(message));
}

} // namespace

class instance::impl {
   public:
    v1::instance _instance;
    std::unique_ptr<logger> _handler;

    impl(impl&&) = delete;
    impl& operator=(impl&&) = delete;
    impl(impl const&) = delete;
    impl& operator=(impl const&) = delete;

    impl(std::unique_ptr<logger> handler) try : _instance{v1::default_logger{}}, _handler{std::move(handler)} {
        // For backward compatibility, allow v1::instance to initialize mongoc with the default log handler, then set
        // the custom logger AFTER initialization has already completed.
        if (_handler) {
            libmongoc::log_set_handler(&custom_log_handler, _handler.get());

            // Inform the user that a custom log handler has been registered.
            // Cannot use mocked `libmongoc::log()` due to varargs.
            mongoc_log(MONGOC_LOG_LEVEL_INFO, "mongocxx", "libmongoc logging callback enabled");
        } else {
            libmongoc::log_set_handler(nullptr, nullptr);
        }
    } catch (v1::exception const&) {
        throw v_noabi::logic_error{error_code::k_cannot_recreate_instance};
    }
};

instance::instance() : instance(nullptr) {}

instance::instance(std::unique_ptr<v_noabi::logger> logger) : _impl{bsoncxx::make_unique<impl>(std::move(logger))} {
    current_instance.store(this, std::memory_order_relaxed);
}

instance::instance(instance&&) noexcept = default;
instance& instance::operator=(instance&&) noexcept = default;

instance::~instance() {
    current_instance.store(sentinel(), std::memory_order_relaxed);
}

instance& instance::current() {
    if (auto const p = current_instance.load(std::memory_order_relaxed)) {
        if (p == sentinel()) {
            throw v_noabi::logic_error{error_code::k_instance_destroyed};
        } else {
            return *p;
        }

    } else {
        static instance the_instance;
        return the_instance;
    }
}

} // namespace v_noabi
} // namespace mongocxx
