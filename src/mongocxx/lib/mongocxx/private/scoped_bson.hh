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

#include <bsoncxx/v1/array/value.hpp>
#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/document/view.hpp>

#include <mongocxx/v1/detail/macros.hpp>

#include <cstdint>
#include <utility>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/export.hh>

namespace mongocxx {

class scoped_bson_view {
   private:
    bsoncxx::v1::document::view _view;
    bson_t _bson = {}; // Non-owning.

   public:
    ~scoped_bson_view() = default;

    // Handled by the copy constructor.
    // scoped_bson_view(scoped_bson_view&& other) noexcept = default;

    // Handled by the copy assignment operator.
    // scoped_bson_view& operator=(scoped_bson_view&& other) noexcept = default;

    scoped_bson_view(scoped_bson_view const& other) noexcept : _view{other._view} {
        this->sync_bson();
    }

    scoped_bson_view& operator=(scoped_bson_view const& other) noexcept {
        _view = other._view;
        this->sync_bson();
        return *this;
    }

    // Initialize as an empty document.
    scoped_bson_view() {
        this->sync_bson();
    }

    // Prevent mistakenly creating a view of a temporary object:
    // ```cpp
    // scoped_bson_view v{BCON_NEW("x", "1")}; // Leak!
    // ```
    explicit scoped_bson_view(bson_t*) = delete;

    explicit scoped_bson_view(bson_t const* v) : _view{v ? bson_get_data(v) : nullptr} {
        this->sync_bson();
    }

    explicit scoped_bson_view(std::nullptr_t) : scoped_bson_view{static_cast<bson_t const*>(nullptr)} {}

    // Prevent mistakently creating a view of a temporary object.
    explicit scoped_bson_view(bsoncxx::v1::document::value&&) = delete;

    /* explicit(false) */ scoped_bson_view(bsoncxx::v1::document::value const& v) : scoped_bson_view{v.view()} {}

    /* explicit(false) */ scoped_bson_view(bsoncxx::v1::document::view v) : _view{v} {
        this->sync_bson();
    }

    std::uint8_t const* data() const {
        return _view.data();
    }

    bson_t const* bson() const {
        return _view ? &_bson : nullptr;
    }

    /* explicit(false) */ operator bson_t const*() const {
        return this->bson();
    }

    bsoncxx::v1::document::view view() const {
        return _view;
    }

    bsoncxx::v1::array::view array_view() const {
        return bsoncxx::v1::array::view{_view.data()};
    }

    bsoncxx::v1::document::value value() const {
        return bsoncxx::v1::document::value{_view};
    }

    // Return an owning copy.
    bson_t* copy() const {
        return _view ? bson_copy(&_bson) : nullptr;
    }

    class out_ptr_type {
        friend scoped_bson_view;

       private:
        scoped_bson_view& _self;
        bson_t const* _bson = nullptr;

        /* explicit(false) */ out_ptr_type(scoped_bson_view& self) : _self{self} {}

       public:
        ~out_ptr_type() {
            _self = scoped_bson_view{_bson};
        }

        out_ptr_type(out_ptr_type&&) = delete;
        out_ptr_type& operator=(out_ptr_type&&) = delete;
        out_ptr_type(out_ptr_type const&) = delete;
        out_ptr_type& operator=(out_ptr_type const&) = delete;

        /* explicit(false) */ operator bson_t const**() && {
            return &_bson;
        }

        /* explicit(false) */ operator bson_t const*() const& = delete;
    };

    out_ptr_type out_ptr() & {
        return {*this};
    }

   private:
    void sync_bson() {
        if (_view && !bson_init_static(&_bson, _view.data(), _view.length())) {
            MONGOCXX_PRIVATE_UNREACHABLE;
        }
    }
};

class scoped_bson {
   private:
    bsoncxx::v1::document::value _value;
    bson_t _bson = {}; // Non-owning.

   public:
    ~scoped_bson() = default;

    scoped_bson(scoped_bson&& other) noexcept : _value{std::move(other._value)} {
        this->sync_bson();
        other._bson = {};
    }

    scoped_bson& operator=(scoped_bson&& other) noexcept {
        _value = std::move(other._value);
        this->sync_bson();
        other._bson = {};
        return *this;
    }

    scoped_bson(scoped_bson const& other) : _value{other._value} {
        this->sync_bson();
    }

    scoped_bson& operator=(scoped_bson const& other) {
        _value = other._value;
        this->sync_bson();
        return *this;
    }

    // Initialize as an empty document.
    scoped_bson() {
        this->sync_bson();
    }

    // Copy `v`.
    explicit scoped_bson(scoped_bson_view v) : scoped_bson{v.view()} {}

    // Takes ownership of `v`. `v` is destroyed.
    explicit scoped_bson(bson_t* v)
        : _value{[&v]() -> bsoncxx::v1::document::value {
              if (!v) {
                  return {nullptr, &bson_free};
              } else if (bson_empty(v)) {
                  bson_destroy(v);
                  return {}; // Avoid unnecessary allocations.
              } else {
                  return {bson_destroy_with_steal(v, true, nullptr), &bson_free};
              }
          }()} {
        this->sync_bson();
    }

