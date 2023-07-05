#include <chrono>
#include <string>
#include <vector>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/query_exception.hpp>
#include <mongocxx/exception/write_exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/search_index_view.hpp>
#include <mongocxx/pipeline.hpp>
#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>
#include <mongocxx/read_concern.hpp>
#include <mongocxx/search_index_view.hpp>
#include <mongocxx/test_util/client_helpers.hh>
#include <mongocxx/write_concern.hpp>

namespace {
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using namespace mongocxx;

}  // namespace
