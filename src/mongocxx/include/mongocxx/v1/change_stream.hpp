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

#include <mongocxx/v1/change_stream-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/value-fwd.hpp>

#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/value.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>

namespace mongocxx {
namespace v1 {

///
/// A MongoDB change stream.
///
/// @attention This feature is experimental! It is not ready for use!
///
/// @see
/// - [Change Streams (MongoDB Manual)](https://www.mongodb.com/docs/manual/changeStreams/)
///
class change_stream {
   private:
    class impl;
    void* _impl;

   public:
    class options;

    class iterator;

    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views and iterators.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~change_stream();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() change_stream(change_stream&& other) noexcept;

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(change_stream&) operator=(change_stream&& other) noexcept;

    ///
    /// This class is not copyable.
    ///
    change_stream(change_stream const&) = delete;

    ///
    /// This class is not copyable.
    ///
    change_stream& operator=(change_stream const&) = delete;

    ///
    /// Return an iterator associated with this change stream.
    ///
    /// @important All iterators associated with the same change stream object share the same state.
    ///
    /// This function advances the underlying cursor to obtain the next (first) event document.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(iterator) begin();

    ///
    /// Return an end iterator.
    ///
    /// @important The end iterator has no associated change stream.
    ///
    iterator end() const;

    ///
    /// Return the resume token for this change stream.
    ///
    /// @returns Empty document when not available.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) get_resume_token() const;
};

///
/// Options for @ref mongocxx::v1::change_stream.
///
/// Supported fields include:
/// - batch_size ("batchSize")
/// - collation
/// - comment
/// - full_document ("fullDocument")
/// - full_document_before_change ("fullDocumentBeforeChange")
/// - max_await_time ("maxAwaitTimeMS")
/// - resume_after ("resumeAfter")
/// - start_after ("startAfter")
/// - start_at_operation_time ("startAtOperationTime")
///
/// @attention This feature is experimental! It is not ready for use!
///
class change_stream::options {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options(options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) operator=(options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options(options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) operator=(options const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options();

    ///
    /// Set the "batchSize" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) batch_size(std::int32_t v);

    ///
    /// Return the current "batchSize" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int32_t>) batch_size() const;

    ///
    /// Set the "collation" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) collation(bsoncxx::v1::document::value v);

    ///
    /// Return the current "collation" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) collation() const;

    ///
    /// Set the "comment" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) comment(bsoncxx::v1::types::value v);

    ///
    /// Return the current "comment" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view>) comment() const;

    ///
    /// Set the "fullDocument" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) full_document(std::string v);

    ///
    /// Return the current "fullDocument" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) full_document() const;

    ///
    /// Set the "fullDocumentBeforeChange" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) full_document_before_change(std::string v);

    ///
    /// Return the current "fullDocumentBeforeChange" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) full_document_before_change()
        const;

    ///
    /// Set the "maxAwaitTimeMS" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) max_await_time(std::chrono::milliseconds v);

    ///
    /// Return the current "maxAwaitTimeMS" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::milliseconds>) max_await_time() const;

    ///
    /// Set the "resumeAfter" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) resume_after(bsoncxx::v1::document::value v);

    ///
    /// Return the current "resumeAfter" option.
    ///
    /// @returns Empty optional when the "resumeAfter" option is unset.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) resume_after() const;

    ///
    /// Set the "startAfter" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) start_after(bsoncxx::v1::document::value v);

    ///
    /// Return the current "startAfter" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) start_after() const;

    ///
    /// Set the "startAtOperationTime" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) start_at_operation_time(bsoncxx::v1::types::b_timestamp v);

    ///
    /// Return the current "startAtOperationTime" option.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::b_timestamp>) start_at_operation_time()
        const;
};

///
/// An iterator over change stream events.
///
/// @attention This feature is experimental! It is not ready for use!
///
/// @important All iterators associated with the same change stream object share the same state.
/// @important The end iterator has no associated change stream.
///
/// @note This iterator almost satisfies Cpp17ForwardIterator, but `std::iterator_traits<T>::reference` is defined as
/// `value_type`, similar to `std::vector<bool>::iterator` and `std::istreambuf_iterator<T>`. Therefore, this iterator
/// only fully satisfies Cpp17InputIterator.
///
/// @see
/// - [Change Streams (MongoDB Manual)](https://www.mongodb.com/docs/manual/changeStreams/)
///
class change_stream::iterator {
   private:
    void* _impl; // v1::change_stream

   public:
    ///
    /// Provide `std::iterator_traits<T>::iterator_category`.
    ///
    using iterator_category = std::input_iterator_tag;

    ///
    /// Provide `std::iterator_traits<T>::value_type`.
    ///
    using value_type = bsoncxx::v1::document::view;

    ///
    /// Provide `std::iterator_traits<T>::difference_type`.
    ///
    using difference_type = std::ptrdiff_t;

    ///
    /// Provide `std::iterator_traits<T>::pointer`.
    ///
    using pointer = value_type const*;

    ///
    /// Provide `std::iterator_traits<T>::reference`.
    ///
    using reference = value_type;

    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~iterator();

    ///
    /// Move construction.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() iterator(iterator&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(iterator&) operator=(iterator&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() iterator(iterator const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(iterator&) operator=(iterator const& other);

    ///
    /// Initialize as an end iterator.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() iterator();

    ///
    /// Return the current event document.
    ///
    /// @returns Empty when there is no event document available.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(value_type) operator*() const;

    ///
    /// Access the current event document.
    ///
    /// @par Preconditions:
    /// - `*this` does not compare equal to the end iterator.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pointer) operator->() const;

    ///
    /// Increment this iterator.
    ///
    /// Advance the underlying cursor to obtain the next event document.
    /// Compare equal to the end iterator when there are no event documents available.
    ///
    /// @note Pre-increment and post-increment are equivalent.
    ///
    /// @warning Invalidates all views to the current event document.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(iterator&) operator++();

    /// @copydoc operator++()
    iterator& operator++(int) {
        return this->operator++();
    }

    ///
    /// Equality comparison.
    ///
    /// `lhs` and `rhs` compare equal when:
    /// - both are associated with the same change stream object, or
    /// - both are end iterators, or
    /// - one is an end iterator and the other has no event document available.
    ///
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(iterator const& lhs, iterator const& rhs);

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(iterator const& lhs, iterator const& rhs) {
        return !(lhs == rhs);
    }
};

inline change_stream::iterator change_stream::end() const {
    return {};
}

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::change_stream.
///
