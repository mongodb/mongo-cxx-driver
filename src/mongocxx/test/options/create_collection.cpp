#include "catch.hpp"

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>
#include <mongocxx/options/create_collection.hpp>

using namespace bsoncxx;
using namespace mongocxx;

TEST_CASE("create_collection", "[create_collection]") {
    options::create_collection cc;

    SECTION("Can be exported to a document") {
        cc.capped(true);
        cc.size(256);
        cc.max(100);
        cc.no_padding(false);

        auto doc = cc.to_document();
        document::view doc_view{doc.view()};

        // capped field is set to true
        document::element capped{doc_view["capped"]};
        REQUIRE(capped);
        REQUIRE(capped.type() == type::k_bool);
        REQUIRE(capped.get_bool() == true);

        // autoIndexId should not be set
        document::element autoIndex{doc_view["autoIndexId"]};
        REQUIRE(!autoIndex);

        // size should be set
        document::element size{doc_view["size"]};
        REQUIRE(size);
        REQUIRE(size.type() == type::k_int32);
        REQUIRE(size.get_int32() == 256);

        // max should be set
        document::element max{doc_view["max"]};
        REQUIRE(max);
        REQUIRE(max.type() == type::k_int32);
        REQUIRE(max.get_int32() == 100);

        // noPadding should be set to false
        document::element padding{doc_view["noPadding"]};
        REQUIRE(padding);
        REQUIRE(padding.type() == type::k_bool);
        REQUIRE(padding.get_bool() == false);

        // storageEngine should not be set
        document::element engine{doc_view["storageEngine"]};
        REQUIRE(!engine);
    }
}
