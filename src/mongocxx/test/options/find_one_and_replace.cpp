#include "catch.hpp"
#include "helpers.hpp"

#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/options/find_one_and_replace.hpp>

using namespace bsoncxx::builder::stream;
using namespace mongocxx;

TEST_CASE("find_one_and_replace", "[find_one_and_replace][option]") {
    options::find_one_and_replace opts{};

    std::int64_t ms{400};
    auto proj = document{} << "_id" << false << finalize;
    auto sort = document{} << "x" << -1 << finalize;

    CHECK_OPTIONAL_ARGUMENT(opts, bypass_document_validation, true);
    CHECK_OPTIONAL_ARGUMENT(opts, max_time_ms, ms);
    CHECK_OPTIONAL_ARGUMENT(opts, projection, proj.view());
    CHECK_OPTIONAL_ARGUMENT(opts, return_document, options::return_document::k_before);
    CHECK_OPTIONAL_ARGUMENT(opts, sort, sort.view());
    CHECK_OPTIONAL_ARGUMENT(opts, upsert, true);
}
