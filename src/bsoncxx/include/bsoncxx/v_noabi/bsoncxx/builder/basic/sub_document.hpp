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

#include <bsoncxx/builder/basic/sub_document-fwd.hpp>

#include <bsoncxx/builder/basic/helpers.hpp>
#include <bsoncxx/builder/concatenate.hpp>
#include <bsoncxx/builder/core.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/stdx/type_traits.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace builder {
namespace basic {

namespace impl {

template <typename T>
void value_append(core* core, T&& t);

}  // namespace impl

///
/// An internal class of builder::basic.
/// Users should almost always construct a builder::basic::document instead.
///
class sub_document {
   public:
    sub_document(core* core) : _core(core) {}

    ///
    /// Appends multiple basic::kvp key-value pairs.
    ///
    template <typename Arg, typename... Args>
    void append(Arg&& a, Args&&... args) {
        append_(std::forward<Arg>(a));
        append(std::forward<Args>(args)...);
    }

    ///
    /// Inductive base-case for the variadic append(...)
    ///
    void append() {}

   private:
    //
    // Appends a basic::kvp where the key is a non-owning string view.
    //
    template <typename K, typename V>
    detail::requires_t<void, detail::is_alike<K, stdx::string_view>>  //
    append_(std::tuple<K, V>&& t) {
        _core->key_view(std::forward<K>(std::get<0>(t)));
        impl::value_append(_core, std::forward<V>(std::get<1>(t)));
    }

    //
    // Appends a basic::kvp where the key is an owning STL string.
    //
    template <typename K, typename V>
    detail::requires_t<void, detail::is_alike<K, std::string>>  //
    append_(std::tuple<K, V>&& t) {
        _core->key_owned(std::forward<K>(std::get<0>(t)));
        impl::value_append(_core, std::forward<V>(std::get<1>(t)));
    }

    //
    // Appends a basic::kvp where the key is a string literal
    //
    template <std::size_t n, typename V>
    void append_(std::tuple<const char (&)[n], V>&& t) {
        _core->key_view(stdx::string_view{std::get<0>(t), n - 1});
        impl::value_append(_core, std::forward<V>(std::get<1>(t)));
    }

    //
    // Concatenates another bson document directly.
    //
    void append_(concatenate_doc doc) {
        _core->concatenate(doc);
    }

    core* _core;
};

}  // namespace basic
}  // namespace builder
}  // namespace v_noabi
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::builder::basic::sub_document.
///
