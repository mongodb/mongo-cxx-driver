#include <chrono>

#include "catch.hpp"
#include "helpers.hpp"

#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/options/find_one_and_update.hpp>

using namespace bsoncxx::builder::stream;
using namespace mongocxx;

TEST_CASE("find_one_and_update", "[find_one_and_update][option]") {
    options::find_one_and_update opts{};

    std::chrono::milliseconds ms{400};
    auto proj = document{} << "_id" << false << finalize;
    auto sort = document{} << "x" << -1 << finalize;

    CHECK_OPTIONAL_ARGUMENT(opts, bypass_document_validation, true);
    CHECK_OPTIONAL_ARGUMENT(opts, max_time, ms);
    CHECK_OPTIONAL_ARGUMENT(opts, projection, proj.view());
    CHECK_OPTIONAL_ARGUMENT(opts, return_document, options::return_document::k_before);
    CHECK_OPTIONAL_ARGUMENT(opts, sort, sort.view());
    CHECK_OPTIONAL_ARGUMENT(opts, upsert, true);
}
