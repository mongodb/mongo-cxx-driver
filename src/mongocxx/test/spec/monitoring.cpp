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
#include <bsoncxx/types/bson_value/value.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/test/spec/monitoring.hh>
#include <mongocxx/test_util/client_helpers.hh>
#include <third_party/catch/include/catch.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace spec {

using namespace mongocxx;
using bsoncxx::to_json;

void apm_checker::compare_v2(bsoncxx::array::view expectations, bool allow_extra) {
    using bsoncxx::types::bson_value::value;

    auto is_ignored = [&](bsoncxx::document::value v) {
        return std::any_of(std::begin(_ignore), std::end(_ignore), [&](stdx::string_view key) {
            return v.view()["commandStartedEvent"]["command"][key] ||
                   v.view()["commandFailedEvent"]["command"][key] ||
                   v.view()["commandSucceededEvent"]["command"][key];
        });
    };

    auto events_iter = _events.begin();
    _events.erase(std::remove_if(_events.begin(), _events.end(), is_ignored), std::end(_events));
    CAPTURE(print_all());
    for (auto expectation : expectations) {
        auto expected = expectation.get_document().view();
        REQUIRE(events_iter != _events.end());
        test_util::assert_matches(value(events_iter->view()), value(expected));
        events_iter++;
    }

    if (!allow_extra && events_iter != _events.end()) {
        FAIL("extra event found '" + to_json(*events_iter) + "'");
    }
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
    for (auto expectation : expectations) {
        auto expected = expectation.get_document().view();
        REQUIRE(events_iter != _events.end());
        CAPTURE(print_all());
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

// commands postfixed with "_v2" are used to support the unified test format.
void apm_checker::set_command_started_v2(options::apm& apm) {
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

void apm_checker::set_command_failed_v2(options::apm& apm) {
    using namespace bsoncxx::builder::basic;

    apm.on_command_failed([&](const events::command_failed_event& event) {
        document builder;
        builder.append(
            kvp("commandFailedEvent", make_document(kvp("commandName", event.command_name()))));
        this->_events.emplace_back(builder.extract());
    });
}

void apm_checker::set_command_succeeded_v2(options::apm& apm) {
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

apm_checker::event apm_checker::to_event(stdx::string_view s) {
    if (s.to_string() == "killCursors")
        return apm_checker::event::kill_cursors;
    if (s.to_string() == "getMore")
        return apm_checker::event::get_more;
    if (s.to_string() == "configureFailPoint")
        return apm_checker::event::configure_fail_point;
    throw mongocxx::logic_error{error_code::k_invalid_parameter,
                                "unrecognized event '" + s.to_string() + "'"};
}

std::string apm_checker::to_string(event e) {
    switch (e) {
        case apm_checker::event::kill_cursors:
            return "killCursors";
        case apm_checker::event::get_more:
            return "getMore";
        case apm_checker::event::configure_fail_point:
            return "configureFailPoint";
    }
}

void apm_checker::set_ignore_command_monitoring_event(event e) {
    this->_ignore.push_back(to_string(e));
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
