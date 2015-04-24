#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/types.hpp>

using namespace bsoncxx;

int main(int, char**) {


    using builder::stream::document;
    using builder::stream::array;

    // bsoncxx::builder::stream presents an iostream like interface for succintly
    // constructing complex BSON objects.

    // stream::document builds a BSON document
    auto doc = document{};
    // stream::array builds a BSON array
    auto arr = array{};

    // We append keys and values to documents using the '<<' operator;
    doc << "myKey" << "myValue";
    // We can chain any number of keys and values
    doc << "foo" << types::b_bool{false}
        << "baz" << types::b_int32{1234}
        << "quz" << types::b_double{1.234};

    // For arrays, everything just becomes a value
    arr << 1 << 2 << types::b_bool{true};

    // The stream namespace includes some helpers that can be used similarly
    // to the stream manipulators in <iomanip>
    // To build a subdocument, use open_document and close_document
    using builder::stream::open_document;
    using builder::stream::close_document;
    doc << "mySubDoc" << open_document << "subdoc key" << "subdoc value" << close_document;
    // To build a subarray, use open_array and close_array
    using builder::stream::open_array;
    using builder::stream::close_array;
    doc << "mySubArr" << open_array << 1 << types::b_bool{false} << "hello" << close_array;

    // There is a special finalize helper that converts a stream to its underlying bson value
    // this is useful for writing one-liners, for example
    using builder::stream::finalize;
    auto myQuery = document{} << "foo" << "bar" << finalize;
}
