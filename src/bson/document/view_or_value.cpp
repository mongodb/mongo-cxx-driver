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

#include <cstdlib>
#include <cstring>

#include "bson/document/view_or_value.hpp"

namespace bson {
namespace document {

view_or_value::view_or_value(bson::document::view view) : _is_view(true), _view(std::move(view)) {}
view_or_value::view_or_value(bson::document::value value)
    : _is_view(false), _value(std::move(value)) {}

view_or_value::view_or_value(view_or_value&& rhs) : _is_view(true) { *this = std::move(rhs); }

view_or_value& view_or_value::operator=(view_or_value&& rhs) {
    if (!_is_view) {
        rhs._value.~value();
    }

    if (_is_view) {
        _view = std::move(rhs._view);
    } else {
        _value = std::move(rhs._value);
    }

    _is_view = rhs._is_view;

    rhs._is_view = true;

    return *this;
}

view_or_value::~view_or_value() {
    if (!_is_view) {
        _value.~value();
    }
}

document::view view_or_value::view() const {
    if (_is_view) {
        return _view;
    } else {
        return _value.view();
    }
}

view_or_value::operator document::view() const { return view(); }

}  // namespace document
}  // namespace bson
