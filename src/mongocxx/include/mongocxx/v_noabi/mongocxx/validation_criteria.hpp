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

#include <mongocxx/validation_criteria-fwd.hpp>

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// Class representing criteria for document validation, to be applied to a collection.
///
/// @see
/// - https://www.mongodb.com/docs/manual/core/document-validation/
///
class validation_criteria {
   public:
    ///
    /// Sets a validation rule for this validation object.
    ///
    /// @param rule
    ///   Document representing a validation rule.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(validation_criteria&)
    rule(bsoncxx::v_noabi::document::view_or_value rule);

    ///
    /// Gets the validation rule for this validation object.
    ///
    /// @return
    ///   Document representing a validation rule.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(
        const bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value>&)
    rule() const;

    ///
    /// A class to represent the different validation level options.
    ///
    enum class validation_level {
        /// Disable validation entirely.
        k_off,

        /// Apply validation rules to inserts, and apply validation rules to updates only if the
        /// document to be updated already fulfills the validation criteria.
        k_moderate,

        /// Apply validation rules to all inserts and updates.
        k_strict,
    };

    ///
    /// Sets a validation level.
    ///
    /// @param level
    ///   An enumerated validation level.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(validation_criteria&) level(validation_level level);

    ///
    /// Gets the validation level.
    ///
    /// @return
    ///   The enumerated validation level.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<validation_level>&)
    level() const;

    ///
    /// A class to represent the different validation action options.
    ///
    enum class validation_action {
        /// Reject any insertion or update that violates the validation criteria.
        k_error,

        /// Log any violations of the validation criteria, but allow the insertion or update to
        /// proceed.
        k_warn,
    };

    ///
    /// Sets a validation action to run when documents failing validation are inserted or modified.
    ///
    /// @param action
    ///   An enumerated validation action.
    ///
    /// @return
    ///   A reference to the object on which this member function is being called.  This facilitates
    ///   method chaining.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(validation_criteria&) action(validation_action action);

    ///
    /// Gets the validation action to run when documents failing validation are inserted or
    /// modified.
    ///
    /// @return
    ///   The enumerated validation action.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(const bsoncxx::v_noabi::stdx::optional<validation_action>&)
    action() const;

    ///
    /// Returns a bson document representing this set of validation criteria.
    ///
    /// @deprecated
    ///   This method is deprecated. To determine which options are set on this object, use the
    ///   provided accessors instead.
    ///
    /// @return Validation criteria, as a document.
    ///
    MONGOCXX_DEPRECATED MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::value)
        to_document() const;

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::value) to_document_deprecated() const;

    ///
    /// @deprecated
    ///   This method is deprecated. To determine which options are set on this object, use the
    ///   provided accessors instead.
    ///
    MONGOCXX_DEPRECATED operator bsoncxx::v_noabi::document::value() const {
        return to_document_deprecated();
    }

   private:
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _rule;
    bsoncxx::v_noabi::stdx::optional<validation_level> _level;
    bsoncxx::v_noabi::stdx::optional<validation_action> _action;
};

///
/// Compare equal when the arguments' rule, level, and action compare equal; otherwise, compare
/// false.
///
/// @{

/// @relatesalso mongocxx::v_noabi::validation_criteria
MONGOCXX_ABI_EXPORT_CDECL(bool)
operator==(const validation_criteria& lhs, const validation_criteria& rhs);

/// @relatesalso mongocxx::v_noabi::validation_criteria
MONGOCXX_ABI_EXPORT_CDECL(bool)
operator!=(const validation_criteria& lhs, const validation_criteria& rhs);
/// @}
///

}  // namespace v_noabi
}  // namespace mongocxx

namespace mongocxx {

using ::mongocxx::v_noabi::operator==;
using ::mongocxx::v_noabi::operator!=;

}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::validation_criteria.
///

#if defined(MONGOCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

namespace mongocxx {

/// @ref mongocxx::v_noabi::operator==(const v_noabi::validation_criteria& lhs, const v_noabi::validation_criteria& rhs)
bool operator==(const v_noabi::validation_criteria& lhs, const v_noabi::validation_criteria& rhs);

/// @ref mongocxx::v_noabi::operator!=(const v_noabi::validation_criteria& lhs, const v_noabi::validation_criteria& rhs)
bool operator!=(const v_noabi::validation_criteria& lhs, const v_noabi::validation_criteria& rhs);

}  // namespace mongocxx

#endif  // defined(MONGOCXX_PRIVATE_DOXYGEN_PREPROCESSOR)
