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

#include <mongocxx/v1/hint.hh>

#include <string>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

class hint::impl {
   public:
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _doc;
    bsoncxx::v1::stdx::optional<std::string> _str;

    static impl const& with(hint const& other) {
        return *static_cast<impl const*>(other._impl);
    }

    static impl const* with(hint const* other) {
        return static_cast<impl const*>(other->_impl);
    }

    static impl& with(hint& other) {
        return *static_cast<impl*>(other._impl);
    }

    static impl* with(hint* other) {
        return static_cast<impl*>(other->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

hint::~hint() {
    delete impl::with(_impl);
}

hint::hint(hint&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

hint& hint::operator=(hint&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

hint::hint(hint const& other) : _impl{new impl{impl::with(other)}} {}

hint& hint::operator=(hint const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

hint::hint() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

hint::hint(std::string str) : hint{} {
    impl::with(this)->_str = std::move(str);
}

hint::hint(bsoncxx::v1::document::value doc) : hint{} {
    impl::with(this)->_doc = std::move(doc);
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> hint::str() const {
    return impl::with(this)->_str;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> hint::doc() const {
    return impl::with(this)->_doc;
}

bsoncxx::v1::types::view hint::to_value() const {
    if (auto const& opt = impl::with(this)->_doc) {
        return bsoncxx::v1::types::b_document{*opt};
    }

    // Invariant: either `_doc` or `_str` has a value.
    return bsoncxx::v1::types::b_string{*impl::with(this)->_str};
}

hint::operator bsoncxx::v1::types::view() const {
    return this->to_value();
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& hint::internal::doc(hint& self) {
    return impl::with(self)._doc;
}

bsoncxx::v1::stdx::optional<std::string>& hint::internal::str(hint& self) {
    return impl::with(self)._str;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& hint::internal::doc(hint const& self) {
    return impl::with(self)._doc;
}

bsoncxx::v1::stdx::optional<std::string> const& hint::internal::str(hint const& self) {
    return impl::with(self)._str;
}

} // namespace v1
} // namespace mongocxx
