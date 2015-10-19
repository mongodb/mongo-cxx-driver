#include "catch.hpp"
#include "helpers.hpp"

#include <mongocxx/options/insert.hpp>

using namespace mongocxx;

TEST_CASE("insert opts", "[insert][option]") {
    options::insert ins;

    CHECK_OPTIONAL_ARGUMENT(ins, bypass_document_validation, true);
    CHECK_OPTIONAL_ARGUMENT_WITHOUT_EQUALITY(ins, write_concern, write_concern{});
}
