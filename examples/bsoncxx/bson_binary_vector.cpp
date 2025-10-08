// Copyright 2009-present MongoDB, Inc.
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

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/sub_binary.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/vector/accessor.hpp>
#include <bsoncxx/vector/formats.hpp>

#include <examples/macros.hh>

int EXAMPLES_CDECL main() {
    using bsoncxx::binary_sub_type;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::builder::basic::sub_binary;
    using bsoncxx::vector::accessor;
    using bsoncxx::vector::formats::f_float32;
    using bsoncxx::vector::formats::f_int8;
    using bsoncxx::vector::formats::f_packed_bit;

    bsoncxx::document::value doc = make_document(

        //
        // Added along with BSON Binary Vector support, the new sub_binary builder
        // allows allocating any type of BSON Binary item in-place. A callback taking
        // a sub_binary argument can calculate the required space before calling allocate()
        // on the sub_binary to get a pointer to the new in-place allocation.
        //
        // Every byte of the allocated binary region must be written or the resulting BSON
        // will have undefined contents. If allocate() isn't called exactly once,
        // an exception will be thrown.
        //
        kvp("binary",
            [&](sub_binary sbin) {
                uint32_t len = 10;
                uint8_t* data = sbin.allocate(binary_sub_type::k_binary, len);
                memset(data, 0x55, len);
            }),

        //
        // The sub_binary also provides an allocate() method for BSON Binary Vector.
        // Instead of a sub_type and byte length, this takes a vector format
        // and an element count.
        //
        // This example uses the f_int8 vector format, which has int8_t elements.
        // The allocate() call here returns a bsoncxx::vector::accessor instance
        // that works like a random access container but does not own memory directly.
        //
        kvp("vector_int8",
            [&](sub_binary sbin) {
                auto vec = sbin.allocate(f_int8{}, 10);
                int8_t i = -5;
                std::generate(vec.begin(), vec.end(), [&] { return ++i; });
            }),

        //
        // BSON Binary Vector supports formats that do not map directly to C++
        // built-in types. The f_float32 format is an unaligned little endian
        // serialization of IEEE 754 32-bit binary floating point. On some platforms,
        // this sort of data could be accessed using a raw float*, but for consistent
        // portability we have a bsoncxx::v_noabi::vector::elements::float32 type which
        // has the unaligned little-endian representation in memory but supports automatic
        // conversion to and from 'float'.
        //
        // The vector accessor works like a container of floats. Elements can be assigned
        // from float expressions or used as float expressions. Assignment operators
        // operate by automatically convering to float and then back to elements::float32.
        //
        kvp("vector_float32",
            [&](sub_binary sbin) {
                auto vec = sbin.allocate(f_float32{}, 10);
                // Calculate a fibonacci sequence starting near the smallest representable value
                vec[0] = 0.f;
                vec[1] = 1e-38f;
                for (size_t i = 2; i < vec.size(); i++) {
                    vec[i] = vec[i - 1] + vec[i - 2];
                }
                // Demonstrate assignment operators
                vec[0] += 1.f;
                vec[1] *= 1e38f;
                vec[1] /= 2.f;
                vec[1] -= 1.f + vec[0];
            }),

        //
        // packed_bit vectors support any number of single-bit elements,
        // using an accessor that works like a random-access container of
        // bool values. This works using a reference-proxy type
        // bsoncxx::v_noabi::vector::elements::packed_bit_element and an iterator
        // bsoncxx::v_noabi::vector::iterators::packed_bit_element.
        //
        // Every bsoncxx::vector::accessor can be accessed either in per-element
        // or per-byte mode. Byte mode is particularly useful for applications that
        // may want to use packed_bit vectors in the serialized format without
        // accessing individual elements.
        //
        kvp("vector_packed_bit", [&](sub_binary sbin) {
            auto vec = sbin.allocate(f_packed_bit{}, 61);
            // Start by setting all bits to 1
            std::fill(vec.begin(), vec.end(), true);
            // Flip a bit using a boolean expression
            vec[5] = !vec[5];
            // Assignment of a packed_bit_element reference copies the referenced bit value
            vec[6] = vec[1];
            vec[7] = vec[5];
            // Bits can be assigned from boolean expressions, and from zero.
            vec[8] = 0;
            vec[60] = false;
            // Demonstrate addressing bits backward from the end of the vector
            std::fill(vec.end() - 20, vec.end() - 4, false);
            std::fill(vec.end() - 8, vec.end() - 5, true);
            // Flip all bits, operating an entire byte at a time.
            // The last byte will have bits that do not correspond to any elements, and writes to these are ignored.
            for (auto i = vec.byte_begin(); i != vec.byte_end(); i++) {
                *i ^= 0xFF;
            }
            // Demonstrate copying bit ranges and byte ranges using std::copy
            std::copy(vec.byte_begin(), vec.byte_begin() + 2, vec.byte_begin() + 2);
            std::copy(vec.begin() + 5, vec.begin() + 9, vec.begin() + 56);
        }));

    // Demonstrate extended JSON serialization of the entire document
    std::cout << bsoncxx::to_json(doc) << std::endl;

    // Iterate over elements in the int8 vector
    {
        accessor<f_int8 const> vec{doc["vector_int8"].get_binary()};
        std::cout << "int8: " << vec.size() << std::endl;
        for (auto&& i : vec) {
            std::cout << int{i} << " ";
        }
        std::cout << std::endl;
    }

    // Iterate over bytes in the int8 vector
    {
        accessor<f_int8 const> vec{doc["vector_int8"].get_binary()};
        std::cout << "int8 bytes: " << vec.byte_size() << std::hex << std::endl;
        for (auto i = vec.byte_begin(); i != vec.byte_end(); i++) {
            std::cout << int{*i} << " ";
        }
        std::cout << std::dec << std::endl;
    }

    // Iterate over elements in the float32 vector
    {
        accessor<f_float32 const> vec{doc["vector_float32"].get_binary()};
        std::cout << "float32: " << vec.size() << std::endl;
        for (auto&& i : vec) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }

    // Iterate over bytes in the float32 vector
    {
        accessor<f_float32 const> vec{doc["vector_float32"].get_binary()};
        std::cout << "float32 bytes: " << vec.byte_size() << std::hex << std::endl;
        for (auto i = vec.byte_begin(); i != vec.byte_end(); i++) {
            std::cout << int{*i} << " ";
        }
        std::cout << std::dec << std::endl;
    }

    // Iterate over elements in the packed_bit vector
    {
        accessor<f_packed_bit const> vec{doc["vector_packed_bit"].get_binary()};
        std::cout << "packed_bit: " << vec.size() << std::endl;
        for (auto&& i : vec) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }

    // Iterate over bytes in the packed_bit vector
    {
        accessor<f_packed_bit const> vec{doc["vector_packed_bit"].get_binary()};
        std::cout << "packed_bit bytes: " << vec.byte_size() << std::hex << std::endl;
        for (auto i = vec.byte_begin(); i != vec.byte_end(); i++) {
            std::cout << int{*i} << " ";
        }
        std::cout << std::dec << std::endl;
    }

    return 0;
}