    // Copy `v`.
    explicit scoped_bson(bson_t const* v) : scoped_bson{bsoncxx::v1::document::view{v ? bson_get_data(v) : nullptr}} {}

    explicit scoped_bson(std::nullptr_t) : scoped_bson{static_cast<bson_t*>(nullptr)} {}

    explicit scoped_bson(bsoncxx::v1::document::value v) : _value{std::move(v)} {
        this->sync_bson();
    }

    explicit scoped_bson(bsoncxx::v1::document::view v) : _value{v} {
        this->sync_bson();
    }

    // For test convenience only: use `BCON_NEW` instead for explicit type correctness.
    // ```cpp
    // scoped_bson doc{R"({"x": 1})"}; // {"x": 1}
    // ```
    explicit scoped_bson(bsoncxx::v1::stdx::string_view json)
        : scoped_bson{[&json]() -> bson_t* {
              bson_error_t error = {};
              if (auto const res = bson_new_from_json(
                      reinterpret_cast<std::uint8_t const*>(json.data()), static_cast<ssize_t>(json.size()), &error)) {
                  return res;
              }
              MONGOCXX_PRIVATE_UNREACHABLE;
          }()} {}

    scoped_bson& operator+=(scoped_bson const& other) {
        return (*this += scoped_bson_view{other.view()});
    }

    // Minimal support for building complex documents via concatenation.
    // ```cpp
    // scoped_bson doc{R"({"x": 1})"}; // {"x": 1}
    // doc += scoped_bson{R"({"x": 2})"};
    // return doc;                     // {"x": 1, "x": 2}
    // ```
    MONGOCXX_ABI_EXPORT_CDECL_TESTING(scoped_bson&) operator+=(scoped_bson_view const& other);

    std::uint8_t const* data() const {
        return _value.data();
    }

    bson_t const* bson() const& {
        return _value ? &_bson : nullptr;
    }

    /* explicit(false) */ operator bson_t const*() const {
        return this->bson();
    }

    bsoncxx::v1::document::view view() const {
        return _value;
    }

    // Convenient helper to obtain an array view of the underlying BSON data.
    bsoncxx::v1::array::view array_view() const {
        return bsoncxx::v1::array::view{_value.data()};
    }

    scoped_bson_view bson_view() const {
        return {_value};
    }

    bsoncxx::v1::document::value const& value() const& {
        return _value;
    }

    bsoncxx::v1::document::value value() && {
        return std::move(_value); // Assign-or-destroy-only.
    }

    // Return an owning copy.
    bson_t* copy() const {
        return _value ? bson_copy(&_bson) : nullptr;
    }

    class out_ptr_type {
        friend scoped_bson;

       private:
        scoped_bson& _self;
        bson_t _bson = BSON_INITIALIZER; // Always pass a valid (overwritable) empty document.

        /* explicit(false) */ out_ptr_type(scoped_bson& self) : _self{self} {}

       public:
        ~out_ptr_type() {
            if (bson_empty(&_bson)) {
                bson_destroy(&_bson);
                _self = scoped_bson{}; // Avoid unnecessary allocations.
            } else {
                _self = scoped_bson{&_bson}; // Ownership transfer.
            }
        }

        out_ptr_type(out_ptr_type&&) = delete;
        out_ptr_type& operator=(out_ptr_type&&) = delete;
        out_ptr_type(out_ptr_type const&) = delete;
        out_ptr_type& operator=(out_ptr_type const&) = delete;

        /* explicit(false) */ operator bson_t*() && {
            return &_bson;
        }

        /* explicit(false) */ operator bson_t*() const& = delete;
    };

    out_ptr_type out_ptr() & {
        return {*this};
    }

    class inout_ptr_type {
        friend scoped_bson;

       private:
        scoped_bson& _self;
        bson_t _bson = BSON_INITIALIZER; // Always pass a valid document (even when invalid).

        /* explicit(false) */ inout_ptr_type(scoped_bson& self) : _self{self} {
            if (self._value && !self._value.empty()) {
                bson_copy_to(&self._bson, &_bson); // Avoid unnecessary allocations.
            }
        }

       public:
        ~inout_ptr_type() {
            if (_bson.len == 0) {
                // Some API *conditionally* use `bson_steal()` to take ownership of `_bson`.
                _self = scoped_bson{nullptr};
            } else if (bson_empty(&_bson)) {
                bson_destroy(&_bson);
                _self = scoped_bson{}; // Avoid unnecessary allocations.
            } else {
                _self = scoped_bson{&_bson}; // Ownership transfer.
            }
        }

        inout_ptr_type(inout_ptr_type&&) = delete;
        inout_ptr_type& operator=(inout_ptr_type&&) = delete;
        inout_ptr_type(inout_ptr_type const&) = delete;
        inout_ptr_type& operator=(inout_ptr_type const&) = delete;

        /* explicit(false) */ operator bson_t*() && {
            return &_bson;
        }

        /* explicit(false) */ operator bson_t*() const& = delete;
    };

    inout_ptr_type inout_ptr() & {
        return {*this};
    }

   private:
    void sync_bson() {
        if (_value && !bson_init_static(&_bson, _value.data(), _value.length())) {
            MONGOCXX_PRIVATE_UNREACHABLE;
        }
    }
};

} // namespace mongocxx
