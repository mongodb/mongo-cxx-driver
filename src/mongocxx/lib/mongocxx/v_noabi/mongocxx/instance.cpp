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

} // namespace

class instance::impl {
   public:
    v1::instance _instance;

    impl(std::unique_ptr<logger> handler) try : _instance{std::move(handler)} {
    } catch (v1::exception const&) {
        throw v_noabi::logic_error{error_code::k_cannot_recreate_instance};
    }
};

instance::instance() : instance(nullptr) {}

instance::instance(std::unique_ptr<v_noabi::logger> logger) : _impl{bsoncxx::make_unique<impl>(std::move(logger))} {
    current_instance.store(this, std::memory_order_relaxed);
}

instance::instance(instance&& other) noexcept = default;
instance& instance::operator=(instance&& other) noexcept = default;

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
