// Copyright 2015 MongoDB Inc.
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

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/stdx.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

///
/// Class representing criteria for document validation, to be applied to a collection.
///
/// @see https://docs.mongodb.org/manual/core/document-validation/
///
class MONGOCXX_API validation_criteria {
   public:
    ///
    /// Set a validation rule for this validation object.
    ///
    /// @param rule
    ///   Document representing a validation rule.
    ///
    void rule(bsoncxx::document::view_or_value rule);

    ///
    /// A class to represent the different validation level options.
    ///
    enum class validation_level {
        k_off,
        k_moderate,
        k_strict,
    };

    ///
    /// Set a validation level.
    ///
    /// @param level
    ///   A validation level, "off," "strict," or "moderate."
    ///
    void level(validation_level level);

    ///
    /// A class to represent the different validation action options.
    ///
    /// @todo document enum values
    ///
    enum class validation_action {
        k_error,
        k_warn,
    };

    ///
    /// Set a validation action to run when documents failing validation
    /// are inserted or modified.
    ///
    /// @param action
    ///   A validation action, either "error" or "warn."
    ///
    void action(validation_action action);

    ///
    /// Return a bson document representing this set of validation criteria.
    ///
    /// @return Validation criteria, as a document.
    ///
    bsoncxx::document::value to_document() const;

    ///
    /// @todo document this method
    ///
    MONGOCXX_INLINE operator bsoncxx::document::value() const;

   private:
    stdx::optional<bsoncxx::document::view_or_value> _rule;
    stdx::optional<validation_level> _level;
    stdx::optional<validation_action> _action;
};

MONGOCXX_INLINE validation_criteria::operator bsoncxx::document::value() const {
    return to_document();
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
