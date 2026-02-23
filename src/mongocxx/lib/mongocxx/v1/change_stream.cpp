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

#include <mongocxx/v1/change_stream.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>
#include <bsoncxx/v1/types/value.hpp>

#include <bsoncxx/v1/types/value.hh>

#include <mongocxx/v1/exception.hh>

#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

namespace {

enum class state {
    has_doc,      // The previous advancement returned a document.
    can_get_more, // No document is currently available, but can get more (end).
    is_dead,      // Cannot obtain any more documents (end).
};

} // namespace

class change_stream::impl {
   public:
    mongoc_change_stream_t* _stream;
    bsoncxx::v1::document::view _doc; // The event document.
    state _state = state::can_get_more;

    ~impl() {
        libmongoc::change_stream_destroy(_stream);
    }

    impl(impl&&) = delete;
    impl& operator=(impl&&) = delete;
    impl(impl const&) = delete;
    impl& operator=(impl const&) = delete;

    /* explicit(false) */ impl(mongoc_change_stream_t* stream) : _stream{stream} {}

    static impl const& with(change_stream const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(change_stream const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(change_stream& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(change_stream* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

change_stream::~change_stream() {
    delete impl::with(this);
}

change_stream::change_stream(change_stream&& other) noexcept : _impl(exchange(other._impl, nullptr)) {}

change_stream& change_stream::operator=(change_stream&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }
    return *this;
}

// NOLINTEND(cppcoreguidelines-owning-memory)

change_stream::iterator change_stream::begin() {
    switch (impl::with(this)->_state) {
        case state::has_doc: {
            // Do not advance on consecutive calls to `.begin()`.
            return iterator::internal::make(this);
        }

        case state::can_get_more: {
            auto iter = iterator::internal::make(this);
            ++iter; // Obtain the first active event document.
            return iter;
        }

        default:
        case state::is_dead: {
            return this->end();
        }
    }
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> change_stream::get_resume_token() const {
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> ret;

    if (bson_t const* const token = libmongoc::change_stream_get_resume_token(impl::with(this)->_stream)) {
        ret.emplace(bson_get_data(token), token->len);
    }

    return ret;
}

change_stream::change_stream(void* impl) : _impl{impl} {}

change_stream change_stream::internal::make(mongoc_change_stream_t* stream) {
    return {new impl{stream}};
}

bsoncxx::v1::document::view change_stream::internal::doc(change_stream const& self) {
    return impl::with(self)._doc;
}

bool change_stream::internal::has_doc(change_stream const& self) {
    return impl::with(self)._state == state::has_doc;
}

bool change_stream::internal::can_get_more(change_stream const& self) {
    return impl::with(self)._state == state::can_get_more;
}

bool change_stream::internal::is_dead(change_stream const& self) {
    return impl::with(self)._state == state::is_dead;
}

void change_stream::internal::advance_iterator(change_stream& self) {
    auto const _stream = impl::with(self)._stream;
    auto& _doc = impl::with(self)._doc;
    auto& _state = impl::with(self)._state;

    scoped_bson_view doc;
    bson_error_t error = {};

    if (libmongoc::change_stream_next(_stream, doc.out_ptr())) {
        // Event document is active.
        _state = state::has_doc;
        _doc = doc.view();
    } else if (libmongoc::change_stream_error_document(_stream, &error, doc.out_ptr())) {
        // An irrecoverable error occurred.
        _state = state::is_dead;
        _doc = {};
        throw_exception(error, doc.view());
    } else {
        // Change stream is resumable.
        _state = state::can_get_more;
        _doc = {};
    }
}

mongoc_change_stream_t const* change_stream::internal::as_mongoc(change_stream const& self) {
    return impl::with(self)._stream;
}

class change_stream::options::impl {
   public:
    bsoncxx::v1::stdx::optional<std::int32_t> _batch_size;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _collation;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> _comment;
    bsoncxx::v1::stdx::optional<std::string> _full_document;
    bsoncxx::v1::stdx::optional<std::string> _full_document_before_change;
    bsoncxx::v1::stdx::optional<std::chrono::milliseconds> _max_await_time;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _resume_after;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _start_after;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::b_timestamp> _start_at_operation_time;

    static impl const& with(options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl& with(options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl const* with(options const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl* with(options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

change_stream::options::~options() {
    delete impl::with(this);
}

change_stream::options::options(options&& other) noexcept : _impl(exchange(other._impl, nullptr)) {}

change_stream::options& change_stream::options::operator=(options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }
    return *this;
}

change_stream::options::options(options const& other) : _impl{new impl{impl::with(other)}} {}

change_stream::options& change_stream::options::operator=(options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }
    return *this;
}

change_stream::options::options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

change_stream::options& change_stream::options::batch_size(std::int32_t v) {
    impl::with(this)->_batch_size = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<std::int32_t> change_stream::options::batch_size() const {
    return impl::with(this)->_batch_size;
}

change_stream::options& change_stream::options::collation(bsoncxx::v1::document::value v) {
    impl::with(this)->_collation = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> change_stream::options::collation() const {
    return impl::with(this)->_collation;
}

change_stream::options& change_stream::options::comment(bsoncxx::v1::types::value v) {
    impl::with(this)->_comment = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view> change_stream::options::comment() const {
    return impl::with(this)->_comment;
}

change_stream::options& change_stream::options::full_document(std::string v) {
    impl::with(this)->_full_document = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> change_stream::options::full_document() const {
    return impl::with(this)->_full_document;
}

change_stream::options& change_stream::options::full_document_before_change(std::string v) {
    impl::with(this)->_full_document_before_change = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> change_stream::options::full_document_before_change()
    const {
    return impl::with(this)->_full_document_before_change;
}

change_stream::options& change_stream::options::max_await_time(std::chrono::milliseconds v) {
    impl::with(this)->_max_await_time = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<std::chrono::milliseconds> change_stream::options::max_await_time() const {
    return impl::with(this)->_max_await_time;
}

change_stream::options& change_stream::options::resume_after(bsoncxx::v1::document::value v) {
    impl::with(this)->_resume_after = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> change_stream::options::resume_after() const {
    return impl::with(this)->_resume_after;
}

change_stream::options& change_stream::options::start_after(bsoncxx::v1::document::value v) {
    impl::with(this)->_start_after = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> change_stream::options::start_after() const {
    return impl::with(this)->_start_after;
}

change_stream::options& change_stream::options::start_at_operation_time(bsoncxx::v1::types::b_timestamp v) {
    impl::with(this)->_start_at_operation_time = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::b_timestamp> change_stream::options::start_at_operation_time() const {
    return impl::with(this)->_start_at_operation_time;
}

bool operator==(change_stream::iterator const& lhs, change_stream::iterator const& rhs) {
    auto const _lhs = static_cast<change_stream const*>(lhs._impl);
    auto const _rhs = static_cast<change_stream const*>(rhs._impl);

    // Both are end iterators (null) or both have the same underlying stream (not null).
    if (_lhs == _rhs) {
        return true;
    }

    // One is an end iterator and the other is an inactive iterator.
    if (!_lhs != !_rhs) {
        auto const& stream = _lhs ? *_lhs : *_rhs;
        return !change_stream::internal::has_doc(stream);
    }

    // Different underlying streams and neither are end iterators.
    return false;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& change_stream::options::internal::collation(
    options const& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value> const& change_stream::options::internal::comment(
    options const& self) {
    return impl::with(self)._comment;
}

bsoncxx::v1::stdx::optional<std::string> const& change_stream::options::internal::full_document(options const& self) {
    return impl::with(self)._full_document;
}

bsoncxx::v1::stdx::optional<std::string> const& change_stream::options::internal::full_document_before_change(
    options const& self) {
    return impl::with(self)._full_document_before_change;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& change_stream::options::internal::resume_after(
    options const& self) {
    return impl::with(self)._resume_after;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& change_stream::options::internal::start_after(
    options const& self) {
    return impl::with(self)._start_after;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& change_stream::options::internal::collation(options& self) {
    return impl::with(self)._collation;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::types::value>& change_stream::options::internal::comment(options& self) {
    return impl::with(self)._comment;
}

bsoncxx::v1::stdx::optional<std::string>& change_stream::options::internal::full_document(options& self) {
    return impl::with(self)._full_document;
}

bsoncxx::v1::stdx::optional<std::string>& change_stream::options::internal::full_document_before_change(options& self) {
    return impl::with(self)._full_document_before_change;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& change_stream::options::internal::resume_after(
    options& self) {
    return impl::with(self)._resume_after;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& change_stream::options::internal::start_after(
    options& self) {
    return impl::with(self)._start_after;
}

bsoncxx::v1::document::value change_stream::options::internal::to_document(v1::change_stream::options const& opts) {
    scoped_bson bson;

    if (auto const& opt = v1::change_stream::options::internal::full_document(opts)) {
        bson += scoped_bson{BCON_NEW("fullDocument", BCON_UTF8(opt->c_str()))};
    }

    if (auto const& opt = v1::change_stream::options::internal::full_document_before_change(opts)) {
        bson += scoped_bson{BCON_NEW("fullDocumentBeforeChange", BCON_UTF8(opt->c_str()))};
    }

    if (auto const& opt = v1::change_stream::options::internal::resume_after(opts)) {
        bson += scoped_bson{BCON_NEW("resumeAfter", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = v1::change_stream::options::internal::start_after(opts)) {
        bson += scoped_bson{BCON_NEW("startAfter", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const opt = opts.batch_size()) {
        bson += scoped_bson{BCON_NEW("batchSize", BCON_INT32(*opt))};
    }

    if (auto const& opt = v1::change_stream::options::internal::collation(opts)) {
        bson += scoped_bson{BCON_NEW("collation", BCON_DOCUMENT(scoped_bson_view{*opt}.bson()))};
    }

    if (auto const& opt = v1::change_stream::options::internal::comment(opts)) {
        scoped_bson v;

        if (!BSON_APPEND_VALUE(v.inout_ptr(), "comment", &bsoncxx::v1::types::value::internal::get_bson_value(*opt))) {
            throw std::logic_error{"mongocxx::v1::client::watch: BSON_APPEND_VALUE failed"};
        }

        bson += v;
    }

    if (auto const opt = opts.start_at_operation_time()) {
        scoped_bson v;

        // BCON_TIMESTAMP() incorrectly uses int32_ptr instead of uint32_ptr. Use BSON_*() API instead.
        if (!BSON_APPEND_TIMESTAMP(v.inout_ptr(), "startAtOperationTime", opt->timestamp, opt->increment)) {
            throw std::logic_error{"mongocxx::v1::client::watch: BSON_APPEND_TIMESTAMP failed"};
        }

        bson += v;
    }

    if (auto const opt = opts.max_await_time()) {
        bson += scoped_bson{BCON_NEW("maxAwaitTimeMS", BCON_INT64(opt->count()))};
    }

    return std::move(bson).value();
}

change_stream::iterator::~iterator() = default;
change_stream::iterator::iterator(iterator&& other) noexcept = default;
change_stream::iterator& change_stream::iterator::operator=(iterator&& other) noexcept = default;
change_stream::iterator::iterator(iterator const& other) = default;
change_stream::iterator& change_stream::iterator::operator=(iterator const& other) = default;

change_stream::iterator::iterator() : _impl{nullptr} {}

namespace {

change_stream* to_change_stream(void* impl) {
    return static_cast<change_stream*>(impl);
}

} // namespace

change_stream::iterator::value_type change_stream::iterator::operator*() const {
    return _impl ? change_stream::impl::with(to_change_stream(_impl))->_doc : bsoncxx::v1::document::view{};
}

change_stream::iterator::pointer change_stream::iterator::operator->() const {
    return &change_stream::impl::with(to_change_stream(_impl))->_doc;
}

change_stream::iterator& change_stream::iterator::operator++() {
    if (_impl) {
        change_stream::internal::advance_iterator(*to_change_stream(_impl));
    }
    return *this;
}

change_stream::iterator::iterator(void* impl) : _impl{impl} {}

change_stream::iterator change_stream::iterator::internal::make(change_stream* self) {
    return {self};
}

} // namespace v1
} // namespace mongocxx
