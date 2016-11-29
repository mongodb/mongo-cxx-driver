+++
date = "2016-08-15T16:11:58+05:30"
title = "Working with BSON"
[menu.main]
  identifier = 'mongocxx3-working-with-bson'
  weight = 15
  parent="mongocxx3"
+++

The mongocxx driver ships with a new library, bsoncxx.  This article will
go over some of the different types in this library, and how and when to
use each.  For more information and example code, see our
[examples](https://github.com/mongodb/mongo-cxx-driver/tree/master/examples/bsoncxx).

1. [Document Builders](#builders)<br/>
2. [Owning BSON Documents (values)](#value)<br/>
3. [Non-owning BSON Documents (views)](#view)<br/>
4. [Optionally-owning BSON Documents(view_or_value)](#view_or_value)<br/>
5. [BSON Document Lifetime](#lifetime)<br/>
6. [Printing BSON Documents](#print)<br/>
7. [Getting Fields out of BSON Documents](#fields)<br/>


### <a name="builders">Document Builders</a>

The bsoncxx library offers two different interfaces for building BSON, a
basic builder and a stream-based builder.

[`bsoncxx::builder::basic::document`](https://github.com/mongodb/mongo-cxx-driver/blob/master/src/bsoncxx/builder/basic/document.hpp)<br/>
[`bsoncxx::builder::stream::document`](https://github.com/mongodb/mongo-cxx-driver/blob/master/src/bsoncxx/builder/stream/document.hpp)

Both equivalent builder document
types are helper objects for building up BSON from scratch.  Either
interface will provide the same results, the choice of which to use is
entirely aesthetic.

**Basic builder**

```
using bsoncxx::builder::basic::kvp;

// { "hello" : "world" }
bsoncxx::builder::basic::document basic_builder{};
basic_builder.append(kvp("hello", "world"));
```

More advanced uses of the basic builder are shown in [this
example](https://github.com/mongodb/mongo-cxx-driver/blob/master/examples/bsoncxx/builder_basic.cpp).

**Stream builder**

```
// { "hello" : "world" }
// Option 1 - build over multiple lines
bsoncxx::builder::stream::document stream_builder{};
stream_builder << "hello" << "world";

// Option 2 - build in a single line
auto stream_builder = bsoncxx::builder::stream::document{} << "hello" << "world";
```

More advanced uses of the stream builder are shown in [this
example](https://github.com/mongodb/mongo-cxx-driver/blob/master/examples/bsoncxx/builder_stream.cpp).

The code above, for either interface, produces builder documents, which are
not yet BSON documents.  The builder documents will need to be converted to
fully-fledged BSON documents to be used.

### <a name="value">Owning BSON Documents (values)</a>

[`bsoncxx::document::value`](https://github.com/mongodb/mongo-cxx-driver/blob/master/src/bsoncxx/document/value.hpp)

This type represents an actual BSON document, one that owns its buffer of
data.  These documents can be constructed from a builder by calling
`extract()`:

```
bsoncxx::document::value basic_doc{basic_builder.extract()};
bsoncxx::document::value stream_doc{stream_builder.extract()};
```

Note that after calling `extract()` the builder is in a moved-from state,
and should not be used.

It's possible to create a `bsoncxx::document::value` in a single line using
the stream builder interface and the `finalize` token. `finalize` returns a
`document::value` from a temporary stream builder:

```
// { "finalize" : "is nifty" }
bsoncxx::document::value one_line = bsoncxx::builder::stream::document{} << "finalize" << "is nifty" << bsoncxx::builder::stream::finalize;
```

### <a name="view">Non-owning BSON Documents (views)</a>

[`bsoncxx::document::view`](https://github.com/mongodb/mongo-cxx-driver/blob/master/src/bsoncxx/document/view.hpp)

This type is a view into an owning `bsoncxx::document::value`.

```
bsoncxx::document::view document_view{document_value.view()};
```

A `document::value` also implicitly converts to a `document::view`:

```
bsoncxx::document::view document_view{document_value};
```

In performance-critical code, passing views around is preferable to using
values because we can avoid excess copying. Also, passing a view of a
document allows us to use the document multiple times:

```
// { "copies" : { "$gt" : 100 } }
auto query_value = document{} << "copies" << open_document << "$gt" << 100 << close_document << finalize;

// Run the same query across different collections
auto collection1 = db["science_fiction"];
auto cursor1 = collection1.find(query_value.view());

auto collection2 = db["cookbooks"];
auto cursor2 = collection2.find(query_value.view());
```

### <a name="view_or_value">Optionally-owning BSON Documents (view_or_value)</a>

Many driver methods take a document::view_or_value parameter, for example, [`run_command`](https://github.com/mongodb/mongo-cxx-driver/blob/master/src/mongocxx/database.hpp#L83-L92):

```
bsoncxx::document::value run_command(bsoncxx::document::view_or_value command);
```

Such methods can take either a `document::view` or a `document::value`. If
a `document::view` is passed in, it must be passed by r-value reference, so
ownership of the document is transferred to the method.

```
document::value ismaster = document{} << "ismaster" << 1 << finalize;

// You can pass a document::view into run_command()
db.run_command(ismaster.view());

// Or you can move in a document::value
db.run_command(std::move(ismaster));
```

You shouldn't need to create view_or_value types directly in order to use
the driver.  They are offered as a convenience method to allow driver
methods to take documents in either an owning or non-owning way.  The
`view_or_value` type also helps mitigate some of the lifetime issues
discussed in the next section.

### <a name="lifetime">BSON Document Lifetime</a>

It is imperative that `document::value`s outlive any `document::view`s that
use them. If the underlying value gets cleaned up, the view will be left
with a dangling pointer.  Consider a method that returns a view of a
newly-created document:

```
bsoncxx::document::view make_a_dangling_view() {
   bsoncxx::builder::basic::document builder{};
   builder.append(kvp("hello", "world"));

   // This creates a document::value on the stack that will disappear when we return.
   bsoncxx::document::value stack_value{builder.extract()};

   // We're returning a view of the local value
   return stack_value.view(); // Bad!!
}
```

This method returns a dangling view that should not be used:

```
// This view contains a dangling pointer
bsoncxx::document::view dangling_view = make_a_dangling_view(); // Warning!!
```

Attempting to create a view off of a builder will similarly create a
dangerous view object, because the temporary value returned from
`extract()` isn't captured:

```
bsoncxx::builder::stream::document temp_builder{};
temp_builder << "oh" << "no";
bsoncxx::document::view dangling_view = temp_builder.extract().view(); // Bad!!
```

### <a name="print">Printing BSON Documents</a>

[`bsoncxx::to_json()`](https://github.com/mongodb/mongo-cxx-driver/blob/master/src/bsoncxx/json.hpp#L28-L36)

The bsoncxx library comes with a convenience method to convert BSON
documents to strings for easy inspection:

```
bsoncxx::document::value = document{} << "I am" << "a BSON document" << finalize;
std::cout << bsoncxx::to_json(doc.view()) << std::endl;
```

There is an analogous method, [from_json()](https://github.com/mongodb/mongo-cxx-driver/blob/master/src/bsoncxx/json.hpp#L60-L68), to build document::values out of existing JSON strings.

### <a name="fields">Getting Fields out of BSON Documents</a>

The [ ] operator reaches into a BSON document to retrieve values:

```
// doc_view = { "store" : "Key Foods", "fruits" : [ "apple", "banana" ] }
auto store = doc_view["store"];
auto first_fruit = doc_view["fruits"][0];
```

This returns a `bsoncxx::document::element`, which holds the actual desired value:

```
document::element store_ele{doc_view["store"]};
if (store_ele) {
    // this block will only execute if "store" was found in the document
    std::cout << "Examining inventory at " << to_json(store_ele.get_value()) << std::endl;
}
```

This feature is shown in more detail in [this example](https://github.com/mongodb/mongo-cxx-driver/blob/master/examples/bsoncxx/getting_values.cpp) and [this example](https://github.com/mongodb/mongo-cxx-driver/blob/master/examples/bsoncxx/view_and_value.cpp).

### BSON Types

The [BSON specification](http://bsonspec.org/spec.html) provides a list
of supported types.  These are represented in C++ using the
[b_xxx](https://mongodb.github.io/mongo-cxx-driver/api/current/classes.html#letter_B)
type wrappers.

Some BSON types don't necessarily have a native representation to wrap and
are implemented via special classes.

#### Decimal128

The `bsoncxx::decimal128` class represents a 128-bit IEEE 754-2008 decimal
floating point value.  We expect users to convert these to and from
strings, but provide access to the low and high 64-bit values if users need
to convert to a native decimal128 type.

You can see how to work with `bsoncxx::decimal128` in [this example](https://github.com/mongodb/mongo-cxx-driver/blob/master/examples/bsoncxx/decimal128.cpp).
