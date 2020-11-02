// Copyright 2020 MongoDB Inc.
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

#include <bsoncxx/config/prelude.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/make_value.hpp>
#include <bsoncxx/types/bson_value/value.hpp>
#include <bsoncxx/types/bson_value/value.hpp>
#include <iostream>
#include <map>
#include <vector>

//#############################################################################
// BSON_REF
//#############################################################################

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {
using namespace bsoncxx::types;

class bson {
   public:
    template <typename T>
    bson(T value) : _value{value} {}

    bson(std::initializer_list<bson> init) {
        size_t counter = 0;
        bool is_document =
            std::all_of(begin(init),
                        end(init),
                        [&](const bson ele) {
                            return counter++ % 2 != 0 || ele._value.view().type() == type::k_utf8;
                        }) &&
            counter % 2 == 0;

        if (is_document) {
            core _core{false};

            counter = 0;
            std::for_each(begin(init), end(init), [&](const bson ele) {
                if (counter % 2 == 0) {
                    auto key = std::string(ele._value.view().get_string().value);
                    _core.key_owned(key);
                } else {
                    _core.append(ele._value);
                }
                counter++;
            });
            _value = bson_value::value(_core.extract_document());
        } else {
            std::cout << "not a document" << std::endl;
        }
        // if (is_object) {
        //     for (auto ele_ref : init) {
        //         auto ele = ele_ref.moved_or_copied();
        //         m_value.document->emplace(std::move(*((*ele.m_value.array)[0].m_value.string)),
        //                                   std::move((*ele.m_value.array)[1]));
        //     }
        //     std::cout << "created document:" << std::endl;
        //     for (auto elem : *m_value.document)
        //         std::cout << "{ " << elem.first << ", " << elem.second.m_value.int_32 << "}"
        //                   << std::endl;
        // } else {
        //     m_type = type::k_array;
        //     m_value.array = create<array_t>(begin(init), end(init));
        //     std::cout << "created array:" << std::endl << "[ ";
        //     for (auto elem : *m_value.array) {
        //         if (elem.m_type == type::k_utf8)
        //             std::cout << *elem.m_value.string << ", ";
        //         else
        //             std::cout << elem.m_value.int_32 << ", ";
        //     }
        //     std::cout << "] " << std::endl;
        // }
    }

    operator bson_value::value() {
        return _value;
    }

   private:
    bson_value::value _value{nullptr};
};
}
BSONCXX_INLINE_NAMESPACE_END
}
// template <typename BasicBsonType>
// class bson_ref {
// public:
// using value_type = BasicBsonType;
//
// bson_ref(std::initializer_list<bson_ref> init)
// : owned_value(init), value_ref(&owned_value), is_rvalue(true) {}
//
// template <class... Args>
// bson_ref(Args&&... args)
// : owned_value(std::forward<Args>(args)...), value_ref(&owned_value), is_rvalue(true) {}
//
// value_type moved_or_copied() const {
// if (is_rvalue)
// return std::move(*value_ref);
// return *value_ref;
// }
//
// value_type const& operator*() const {
// return *static_cast<value_type const*>(value_ref);
// }
//
// value_type const* operator->() const {
// return static_cast<value_type const*>(value_ref);
// }
//
// private:
// mutable value_type owned_value = nullptr;
// value_type* value_ref = nullptr;
// const bool is_rvalue = true;
// };
// }  // namespace builder
// BSONCXX_INLINE_NAMESPACE_END
// }  // namespace bsoncxx
//
// //#############################################################################
// // ADL_SERIALIZER
// //#############################################################################
// namespace bsoncxx {
// BSONCXX_INLINE_NAMESPACE_BEGIN
// namespace builder {
//
// struct adl_serializer {
// types::b_int32 operator()(std::int32_t val) {
// return types::b_int32{val};
// }
//
// types::b_utf8 operator()(std::string val) {
// return types::b_utf8{std::move(val)};
// }
// };
// }  // namespace builder
// BSONCXX_INLINE_NAMESPACE_END
// }  // namespace bsoncxx
//
// //#############################################################################
// // BASIC_BSON
// //#############################################################################
//
// namespace bsoncxx {
// BSONCXX_INLINE_NAMESPACE_BEGIN
// namespace builder {
//
// using bsoncxx::types::bson_value::make_value;
//
// class basic_bson {
// using value_type = basic_bson;
// using initializer_list_t = std::initializer_list<builder::bson_ref<basic_bson>>;
//
// public:
// template <typename T>
// basic_bson(bson_ref<T> ref) : basic_bson(ref.moved_or_copied()) {}
// basic_bson(const basic_bson& other) : m_type(other.m_type) {
// switch (m_type) {
// case type::k_array:
// m_value = *other.m_value.array;
// break;
// case type::k_document:
// m_value = *other.m_value.document;
// break;
// case type::k_utf8:
// m_value = *other.m_value.string;
// break;
// case type::k_int32:
// m_value = other.m_value.int_32;
// break;
// default:
// break;
// }
// }
//
// basic_bson(initializer_list_t init) {
// bool is_object = std::all_of(begin(init), end(init), [](const bson_ref<basic_bson>& b) {
// return b->is_array() && b->size() == 2 && (*b)[0].is_string();
// });
//
// m_type = type::k_document;
// m_value = type::k_document;
//
// if (is_object) {
// for (auto ele_ref : init) {
// auto ele = ele_ref.moved_or_copied();
// m_value.document->emplace(std::move(*((*ele.m_value.array)[0].m_value.string)),
// std::move((*ele.m_value.array)[1]));
// }
//
// std::cout << "created document:" << std::endl;
// for (auto elem : *m_value.document)
// std::cout << "{ " << elem.first << ", " << elem.second.m_value.int_32 << "}"
// << std::endl;
// } else {
// m_type = type::k_array;
// m_value.array = create<array_t>(begin(init), end(init));
//
// std::cout << "created array:" << std::endl << "[ ";
// for (auto elem : *m_value.array) {
// if (elem.m_type == type::k_utf8)
// std::cout << *elem.m_value.string << ", ";
// else
// std::cout << elem.m_value.int_32 << ", ";
// }
// std::cout << "] " << std::endl;
// }
// }
//
// basic_bson(std::nullptr_t = nullptr) noexcept : basic_bson(type::k_null) {}
//
// basic_bson(const type t) : m_type(t), m_value(t) {}
//
// basic_bson(std::string&& val) : m_value{val}, m_type{type::k_utf8} {
// std::cout << "created string" << std::endl;
// }
// basic_bson(std::int32_t&& val) : m_value{adl_serializer{}(val)}, m_type{type::k_int32} {
// std::cout << "created int" << std::endl;
// }
//
// basic_bson(basic_bson&& other) noexcept : m_type(std::move(other.m_type)),
// m_value(std::move(other.m_value)) {
// other.m_type = type::k_null;
// other.m_value = {};
// }
//
// value_type& operator=(basic_bson other) noexcept {
// std::swap(m_type, other.m_type);
// std::swap(m_value, other.m_value);
// return *this;
// }
//
// constexpr bool is_array() const noexcept {
// return m_type == type::k_array;
// }
//
// constexpr bool is_string() const noexcept {
// return m_type == type::k_utf8;
// }

