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

#include <mongocxx/v1/instance.hpp>

//

#include <bsoncxx/v1/detail/macros.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/version.hpp>
#include <mongocxx/v1/logger.hpp>

#include <mongocxx/v1/exception.hh>

#include <atomic>
#include <memory>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>

#include <bsoncxx/private/immortal.hh>
#include <bsoncxx/private/make_unique.hh>

#include <mongocxx/private/config/config.hh>
#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {

using code = v1::instance::errc;

static_assert(!std::is_move_constructible<instance>::value, "mongocxx::v1::instance must be non-moveable");
static_assert(!std::is_copy_constructible<instance>::value, "mongocxx::v1::instance must be non-copyable");

namespace {

// 0: no instance object has been created.
// 1: there is a single instance object.
// 2: the single instance object has been destroyed.
std::atomic_int instance_state{0};

void custom_log_handler(
    mongoc_log_level_t log_level,
    char const* domain,
    char const* message,
    void* user_data) noexcept {
    (*static_cast<v1::logger*>(user_data))(
        static_cast<v1::log_level>(log_level),
        bsoncxx::v1::stdx::string_view(domain),
        bsoncxx::v1::stdx::string_view(message));
}

} // namespace

class instance::impl {
   public:
    std::unique_ptr<v1::logger> _handler;

    ~impl() {
        if (_handler) {
            libmongoc::log_set_handler(nullptr, nullptr);
        }

        libmongoc::cleanup();

        instance_state.fetch_add(1, std::memory_order_release);
    }

    impl(impl&&) = delete;
    impl& operator=(impl&&) = delete;
    impl(impl const&) = delete;
    impl& operator=(impl const&) = delete;

    explicit impl(std::unique_ptr<v1::logger> handler) : _handler{std::move(handler)} {
        this->init(true);
    }

    explicit impl(v1::default_logger tag) {
        (void)tag;
        this->init(false);
    }

   private:
    void init(bool set_custom_handler) {
        {
            int expected = 0;

            if (!instance_state.compare_exchange_strong(
                    expected, 1, std::memory_order_acquire, std::memory_order_relaxed)) {
                throw v1::exception::internal::make(std::error_code{code::multiple_instances});
            }
        }

        if (set_custom_handler) {
            if (auto ptr = _handler.get()) {
                libmongoc::log_set_handler(&custom_log_handler, ptr);
            } else {
                libmongoc::log_set_handler(nullptr, nullptr);
            }
        }

        libmongoc::init();

        {
            // Avoid /Zc:__cplusplus problems with MSVC.
#pragma push_macro("STDCXX")
#undef STDCXX
#if defined(_MSVC_LANG)
#define STDCXX BSONCXX_PRIVATE_STRINGIFY(_MSVC_LANG)
#else
#define STDCXX BSONCXX_PRIVATE_STRINGIFY(__cplusplus)
#endif

            // Handshake data can only be appended once (or it will return false): this is that "once".
            // Despite the name, mongoc_handshake_data_append() *prepends* the platform string.
            // Use " / " to delimit handshake date for mongocxx and mongoc.
            (void)libmongoc::handshake_data_append(
                "mongocxx",
                MONGOCXX_VERSION_STRING,
                "CXX=" MONGOCXX_COMPILER_ID " " MONGOCXX_COMPILER_VERSION " stdcxx=" STDCXX " / ");

#pragma pop_macro("STDCXX")
        }
    }
};

instance::~instance() = default;

instance::instance() : instance{v1::default_logger{}} {}

instance::instance(std::unique_ptr<v1::logger> handler) : _impl{bsoncxx::make_unique<impl>(std::move(handler))} {}

instance::instance(v1::default_logger tag) : _impl{bsoncxx::make_unique<impl>(tag)} {
    (void)tag;
}

std::error_category const& instance::error_category() {
    class type final : public std::error_category {
        char const* name() const noexcept override {
            return "mongocxx::v1::instance";
        }

        std::string message(int v) const noexcept override {
            switch (static_cast<code>(v)) {
                case code::zero:
                    return "zero";
                case code::multiple_instances:
                    return "cannot construct multiple instance objects in a given process";
                default:
                    return std::string(this->name()) + ':' + std::to_string(v);
            }
        }

        bool equivalent(int v, std::error_condition const& ec) const noexcept override {
            if (ec.category() == v1::source_error_category()) {
                using condition = v1::source_errc;

                auto const source = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::multiple_instances:
                        return source == condition::mongocxx;

                    case code::zero:
                    default:
                        return false;
                }
            }

            if (ec.category() == v1::type_error_category()) {
                using condition = v1::type_errc;

                auto const type = static_cast<condition>(ec.value());

                switch (static_cast<code>(v)) {
                    case code::multiple_instances:
                        return type == condition::runtime_error;

                    case code::zero:
                    default:
                        return false;
                }
            }

            return false;
        }
    };

    static bsoncxx::immortal<type> const instance;

    return instance.value();
}

} // namespace v1
} // namespace mongocxx
