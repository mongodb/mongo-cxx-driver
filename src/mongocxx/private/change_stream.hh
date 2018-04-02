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

#pragma once

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/change_stream.hpp>
#include <mongocxx/private/libmongoc.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class change_stream::impl {
    // no copy or move
    impl(impl&) = delete;
    impl(impl&&) = delete;
    impl& operator=(const impl&) = delete;

   public:
    // lifecycle of the cursor
    // k_started means that libmongoc::change_stream_next has been called at least once.
    // k_pending means it hasn't
    // k_dead means that an error was indicated by a call to next
    enum class state { k_pending = 0, k_started = 1, k_dead = 2 };

    explicit impl(mongoc_change_stream_t* change_stream)
        : change_stream_{change_stream}, status_{state::k_pending}, exhausted_{true} {}

    ~impl() {
        libmongoc::change_stream_destroy(this->change_stream_);
    }

    inline bool has_started() const {
        return status_ >= state::k_started;
    }

    inline bool is_dead() const {
        return status_ == state::k_dead;
    }

    inline bool is_exhausted() const {
        return exhausted_;
    }

    inline void mark_dead() {
        mark_nothing_left();
        status_ = state::k_dead;
    }

    inline void mark_nothing_left() {
        doc_ = bsoncxx::document::view{};
        exhausted_ = true;
        status_ = state::k_pending;
    }

    inline void mark_started() {
        status_ = state::k_started;
        exhausted_ = false;
    }

    void advance_iterator() {
        const bson_t* out;
        if (libmongoc::change_stream_next(this->change_stream_, &out)) {
            this->doc_ = bsoncxx::document::view{bson_get_data(out), out->len};
        } else {
            // Separate if/else branch to avoid stack-allocating bson_error_t in happy case
            // change_stream_next succeeding.
            bson_error_t error;
            if (libmongoc::change_stream_error_document(this->change_stream_, &error, &out)) {
                this->mark_dead();
                this->doc_ = bsoncxx::document::view{};
                throw_exception<query_exception>(error);
            } else {
                this->mark_nothing_left();
            }
        }
    }

    inline bsoncxx::document::view& doc() {
        return this->doc_;
    }

   private:
    mongoc_change_stream_t* const change_stream_;
    bsoncxx::document::view doc_;
    state status_;
    bool exhausted_;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
