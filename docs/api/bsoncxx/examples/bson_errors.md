# Create a BSON Document

## From an Invalid JSON String

@snippet api/bsoncxx/examples/bson_errors/create_json.cpp Example

## Using the Basic Document Builder

### Basic Append Failure

@snippet api/bsoncxx/examples/bson_errors/create_doc_append.cpp Example

### Sub-Document Append Failure

@snippet api/bsoncxx/examples/bson_errors/create_doc_append_sub_document.cpp Example

### Sub-Array Append Failure

@snippet api/bsoncxx/examples/bson_errors/create_doc_append_sub_array.cpp Example

## Using the Basic Array Builder

### Basic Append Failure

@snippet api/bsoncxx/examples/bson_errors/create_arr_append.cpp Example

### Sub-Document Append Failure

@snippet api/bsoncxx/examples/bson_errors/create_arr_append_sub_document.cpp Example

### Sub-Array Append Failure

@snippet api/bsoncxx/examples/bson_errors/create_arr_append_sub_array.cpp Example

# Access a Document Element

## By Iteration

### End Iterators

@snippet api/bsoncxx/examples/bson_errors/access_doc_iter_end.cpp Example

### Invalid BSON Documents

@snippet api/bsoncxx/examples/bson_errors/access_doc_iter_invalid.cpp Example

## By Key

### Missing Element

@snippet api/bsoncxx/examples/bson_errors/access_doc_key_missing.cpp Example

# Access an Array Element

## By Iteration

### End Iterators

@snippet api/bsoncxx/examples/bson_errors/access_arr_iter_end.cpp Example

### Invalid BSON Arrays

@snippet api/bsoncxx/examples/bson_errors/access_arr_iter_invalid.cpp Example

## By Key

### Missing Element

@snippet api/bsoncxx/examples/bson_errors/access_arr_key_missing.cpp Example

# Query an Element

## In a Document

### Invalid Element

@snippet api/bsoncxx/examples/bson_errors/query_element_doc_invalid.cpp Example

### Invalid Type

@snippet api/bsoncxx/examples/bson_errors/query_element_doc_invalid_type.cpp Example

## In an Array

### Invalid Element

@snippet api/bsoncxx/examples/bson_errors/query_element_arr_invalid.cpp Example

### Invalid Type

@snippet api/bsoncxx/examples/bson_errors/query_element_arr_invalid_type.cpp Example

# Use a BSON Value

## Query an Invalid Type

@snippet api/bsoncxx/examples/bson_errors/query_bson_value_invalid_type.cpp Example

## Create an Invalid Value

@snippet api/bsoncxx/examples/bson_errors/create_bson_value_invalid_type.cpp Example

## From an Invalid Element

@snippet api/bsoncxx/examples/bson_errors/create_bson_value_invalid_element.cpp Example

# Convert to a JSON String

## From an Invalid BSON Document

@snippet api/bsoncxx/examples/bson_errors/to_json_invalid_doc.cpp Example

## From an Invalid BSON Array

@snippet api/bsoncxx/examples/bson_errors/to_json_invalid_arr.cpp Example
