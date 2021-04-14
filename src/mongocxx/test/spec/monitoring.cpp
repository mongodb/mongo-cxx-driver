// Copyright 2018-present MongoDB Inc.
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

#include <mongocxx/config/private/prelude.hh>

#include <iostream>
#include <sstream>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/test/spec/monitoring.hh>
#include <mongocxx/test/spec/unified_tests/assert.hh>
#include <mongocxx/test_util/client_helpers.hh>
#include <third_party/catch/include/catch.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace spec {

using namespace mongocxx;
using bsoncxx::to_json;

void remove_ignored_command_monitoring_events(apm_checker::event_vector& events,
                                              const std::vector<std::string>& ignore) {
    auto is_ignored = [&](bsoncxx::document::value v) {
        return std::any_of(std::begin(ignore), std::end(ignore), [&](stdx::string_view key) {
            return v.view()["commandStartedEvent"]["command"][key] ||
                   v.view()["commandFailedEvent"]["command"][key] ||
                   v.view()["commandSucceededEvent"]["command"][key];
        });
    };

    events.erase(std::remove_if(events.begin(), events.end(), is_ignored), std::end(events));
}

// commands postfixed with "_unified" are used to support the unified test format.
void apm_checker::compare_unified(bsoncxx::array::view expectations, entity::map& map) {
    remove_ignored_command_monitoring_events(_events, _ignore);

    auto equal = [&](const bsoncxx::array::element& exp, const bsoncxx::document::view actual) {
        CAPTURE(print_all(), to_json(actual), assert::to_string(exp.get_value()));

        // Extra fields are only allowed in root-level documents. Here, each k in keys is treated
        // as its own root-level document, allowing extra fields.
        auto match_events = [&](stdx::string_view event, std::initializer_list<std::string> keys) {
            for (auto&& k : keys)
                if (exp[event][k])
                    assert::matches(actual[event][k].get_value(), exp[event][k].get_value(), map);
        };

        match_events("commandStartedEvent", {"command", "commandName", "databaseName"});
        match_events("commandSucceededEvent", {"reply", "commandName"});
        match_events("commandFailedEvent", {"commandName"});
        return true;
    };

    // This will throw an exception on unmatched fields and return true in all other cases.
    std::equal(expectations.begin(), expectations.end(), _events.begin(), equal);
}

void apm_checker::compare(bsoncxx::array::view expectations,
                          bool allow_extra,
                          const test_util::match_visitor& match_visitor) {
    auto is_ignored = [&](bsoncxx::document::value v) {
        return std::any_of(std::begin(_ignore), std::end(_ignore), [&](stdx::string_view key) {
            return v.view()["command_started_event"]["command"][key] ||
                   v.view()["command_failed_event"]["command"][key] ||
                   v.view()["command_succeeded_event"]["command"][key];
        });
    };

    auto events_iter = _events.begin();
    _events.erase(std::remove_if(_events.begin(), _events.end(), is_ignored), std::end(_events));
    CAPTURE(print_all());
    for (auto expectation : expectations) {
        auto expected = expectation.get_document().view();
        REQUIRE(events_iter != _events.end());
        REQUIRE_BSON_MATCHES_V(*events_iter, expected, match_visitor);
        events_iter++;
    }

    if (!allow_extra)
        REQUIRE(events_iter == _events.end());
}

void apm_checker::has(bsoncxx::array::view expectations) {
    for (auto expectation : expectations) {
        auto expected = expectation.get_document().view();
        CAPTURE(to_json(expected).c_str(), print_all());
        REQUIRE(std::find_if(_events.begin(), _events.end(), [&](bsoncxx::document::view doc) {
                    return test_util::matches(doc, expected);
                }) != _events.end());
    }
}

std::string apm_checker::print_all() {
    std::stringstream output{};
    output << std::endl << std::endl;
    output << "APM Checker contents: " << std::endl;
    for (auto&& event : _events) {
        output << "APM event: " << bsoncxx::to_json(event) << std::endl;
    }
    output << std::endl << std::endl;
    return output.str();
}

void apm_checker::set_command_started_unified(options::apm& apm) {
    using namespace bsoncxx::builder::basic;

    apm.on_command_started([&](const events::command_started_event& event) {
        document builder;
        builder.append(kvp("commandStartedEvent",
                           make_document(kvp("command", event.command()),
                                         kvp("commandName", event.command_name()),
                                         kvp("databaseName", event.database_name()))));
        this->_events.emplace_back(builder.extract());
    });
}

void apm_checker::set_command_failed_unified(options::apm& apm) {
    using namespace bsoncxx::builder::basic;

    apm.on_command_failed([&](const events::command_failed_event& event) {
        document builder;
        builder.append(
            kvp("commandFailedEvent", make_document(kvp("commandName", event.command_name()))));
        this->_events.emplace_back(builder.extract());
    });
}

void apm_checker::set_command_succeeded_unified(options::apm& apm) {
    using namespace bsoncxx::builder::basic;

    apm.on_command_succeeded([&](const events::command_succeeded_event& event) {
        document builder;
        builder.append(kvp(
            "commandSucceededEvent",
            make_document(kvp("reply", event.reply()), kvp("commandName", event.command_name()))));
        this->_events.emplace_back(builder.extract());
    });
}

void apm_checker::set_command_started(options::apm& apm) {
    using namespace bsoncxx::builder::basic;

    apm.on_command_started([&](const events::command_started_event& event) {
        document builder;
        builder.append(kvp("command_started_event",
                           make_document(kvp("command", event.command()),
                                         kvp("command_name", event.command_name()),
                                         kvp("operation_id", event.operation_id()),
                                         kvp("database_name", event.database_name()))));
        this->_events.emplace_back(builder.extract());
    });
}

void apm_checker::set_command_failed(options::apm& apm) {
    using namespace bsoncxx::builder::basic;

    apm.on_command_failed([&](const events::command_failed_event& event) {
        document builder;
        builder.append(kvp("command_failed_event",
                           make_document(kvp("command_name", event.command_name()),
                                         kvp("operation_id", event.operation_id()))));
        this->_events.emplace_back(builder.extract());
    });
}

void apm_checker::set_command_succeeded(options::apm& apm) {
    using namespace bsoncxx::builder::basic;

    apm.on_command_succeeded([&](const events::command_succeeded_event& event) {
        document builder;
        builder.append(kvp("command_succeeded_event",
                           make_document(kvp("reply", event.reply()),
                                         kvp("command_name", event.command_name()),
                                         kvp("operation_id", event.operation_id()))));
        this->_events.emplace_back(builder.extract());
    });
}

options::apm apm_checker::get_apm_opts(bool command_started_events_only) {
    options::apm opts;

    this->set_command_started(opts);
    if (command_started_events_only)
        return opts;

    this->set_command_failed(opts);
    this->set_command_succeeded(opts);
    return opts;
}

void apm_checker::set_ignore_command_monitoring_event(const std::string& event) {
    auto valid_events = {"killCursors", "getMore", "configureFailPoint"};
    REQUIRE(std::find(valid_events.begin(), valid_events.end(), event) != valid_events.end());

    this->_ignore.push_back(event);
}

void apm_checker::clear_events() {
    this->_events.clear();
}

void apm_checker::clear() {
    clear_events();
    this->_ignore.clear();
}

}  // namespace spec
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
#include <mongocxx/config/private/postlude.hh>
