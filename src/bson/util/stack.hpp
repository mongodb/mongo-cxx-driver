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

#pragma once

#include "driver/config/prelude.hpp"

#include <type_traits>
#include <memory>
#include <list>

namespace bson {
namespace util {

template <typename T, std::size_t size>
class stack {
   public:
    stack() : _bucket_index(0), _bucket_size(size), _is_empty(true) {}

    ~stack() {
        while (!empty()) {
            pop_back();
        }

        while (!_buckets.empty()) {
            operator delete(_buckets.back());
            _buckets.pop_back();
        }
    }

    bool empty() const { return _is_empty; }

    T &back() { return *(_get_ptr()); }

    template <typename... Args>
    void emplace_back(Args &&... args) {
        if (_is_empty) {
            _is_empty = false;
        } else {
            _inc();
        }

        new (_get_ptr()) T(std::forward<Args>(args)...);
    }

    void pop_back() { _dec(); }

    void unsafe_reset() {
        _bucket_index = 0;
        if (!_buckets.empty()) {
            _bucket_iter = _buckets.begin();
        }
        _is_empty = true;
    }

   private:
    typename std::aligned_storage<sizeof(T), alignof(T)>::type _object_memory[size];

    std::list<T *> _buckets;

    typename decltype(_buckets)::iterator _bucket_iter;

    int _bucket_index;
    int _bucket_size;
    bool _is_empty;

    T *_get_ptr() {
        if (_bucket_size == size) {
            return reinterpret_cast<T *>(_object_memory) + _bucket_index;
        } else {
            return *(_bucket_iter) + _bucket_index;
        }
    }

    void _inc() {
        if (_bucket_index == _bucket_size - 1) {
            _bucket_index = 0;
            _bucket_size *= 2;

            if (_buckets.empty()) {
                // first pass at needing dynamic memory
                _buckets.emplace_back(
                    reinterpret_cast<T *>(operator new(sizeof(T) * _bucket_size)));

                _bucket_iter = _buckets.begin();
            } else if (_bucket_size != size * 2) {
                // we're _not_ transitioning from stack to heap
                auto tmp_iter = _bucket_iter;

                if (++tmp_iter == _buckets.end()) {
                    _buckets.emplace_back(
                        reinterpret_cast<T *>(operator new(sizeof(T) * _bucket_size)));
                }
                ++_bucket_iter;
            }
        } else {
            ++_bucket_index;
        }
    }

    void _dec() {
        _get_ptr()->~T();

        if (_bucket_index == 0) {
            if (_bucket_size == size) {
                /* we're already in object memory */
                _is_empty = true;
            } else {
                /* we're on the linked list */
                _bucket_size /= 2;
                _bucket_index = _bucket_size - 1;

                if (_bucket_iter != _buckets.begin()) {
                    --_bucket_iter;
                }
            }
        } else {
            --_bucket_index;
        }
    }
};

}  // namespace util
}  // namespace bson

#include "driver/config/postlude.hpp"
