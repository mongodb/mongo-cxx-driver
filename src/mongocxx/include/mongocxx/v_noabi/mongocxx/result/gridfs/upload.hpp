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

#include <mongocxx/result/gridfs/upload-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/gridfs/upload_result.hpp> // IWYU pragma: export

#include <bsoncxx/array/value.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/types/value.hpp>
#include <bsoncxx/types/view.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {
namespace gridfs {

/// The result of a GridFS upload operation.
class upload {
   public:
    ~upload() = default;

    upload(upload&& other) noexcept = default;
    upload& operator=(upload&& other) noexcept = default;

    upload(upload const& other) : _id_owner{other._id_owner}, _id{_id_owner} {}

    upload& operator=(upload const& other) {
        if (this != &other) {
            _id_owner = other._id_owner;
            _id = _id_owner;
        }

        return *this;
    }

    ///
    /// @deprecated For internal use only.
    ///
    upload(bsoncxx::v_noabi::types::bson_value::view id) : _id_owner{id}, _id{_id_owner} {}

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() upload(v1::gridfs::upload_result opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @note The `comment` field is initialized with `this->comment_option()` (BSON type value) when set; otherwise, by
    /// `this->comment()` (`std::string`) when set; otherwise, it is unset.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() operator v1::gridfs::upload_result() const;

    ///
    /// Gets the id of the uploaded GridFS file.
    ///
    /// @return The id of the uploaded file.
    ///
    bsoncxx::v_noabi::types::view const& id() const {
        return _id;
    }

    friend bool operator==(upload const& lhs, upload const& rhs) {
        return lhs._id == rhs._id;
    }

    friend bool operator!=(upload const& lhs, upload const& rhs) {
        return !(lhs == rhs);
    }

   private:
    bsoncxx::v_noabi::types::value _id_owner;
    bsoncxx::v_noabi::types::view _id;
};

} // namespace gridfs
} // namespace result
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::result::gridfs::upload.
///
/// @par Includes
/// - @ref mongocxx/v1/gridfs/upload_result.hpp
///
