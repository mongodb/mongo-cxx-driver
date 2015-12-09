#include <chrono>

#include "catch.hpp"
#include "helpers.hpp"

#include <bsoncxx/document/view.hpp>
#include <mongocxx/options/find.hpp>

using namespace mongocxx;

TEST_CASE("find", "[find][option]") {
    options::find find_opts{};
    using cursor_type = enum options::find::cursor_type;

    CHECK_OPTIONAL_ARGUMENT(find_opts, allow_partial_results, true);
    CHECK_OPTIONAL_ARGUMENT(find_opts, batch_size, 3);
    CHECK_OPTIONAL_ARGUMENT(find_opts, comment, "comment");
    CHECK_OPTIONAL_ARGUMENT(find_opts, cursor_type, cursor_type::k_non_tailable);
    CHECK_OPTIONAL_ARGUMENT(find_opts, limit, 3);
    CHECK_OPTIONAL_ARGUMENT(find_opts, max_await_time, std::chrono::milliseconds{300});
    CHECK_OPTIONAL_ARGUMENT(find_opts, max_time, std::chrono::milliseconds{300});
    CHECK_OPTIONAL_ARGUMENT(find_opts, no_cursor_timeout, true);
    CHECK_OPTIONAL_ARGUMENT(find_opts, oplog_replay, true);
    CHECK_OPTIONAL_ARGUMENT(find_opts, skip, 3);

    CHECK_OPTIONAL_ARGUMENT_WITHOUT_EQUALITY(find_opts, read_preference, read_preference{});
    CHECK_OPTIONAL_ARGUMENT(find_opts, modifiers, bsoncxx::document::view{});
    CHECK_OPTIONAL_ARGUMENT(find_opts, projection, bsoncxx::document::view{});
    CHECK_OPTIONAL_ARGUMENT(find_opts, sort, bsoncxx::document::view{});
}
