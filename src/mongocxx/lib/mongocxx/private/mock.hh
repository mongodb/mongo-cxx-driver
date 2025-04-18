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

#pragma once

#include <bsoncxx/v1/detail/macros.hpp>

#include <array>
#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <stack>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <bsoncxx/stdx/type_traits.hpp>

#include <bsoncxx/private/bson.hh>

namespace mongocxx {
namespace test_util {

template <typename CRTP, typename R>
struct mock_invoke_fn {
    template <typename Self, typename... Args>
    static R invoke(Self self, Args... args) {
        auto crtp = static_cast<CRTP*>(self);
        auto instance = crtp->active_instance();
        if (instance) {
            while (!instance->_callbacks.empty()) {
                if (instance->_callbacks.top()._conditional(args...)) {
                    return instance->_callbacks.top()._callback(args...);
                }
                instance->_callbacks.pop();
            }
        }

        return crtp->_func(args...);
    }
};

template <typename CRTP, typename T>
struct mock_invoke;

template <typename CRTP, typename R, typename... Args>
struct mock_invoke<CRTP, R(Args...)> {
    R operator()(Args... args) {
        return mock_invoke_fn<CRTP, R>::invoke(this, args...);
    }
};

BSONCXX_PRIVATE_WARNINGS_PUSH();
#if defined(__GNUC__) && (__GNUC__ >= 6) && !defined(__clang__)
BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wignored-attributes"));
#endif

template <typename CRTP, typename... Args>
struct mock_invoke<CRTP, bson_t const*(Args...)> {
    bson_t const* operator()(Args... args) {
        return mock_invoke_fn<CRTP, bson_t const*>::invoke(this, args...);
    }
};

template <typename CRTP, typename... Args>
struct mock_invoke<CRTP, bson_t*(Args...)> {
    bson_t* operator()(Args... args) {
        return mock_invoke_fn<CRTP, bson_t*>::invoke(this, args...);
    }
};

BSONCXX_PRIVATE_WARNINGS_POP();

template <typename T>
class mock;

template <typename R, typename... Args>
class mock<R(MONGOCXX_ABI_CDECL*)(Args...)> : public mock_invoke<mock<R(MONGOCXX_ABI_CDECL*)(Args...)>, R(Args...)> {
   public:
    using underlying_ptr = R (*)(Args...);
    using callback = std::function<R(Args...)>;
    using conditional = std::function<bool(Args...)>;

    class rule {
        friend mock;
        friend mock_invoke_fn<mock<R(MONGOCXX_ABI_CDECL*)(Args...)>, R>;

       public:
        rule(callback callback) : _callback(std::move(callback)) {
            times(1);
        }

        void times(int n) {
            until([n](Args...) mutable -> bool { return n-- > 0; });
        }

        void forever() {
            until([](Args...) { return true; });
        }

        void until(conditional conditional) {
            _conditional = std::move(conditional);
        }

       private:
        callback _callback;
        conditional _conditional;
    };

    class instance {
        friend mock;
        friend mock_invoke_fn<mock<R(MONGOCXX_ABI_CDECL*)(Args...)>, R>;

       public:
        instance(instance const&) = delete;
        instance& operator=(instance const&) = delete;

        ~instance() {
            _parent->destroy_active_instance();
        }

        // Interposing functions replace C-Driver functionality completely
        rule& interpose(std::function<R(Args...)> const& func) {
            _callbacks.emplace([func](Args... args) { return func(args...); });

            return _callbacks.top();
        }

        template <typename T, typename... U, bsoncxx::detail::requires_t<int, std::is_same<T, R>> = 0>
        rule& interpose(T r, U... rs) {
            std::array<R, sizeof...(rs) + 1> vec = {r, rs...};
            std::size_t i = 0;

            _callbacks.emplace([vec, i](Args...) mutable -> R {
                if (i == vec.size()) {
                    i = 0;
                }
                return vec[i++];
            });
            _callbacks.top().times(vec.size());

            return _callbacks.top();
        }

        // Visiting functions get called in addition to the original C-Driver function
        rule& visit(std::function<void(Args...)> func) {
            _callbacks.emplace([this, func](Args... args) {
                func(args...);
                return _parent->_func(args...);
            });

            return _callbacks.top();
        }

        std::size_t depth() const {
            return _callbacks.size();
        }

        bool empty() const {
            return _callbacks.empty();
        }

       private:
        instance(mock* parent) : _parent(parent) {}

        mock* _parent;
        std::stack<rule> _callbacks;
    };

    friend instance;
    friend mock_invoke_fn<mock<R(MONGOCXX_ABI_CDECL*)(Args...)>, R>;

    mock(underlying_ptr func) : _func(std::move(func)) {}
    mock(mock&&) = delete;
    mock(mock const&) = delete;
    mock& operator=(mock const&) = delete;

    std::unique_ptr<instance> create_instance() {
        std::unique_ptr<instance> mock_instance(new instance(this));
        active_instance(mock_instance.get());
        return mock_instance;
    }

   private:
    instance* active_instance() {
        auto const id = std::this_thread::get_id();
        std::lock_guard<std::mutex> lock(_active_instances_lock);
        auto const iterator = _active_instances.find(id);
        if (iterator != _active_instances.end()) {
            return iterator->second;
        }
        return nullptr;
    }

    void active_instance(instance* instance) {
        auto const id = std::this_thread::get_id();
        std::lock_guard<std::mutex> lock(_active_instances_lock);

        auto& current = _active_instances[id];
        assert(!current); // It is impossible to create two instances in a single thread
        current = instance;
    }

    void destroy_active_instance() {
        auto const id = std::this_thread::get_id();
        std::lock_guard<std::mutex> lock(_active_instances_lock);
        _active_instances.erase(id);
    }

    std::mutex _active_instances_lock;
    std::unordered_map<std::thread::id, instance*> _active_instances;
    underlying_ptr const _func;
};

} // namespace test_util
} // namespace mongocxx
