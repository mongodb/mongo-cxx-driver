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

#include <bsoncxx/json.hpp>
#include <iostream>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/test/spec/monitoring.hh>
#include <mongocxx/test_util/client_helpers.hh>
#include <third_party/catch/include/catch.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace spec {

using namespace mongocxx;

void apm_checker::compare(bsoncxx::array::view expectations,
                          bool allow_extra,
                          const test_util::match_visitor& match_visitor) {
    using bsoncxx::to_json;

    auto events_iter = _events.begin();
    for (auto expectation : expectations) {
        if (events_iter == _events.end()) {
            fprintf(stderr,
                    "more expectations than events captured\nExpectations:\n\n%s\n\nCaptured:\n\n",
                    to_json(expectations).c_str());
            this->print_all();
            REQUIRE(false);
        }
        REQUIRE(events_iter != _events.end());

        auto expected = expectation.get_document().view();
        REQUIRE_BSON_MATCHES_V(*events_iter, expected, match_visitor);
        events_iter++;
    }
    if (!allow_extra) {
        if (_skip_kill_cursors && events_iter != _events.end()) {
            if (events_iter->view()["command_started_event"]["command"]["killCursors"]) {
                ++events_iter;
            }
        }

        if (events_iter == _events.end()) {
            return;
        }

        if (events_iter != _events.end()) {
            fprintf(stderr, "Error: extra event captured: %s\n", to_json(*events_iter).c_str());
            events_iter++;
        }

        REQUIRE(false);
    }
}

void apm_checker::print_all() {
    printf("\n\n");
    for (auto&& event : _events) {
        printf("APM event: %s\n", bsoncxx::to_json(event).c_str());
    }
    printf("\n\n");
}

options::apm apm_checker::get_apm_opts(bool command_started_events_only) {
    options::apm opts;
    using namespace bsoncxx::builder::basic;

    opts.on_command_started([&](const events::command_started_event& event) {
        bsoncxx::builder::basic::document builder;
        builder.append(kvp("command_started_event",
                           make_document(kvp("command", event.command()),
                                         kvp("command_name", event.command_name()),
                                         kvp("operation_id", event.operation_id()),
                                         kvp("database_name", event.database_name()))));
        this->_events.emplace_back(builder.extract());
    });

    if (!command_started_events_only) {
        opts.on_command_failed([&](const events::command_failed_event& event) {
            bsoncxx::builder::basic::document builder;
            builder.append(kvp("command_failed_event",
                               make_document(kvp("command_name", event.command_name()),
                                             kvp("operation_id", event.operation_id()))));
            this->_events.emplace_back(builder.extract());
        });
        opts.on_command_succeeded([&](const events::command_succeeded_event& event) {
            bsoncxx::builder::basic::document builder;
            builder.append(kvp("command_suceeded_event",
                               make_document(kvp("reply", event.reply()),
                                             kvp("command_name", event.command_name()),
                                             kvp("operation_id", event.operation_id()))));
            this->_events.emplace_back(builder.extract());
        });
    }
    return opts;
}

void apm_checker::clear() {
    this->_events.clear();
}

void apm_checker::skip_kill_cursors() {
    this->_skip_kill_cursors = true;
}

}  // namespace spec
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
#include <mongocxx/config/private/postlude.hh>
