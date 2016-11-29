+++
date = "2016-08-15T16:11:58+05:30"
title = "Tutorial for mongocxx"
[menu.main]
  identifier = 'mongocxx-tutorial'
  weight = 11
  parent="mongocxx3"
+++

## Quick Start

### Prerequisites

- A [mongod](https://docs.mongodb.com/master/reference/program/mongod/)
  instance running on localhost on port 27017.

- The mongocxx Driver. See [Installation for mongocxx]({{< ref "mongocxx-v3/installation.md" >}}).

- The following statements at the top of your source file:

```c++
#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
```

### Compiling

The mongocxx driver's installation process will install a
`libmongocxx.pc` file for use
with [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/).

To compile a program, run the following command:

```sh
c++ --std=c++11 <input>.cpp $(pkg-config --cflags --libs libmongocxx)
```

If you don't have pkg-config available, you will need to set include and
library flags manually on the command line or in your IDE.  For example, if
libmongoc and mongocxx are installed in `/usr/local`, then the compilation
line in above expands to this:

```sh
c++ --std=c++11 <input>.cpp
  -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/libmongoc-1.0 \
  -I/usr/local/include/bsoncxx/v_noabi -I/usr/local/include/libbson-1.0 \
  -L/usr/local/lib -lmongocxx -lbsoncxx
```

### Make a Connection

To connect to a running MongoDB instance, use the
[`mongocxx::client`]({{< api3ref classmongocxx_1_1client >}})
class.

You must specify the host to connect to using a
[`mongocxx::uri`]({{< api3ref classmongocxx_1_1uri >}}) instance containing a
[MongoDB URI](https://docs.mongodb.com/master/reference/connection-string/),
and pass that into the `mongocxx::client` constructor.

The default `mongocxx::uri` constructor will connect to a
server running on localhost on port `27017`:

```c++
mongocxx::client client{mongocxx::uri{}};
```

This is equivalent to the following:

```c++
mongocxx::uri uri("mongodb://localhost:27017");
mongocxx::client client(uri);
```

### Access a Database and a Collection

#### Access a Database

Once you have a [`mongocxx::client`]({{< api3ref classmongocxx_1_1client >}})
instance connected to a MongoDB deployment, use either the
`database()` method or `operator[]` to obtain a
[`mongocxx::database`]({{< api3ref classmongocxx_1_1database >}})
instance.

If the database you request does not exist, MongoDB creates it when you
first store data.

The following example accesses the `mydb` database:

```c++
mongocxx::database db = client["mydb"];
```

#### Access a Collection

Once you have a
[`mongocxx::database`]({{< api3ref classmongocxx_1_1database >}})
instance, use either the `collection()` method or `operator[]` to obtain a
[`mongocxx::collection`]({{< api3ref classmongocxx_1_1collection >}})
instance.

If the collection you request does not exist, MongoDB creates it when
you first store data.

For example, using the `db` instance created in the previous section,
the following statement accesses the collection named `test` in the
`mydb` database:

```c++
mongocxx::collection coll = db["test"];
```

### Create a Document

To create a :term:`document` using the C++ driver, use one of the two
available builder interfaces:

Stream builder: `bsoncxx::builder::stream`
  A document builder using the streaming operators that works well for
  literal document construction.

Basic builder: `bsoncxx::builder::basic`
  A more conventional document builder that involves calling methods on
  a builder instance.

This guide only briefly describes the stream builder.

For example, consider the following JSON document:

```json
{
   "name" : "MongoDB",
   "type" : "database",
   "count" : 1,
   "versions": [ "v3.2", "v3.0", "v2.6" ],
   "info" : {
               "x" : 203,
               "y" : 102
            }
}
```

Using the stream builder interface, you can construct this document
as follows:

```c++
auto builder = bsoncxx::builder::stream::document{};
bsoncxx::document::value doc_value = builder
  << "name" << "MongoDB"
  << "type" << "database"
  << "count" << 1
  << "versions" << bsoncxx::builder::stream::open_array
    << "v3.2" << "v3.0" << "v2.6"
  << close_array
  << "info" << bsoncxx::builder::stream::open_document
    << "x" << 203
    << "y" << 102
  << bsoncxx::builder::stream::close_document
  << bsoncxx::builder::stream::finalize;
```

Use `bsoncxx::builder::stream::finalize` to obtain a
[`bsoncxx::document::value`]({{< api3ref classbsoncxx_1_1document_1_1value >}})
instance.

This `bsoncxx::document::value` type is a read-only object owning
its own memory. To use it, you must obtain a
[`bsoncxx::document::view`]({{< api3ref classbsoncxx_1_1document_1_1view >}}) using
the `view()` method:

```c++
bsoncxx::document::view view = doc_value.view();
```

You can access fields within this document view using `operator[]`,
which will return a
[`bsoncxx::document::element`]({{< api3ref classbsoncxx_1_1document_1_1element >}})
instance. For example, the following will extract the `name` field whose
value is a string:

```c++
bsoncxx::document::element element = view["name"];
if(element.type() != bsoncxx::type::k_utf8) {
  // Error
}
std::string name = element.get_utf8().value.to_string();
```

If the value in the `name` field is not a string and you do not
include a type guard as seen in the preceding example, this code will
throw an instance of
[`bsoncxx::exception`]({{< api3ref classbsoncxx_1_1exception >}}).

### Insert Documents

#### Insert One Document

To insert a single document into the collection, use a
[`mongocxx::collection`]({{< api3ref classmongocxx_1_1collection >}})
instance's `insert_one()` method:

```c++
mongocxx::result::insert_one result = restaurants.insert_one(doc);
```

If you do not specify a top-level `_id` field in the document,
MongoDB automatically adds an `_id` field to the inserted document.

You can obtain this value using the `inserted_id()` method of the
returned
[`mongocxx::result::insert_one`]({{< api3ref classmongocxx_1_1result_1_1insert__one >}})
instance.

#### Insert Multiple Documents

To insert multiple documents to the collection, use a
[`mongocxx::collection`]({{< api3ref classmongocxx_1_1collection >}}) instance's
`insert_many()` method, which takes a list of documents to insert.

The following example will add multiple documents of the form:

```json
{ "i" : value }
```

Create the documents in a loop and add to the documents list:

```c++
std::vector<bsoncxx::document::value> documents;
for(int i = 0; i < 100; i++) {
    documents.push_back(
      bsoncxx::builder::stream::document{} << "i" << i << finalize);
}
```

To insert these documents to the collection, pass the list of documents
to the `insert_many()` method.

```c++
collection.insert_many(documents);
```

If you do not specify a top-level `_id` field in each document,
MongoDB automatically adds a `_id` field to the inserted documents.

You can obtain this value using the `inserted_ids()` method of the
returned
[`mongocxx::result::insert_many`]({{< api3ref classmongocxx_1_1result_1_1insert__many >}})
instance.

### Query the Collection

To query the collection, use the collection’s `find()` and
`find_one` methods.

`find()` will return an instance of
[`mongocxx::cursor`]({{< api3ref classmongocxx_1_1cursor >}}),
while `find_one()` will return an instance of
`std::optional<`[`bsoncxx::document::value`]({{< api3ref classbsoncxx_1_1document_1_1value >}})`>`

You can call either method without any arguments to query all documents
in a collection, or pass a filter to query for documents that match the
filter criteria.

#### Find a Single Document in a Collection

To return a single document in the collection, use the `find_one()`
method without any parameters.

```c++
mongocxx::stdx::optional<bsoncxx::document::value> maybe_result =
  collection.find_one(document{} << finalize);
if(maybe_result) {
  // Do something with *maybe_result;
}
```

#### Find All Documents in a Collection

```c++
mongocxx::cursor cursor = collection.find(document{} << finalize);
for(auto doc : cursor) {
  std::cout << bsoncxx::to_json(doc) << "\n";
}
```

#### Specify a Query Filter

##### Get A Single Document That Matches a Filter

To find the first document where the field `i` has the value `71`,
pass the document `{"i": 71}` to specify the equality condition:

```c++
mongocxx::stdx::optional<bsoncxx::document::value> maybe_result =
  collection.find_one(document{} << "i" << 71 << finalize);
if(maybe_result) {
  std::cout << bsoncxx::to_json(*maybe_result) << "\n";
}
```

The example prints one document:

```json
{ "_id" : { "$oid" : "5755e19b38c96f1fb25667a8" },  "i" : 71 }
```

The `_id` element has been added automatically by MongoDB to your
document and your value will differ from that shown. MongoDB reserves
field names that start with an underscore (`_`) and the dollar sign
(`$`) for internal use.

##### Get All Documents That Match a Filter

The following example returns and prints all documents where
`50 < "i" <= 100`:

```c++
mongocxx::cursor cursor = collection.find(
  document{} << "i" << open_document <<
    "$gt" << 50 <<
    "$lte" << 100
  << close_document << finalize);
for(auto doc : cursor) {
  std::cout << bsoncxx::to_json(doc) << "\n";
}
```

### Update Documents

To update documents in a collection, you can use the collection’s
`update_one()` and `update_many()` methods.

The update methods return an instance of
`std::optional<`[`mongocxx::result::update`]({{< api3ref classmongocxx_1_1result_1_1update >}})`>`,
which provides information about the operation including the number of
documents modified by the update.

#### Update a Single Document

To update at most one document, use the `update_one()` method.

The following example updates the first document that matches the filter
`{ "i": 10 }` and sets the value of `i` to `110`:

```c++
collection.update_one(document{} << "i" << 10 << finalize,
                      document{} << "$set" << open_document <<
                        "i" << 110 << close_document << finalize);
```


#### Update Multiple Documents

To update all documents matching a filter, use the `update_many()`
method.

The following example increments the value of `i` by `100` where
`i` is less than `100`:

```c++
mongocxx::stdx::optional<mongocxx::result::update> result =
 collection.update_many(
  document{} << "i" << open_document <<
    "$lt" << 100 << close_document << finalize,
  document{} << "$inc" << open_document <<
    "i" << 100 << close_document << finalize);

if(result) {
  std::cout << result->modified_count() << "\n";
}
```

### Delete Documents

To delete documents from a collection, you can use a collection’s
`delete_one()` and `delete_many()` methods.

The delete methods return an instance of
`std::optional<`[`mongocxx::result::delete`]({{< api3ref classmongocxx_1_1result_1_1delete__result >}})`>`,
which contains the number of documents deleted.

#### Delete a Single Document

To delete at most a single document that matches a filter, use the
`delete_one()` method.

For example, to delete a document that matches the filter
`{ "i": 110 }`:

```c++
collection.delete_one(document{} << "i" << 110 << finalize);
```

#### Delete All Documents That Match a Filter

To delete all documents matching a filter, use a collection's
`delete_many()` method.

The following example deletes all documents where `i` is greater or
equal to `100`:

```c++
mongocxx::stdx::optional<mongocxx::result::delete_result> result =
 collection.delete_many(
  document{} << "i" << open_document <<
    "$gte" << 100 << close_document << finalize);

if(result) {
  std::cout << result->deleted_count() << "\n";
}
```

### Create Indexes

To create an [index](https://docs.mongodb.com/master/indexes/) on a
field or set of fields, pass an index specification document to the
`create_index()` method of a
[`mongocxx::collection`]({{< api3ref classmongocxx_1_1collection >}}) instance. An
index key specification document contains the fields to index and the
index type for each field:

```json
{ "index1": "<type>", "index2": <type> }
```

- For an ascending index type, specify 1 for <type>.
- For a descending index type, specify -1 for <type>.

The following example creates an ascending index on the `i` field:

```c++
auto index_specification = document{} << "i" << 1 << finalize;
collection.create_index(std::move(index_specification));
```