//    const value_type& operator[](std::size_t idx) const {
//         return m_value.array->operator[](idx);
//     }
//
//     template <typename T>
//     value_type& operator[](T* key) {
//         if (is_null()) {
//             m_type = type::k_document;
//             m_value = type::k_document;
//         }
//
//         if (is_document()) {
//             return m_value.document->operator[](key);
//         }
//     }
//
//     constexpr bool is_document() const noexcept {
//         return m_type == type::k_document;
//     }
//
//     constexpr bool is_null() const noexcept {
//         return m_type == type::k_null;
//     }
//
//     std::size_t size() const noexcept {
//         switch (m_type) {
//             case type::k_array:
//                 return m_value.array->size();
//             case type::k_document:
//                 return m_value.document->size();
//             default:
//                 return 1;
//         }
//     }
//     template <typename T, typename... Args>
//     static T* create(Args&&... args) {
//         std::allocator<T> alloc;
//         using AllocatorTraits = std::allocator_traits<std::allocator<T>>;
//
//         auto deleter = [&](T* object) { AllocatorTraits::deallocate(alloc, object, 1); };
//         std::unique_ptr<T, decltype(deleter)> object(AllocatorTraits::allocate(alloc, 1),
//         deleter);
//         AllocatorTraits::construct(alloc, object.get(), std::forward<Args>(args)...);
//         return object.release();
//     }
//
//    private:
//     using object_t = std::map<std::string,
//                               basic_bson,
//                               std::less<std::string>,
//                               std::allocator<std::pair<const std::string, basic_bson>>>;
//
//     using array_t = std::vector<basic_bson, std::allocator<basic_bson>>;
//
//     //#############################################################################
//     // BSON_UNION
//     //#############################################################################
//     union bson_value {
//         std::string* string;
//         types::b_int32 int_32;
//         object_t* document;
//         array_t* array;
//
//         bson_value() = default;
//         bson_value(types::b_int32 v) noexcept : int_32{v} {}
//         bson_value(std::string v) {
//             string = create<std::string>(v);
//         }
//         bson_value(const object_t& v) {
//             document = create<object_t>(v);
//         }
//
//         bson_value(const array_t& v) {
//             array = create<array_t>(v);
//         }
//
//         bson_value(type t) {
//             switch (t) {
//                 case type::k_document: {
//                     document = create<object_t>();
//                 }
//             }
//         }
//     };
//
//     bson_value m_value{};
//     type m_type{};
// };
//
// using bson = basic_bson;
//
// }  // namespace builder
// BSONCXX_INLINE_NAMESPACE_END
// }  // namespace bsoncxx
