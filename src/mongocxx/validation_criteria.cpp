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

#include <mongocxx/validation_criteria.hpp>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>

#include <mongocxx/config/private/prelude.hpp>

namespace {

// Convert validation levels to strings.
std::string validation_level_to_string(mongocxx::validation_criteria::validation_level level) {
    switch (level) {
        case mongocxx::validation_criteria::validation_level::k_off:
            return "off";
        case mongocxx::validation_criteria::validation_level::k_moderate:
            return "moderate";
        case mongocxx::validation_criteria::validation_level::k_strict:
            return "strict";
    }

    MONGOCXX_UNREACHABLE;
}

// Convert validation actions to strings.
std::string validation_action_to_string(mongocxx::validation_criteria::validation_action action) {
    switch (action) {
        case mongocxx::validation_criteria::validation_action::k_warn:
            return "warn";
        case mongocxx::validation_criteria::validation_action::k_error:
            return "error";
    }

    MONGOCXX_UNREACHABLE;
}

}  // namespace

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

void validation_criteria::rule(bsoncxx::document::view_or_value rule) {
    _rule = std::move(rule);
}

void validation_criteria::level(validation_criteria::validation_level level) {
    _level = std::move(level);
}

void validation_criteria::action(validation_criteria::validation_action action) {
    _action = std::move(action);
}

bsoncxx::document::value validation_criteria::to_document() const {
    auto doc = bsoncxx::builder::stream::document{};

    if (_rule) {
        doc << "validator" << bsoncxx::types::b_document{*_rule};
    }

    if (_level) {
        doc << "validationLevel" << validation_level_to_string(*_level);
    }

    if (_action) {
        doc << "validationAction" << validation_action_to_string(*_action);
    }

    return doc.extract();
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
