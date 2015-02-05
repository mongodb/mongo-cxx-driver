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

#include <bsoncxx/types/value.hpp>

#include <cstdlib>
#include <cstring>

#include <bson.h>

#include <bsoncxx/document/element.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/json.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace types {

value::value() : _mode(mode::unset) {
}

value::value(const document::element& element) : _mode(mode::element), _element(&element) {
}

#define BSONCXX_ENUM(name, val) \
value::value(b_##name value) : \
    _mode(mode::variant), \
    _type(static_cast<bsoncxx::type>(val)), \
    _b_##name(std::move(value)) \
{}

#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM

value::value(const value& rhs) {
    *this = rhs;
}

value& value::operator=(const value& rhs) {
    switch(rhs._mode) {
        case mode::unset:
            _mode = mode::unset;
            break;
        case mode::element:
            _mode = mode::element;
            _element = rhs._element;
            break;
        case mode::variant:
            _mode = mode::variant;
            _type = rhs._type;
            switch (static_cast<int>(_type)) {
#define BSONCXX_ENUM(type, val) \
                case val: \
                    _b_##type = rhs.get_##type(); \
                    break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
            }
            break;
    }

    return *this;
}

value::value(value&& rhs) {
    *this = rhs;
}

value& value::operator=(value&& rhs) {
    *this = rhs;

    return *this;
}

value::~value() {
    switch(_mode) {
        case mode::unset:
            break;
        case mode::element:
            _element->~element();
            break;
        case mode::variant:
            switch (static_cast<int>(_type)) {
#define BSONCXX_ENUM(type, val) \
                case val: \
                    _b_##type.~b_##type(); \
                    break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
            }
            break;
    }
}

bsoncxx::type value::type() const {
    if (_mode == mode::element) {
        return _element->type();
    } else if (_mode == mode::variant) {
        return _type;
    } else {
        throw std::runtime_error("unset element");
    }
}

#define BSONCXX_ENUM(type, val) \
types::b_##type value::get_##type() const { \
    if (_mode == mode::element) { \
        return _element->get_##type(); \
    } else if (_mode == mode::variant) { \
        return _b_##type; \
    } else { \
        throw std::runtime_error("unset element"); \
    } \
}
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM

value::operator bool() const {
    return _mode != mode::unset;
}

bool operator==(const value& lhs, const value& rhs) {
    if (lhs._mode == value::mode::unset && rhs._mode == value::mode::unset) {
        return true;
    } else if (lhs._mode == value::mode::unset || rhs._mode == value::mode::unset) {
        return false;
    } else {
        // TODO we really should optimize this
        bsoncxx::builder::stream::array lhs_builder;
        bsoncxx::builder::stream::array rhs_builder;

        switch (static_cast<int>(lhs.type())) {
#define BSONCXX_ENUM(type, val) \
            case val: \
                lhs_builder << lhs.get_##type(); \
                break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
        };

        switch (static_cast<int>(rhs.type())) {
#define BSONCXX_ENUM(type, val) \
            case val: \
                rhs_builder << rhs.get_##type(); \
                break;
#include <bsoncxx/enums/type.hpp>
#undef BSONCXX_ENUM
        };

        auto lhs_view = lhs_builder.view();
        auto rhs_view = rhs_builder.view();

        if (lhs_view.length() == rhs_view.length()) {
            return std::memcmp(lhs_view.data(), rhs_view.data(), lhs_view.length()) == 0;
        } else {
            return false;
        }
    }
}

bool operator!=(const value& lhs, const value& rhs) {
    return !(lhs == rhs);
}


}  // namespace types
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
