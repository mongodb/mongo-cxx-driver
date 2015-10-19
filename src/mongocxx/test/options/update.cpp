#include "catch.hpp"
#include "helpers.hpp"

#include <mongocxx/options/update.hpp>

using namespace mongocxx;

TEST_CASE("update opts", "[update][option]") {
    options::update updt;

    CHECK_OPTIONAL_ARGUMENT(updt, upsert, true);
    CHECK_OPTIONAL_ARGUMENT(updt, bypass_document_validation, true);
    CHECK_OPTIONAL_ARGUMENT_WITHOUT_EQUALITY(updt, write_concern, write_concern{});
}
