// Copyright 2017 MongoDB Inc.
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

#include <unordered_set>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/exception/private/mongoc_error.hh>
#include <mongocxx/exception/query_exception.hpp>
#include <mongocxx/private/collection.hh>
#include <mongocxx/private/cursor.hh>
#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

using bsoncxx::builder::basic::concatenate;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

class change_stream::impl {
   public:
    impl(const collection& coll,
         const pipeline& pipe,
         const read_preference::impl& read_pref,
         options::change_stream options)
        : cursor_ptr{},
          _coll{coll},
          _read_pref{read_pref},
          _opts_doc{make_document()},
          _pipe_doc{pipe.view_array()},
          _resume_after_doc{make_document()},
          _max_await_time{options.max_await_time()},
          _cursor_it{nullptr} {
        bsoncxx::builder::basic::document opts_doc;

        if (auto options_fd = options.full_document()) {
            _full_document = {std::string(options_fd->terminated().data())};
        }

        if (auto options_ra = options.resume_after()) {
            _resume_after_doc = bsoncxx::document::value{*options_ra};
        }

        if (auto options_bs = options.batch_size()) {
            opts_doc.append(kvp("batchSize", *options_bs));
        }

        if (auto options_coll = options.collation()) {
            opts_doc.append(kvp("collation", options_coll->view()));
        }

        _opts_doc = bsoncxx::document::value{opts_doc.view()};
    }

    // Gets most recent / current change notification
    const bsoncxx::document::view& get() {
        return *_cursor_it;
    }

    // Fetches more data. Retries on certain errors. Throws exception if retry fails.
    void get_more() {
        // Retry once
        try {
            poll_cursor();
            return;
        } catch (const query_exception& e) {
            if (is_resumable(e)) {
                update_cursor();
            } else {
                throw e;
            }
        }

        // Failure here gets through
        poll_cursor();
    }

    bool is_exhausted() {
        return _cursor_it.is_exhausted();
    }

    void mark_started() {
        update_cursor();
    }

    bool has_started() {
        return !!cursor_ptr;
    }

    ~impl() {}

    std::unique_ptr<cursor> cursor_ptr;

   private:
    // Increments underlying cursor. Throws logic_error if _id in notification projected out.
    // Throws query_exception if cursor / server has issues.
    void poll_cursor() {
        if (_cursor_it == cursor_ptr->end()) {
            _cursor_it = cursor_ptr->begin();
        } else {
            _cursor_it++;
        }

        if (!_cursor_it.is_exhausted()) {
            if (auto resume_after = get()["_id"]) {
                _resume_after_doc = bsoncxx::document::value{resume_after.get_document().view()};
            } else {
                throw mongocxx::logic_error{
                    error_code::k_invalid_parameter,
                    "Cannot provide resume functionality when the resume token is missing."};
            }
        }
    }

    // From spec, a resumable error is "any error encountered which is not a server error, with the
    // exception of a 'not master' server response."
    bool is_resumable(const query_exception& e) {
        if (e.code().category() == server_error_category()) {
            return e.code() == server_error_code::k_not_master ||
                   e.code() == server_error_code::k_not_master_no_slave_ok;
        }

        return true;
    }

    // Performs aggregation, using resume token if possible.
    void update_cursor() {
        bsoncxx::builder::basic::array pipe_arr_builder;
        bsoncxx::builder::basic::document change_doc_builder;
        bsoncxx::builder::basic::document opts_builder;

        if (has_started() && !_resume_after_doc.view().empty()) {
            change_doc_builder.append(kvp("resumeAfter", _resume_after_doc.view()));
        }

        if (_full_document) {
            change_doc_builder.append(kvp("fullDocument", *_full_document));
        }

        pipe_arr_builder.append(make_document(kvp("$changeStream", change_doc_builder.view())));
        pipe_arr_builder.append(bsoncxx::builder::basic::concatenate(_pipe_doc.view()));

        bsoncxx::document::view pipe_arr_view{pipe_arr_builder.view()};

        // TODO: remove this manual selection and just aggregate (waiting on CDRIVER-2237)
        bson_error_t selection_error;
        mongoc_server_description_t* server =
            libmongoc::client_select_server(_coll._get_impl().client_impl->client_t,
                                            false,
                                            _read_pref.read_preference_t,
                                            &selection_error);
        if (!server) {
            throw_exception<operation_exception>(selection_error);
        }

        opts_builder.append(
            kvp("serverId", static_cast<std::int64_t>(libmongoc::server_description_id(server))));
        opts_builder.append(concatenate(_opts_doc.view()));

        libbson::scoped_bson_t pipe_bson{pipe_arr_view};
        libbson::scoped_bson_t opts_bson{opts_builder.view()};

        mongoc_cursor_t* curs = libmongoc::collection_aggregate(
            _coll._get_impl().collection_t,
            static_cast<::mongoc_query_flags_t>(MONGOC_QUERY_TAILABLE_CURSOR |
                                                MONGOC_QUERY_AWAIT_DATA),
            pipe_bson.bson(),
            opts_bson.bson(),
            _read_pref.read_preference_t);

        bson_error_t error;
        if (libmongoc::cursor_error(curs, &error)) {
            throw_exception<query_exception>(error);
        }

        if (_max_await_time) {
            libmongoc::cursor_set_max_await_time_ms(
                curs, static_cast<std::uint32_t>(_max_await_time->count()));
        }

        cursor_ptr = std::unique_ptr<cursor>(new cursor{curs, cursor::type::k_tailable_await});

        _cursor_it = cursor_ptr->begin();
    }

    const collection& _coll;
    const read_preference::impl _read_pref;

    // Need to own everything in case change_stream lives longer than objects used to create it.
    bsoncxx::document::value _opts_doc;
    bsoncxx::array::value _pipe_doc;
    bsoncxx::document::value _resume_after_doc;

    stdx::optional<std::chrono::milliseconds> _max_await_time;
    stdx::optional<std::string> _full_document;

    cursor::iterator _cursor_it;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>