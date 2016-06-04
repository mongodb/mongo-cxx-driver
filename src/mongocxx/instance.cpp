// Copyright 2014 MongoDB Inc.
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

#include <mutex>
#include <utility>

#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/logger.hpp>
#include <mongocxx/private/libmongoc.hpp>

#include <mongocxx/config/private/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

namespace {

log_level convert_log_level(::mongoc_log_level_t mongoc_log_level) {
    switch (mongoc_log_level) {
        case MONGOC_LOG_LEVEL_ERROR:
            return log_level::k_error;
        case MONGOC_LOG_LEVEL_CRITICAL:
            return log_level::k_critical;
        case MONGOC_LOG_LEVEL_WARNING:
            return log_level::k_warning;
        case MONGOC_LOG_LEVEL_MESSAGE:
            return log_level::k_message;
        case MONGOC_LOG_LEVEL_INFO:
            return log_level::k_info;
        case MONGOC_LOG_LEVEL_DEBUG:
            return log_level::k_debug;
        case MONGOC_LOG_LEVEL_TRACE:
            return log_level::k_trace;
        default:
            MONGOCXX_UNREACHABLE;
    }
}

void null_log_handler(::mongoc_log_level_t, const char *, const char *, void *) {
}

void user_log_handler(::mongoc_log_level_t mongoc_log_level, const char *log_domain,
                      const char *message, void *user_data) {
    (*static_cast<logger *>(user_data))(convert_log_level(mongoc_log_level),
                                        stdx::string_view{log_domain}, stdx::string_view{message});
}

std::recursive_mutex instance_mutex;
instance *current_instance = nullptr;
std::unique_ptr<instance> global_instance;

}  // namespace

class instance::impl {
   public:
    impl(std::unique_ptr<logger> logger) : _user_logger(std::move(logger)) {
        libmongoc::init();
        if (_user_logger) {
            libmongoc::log_set_handler(user_log_handler, _user_logger.get());
            mongoc_log(MONGOC_LOG_LEVEL_INFO, "mongocxx", "libmongoc logging callback enabled");
        } else {
            libmongoc::log_set_handler(null_log_handler, nullptr);
        }
    }

    ~impl() {
        // If we had a user logger, remove it so that it can't be used by the driver after it goes
        // out of scope
        if (_user_logger) {
            libmongoc::log_set_handler(null_log_handler, nullptr);
        }
        libmongoc::cleanup();
    }

    const std::unique_ptr<logger> _user_logger;
};

instance::instance() : instance(nullptr) {
}

instance::instance(std::unique_ptr<logger> logger) {
    std::lock_guard<std::recursive_mutex> lock(instance_mutex);
    if (current_instance) {
        throw logic_error{error_code::k_instance_already_exists};
    }
    _impl = stdx::make_unique<impl>(std::move(logger));
    current_instance = this;
}

instance::instance(instance &&) noexcept = default;
instance &instance::operator=(instance &&) noexcept = default;

instance::~instance() {
    std::lock_guard<std::recursive_mutex> lock(instance_mutex);
    if (current_instance != this) std::abort();
    _impl.reset();
    current_instance = nullptr;
}

instance &instance::current() {
    std::lock_guard<std::recursive_mutex> lock(instance_mutex);
    if (!current_instance) {
        global_instance.reset(new instance);
        current_instance = global_instance.get();
    }
    return *current_instance;
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
