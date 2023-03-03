// Copyright 2016 MongoDB Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "bsoncxx/document/view.hpp"

namespace bsoncxx {
    BSONCXX_INLINE_NAMESPACE_BEGIN
    namespace util {
        using DocView = bsoncxx::document::view;
        int unpack_size(const char *data, size_t position) {
            return *(int *) (data + position);
        }
        // Read and parse bson file
        std::vector<DocView> parse_bson_file(const std::string &bson_file_path) {
            // Read data from bson file
            std::ifstream bson_file(bson_file_path);
            std::stringstream bson_buffer;
            // Read all contents of bson_file into bson_buffer
            bson_buffer << bson_file.rdbuf();
            // Cast the type of bson_buffer to string
            std::string bson_data = bson_buffer.str();
            // The vector used to store the parsing results
            std::vector<DocView> parse_objs;
            // Begin to parse bson data
            size_t position = 0u, end = bson_data.length() - 1u, data_len = bson_data.size();
            size_t obj_size, obj_end;
            const char *dc = bson_data.c_str();
            try {
                while (position < end) {
                    obj_size = unpack_size(dc, position);
                    // Check the object size
                    if (position + obj_size > data_len) {
                        throw std::runtime_error("invalid object size");
                    }
                    obj_end = position + obj_size;
                    // Check the terminal mark at the end of the string
                    if (*(dc + obj_end - 1) != '\0') {
                        throw std::runtime_error("bad end of object");
                    }
                    // Parse single document data and return user-defined object type
                    parse_objs.emplace_back((std::uint8_t *) (dc + position), obj_size);
                    position = obj_end;
                }
            } catch (const std::exception &e) {
                throw (e);
            }
            // Return the parsed bson data
            return std::move(parse_objs);
        }
    }  // namespace util
    BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

