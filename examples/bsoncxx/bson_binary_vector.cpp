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
#include <limits>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/sub_binary.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/vector/accessor.hpp>
#include <bsoncxx/vector/formats.hpp>

#include <examples/macros.hh>

int EXAMPLES_CDECL main() {
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::builder::basic::sub_binary;

    bsoncxx::document::value doc = make_document(
        kvp("binary",
            [&](sub_binary sbin) {
                uint32_t len = 1000;
                uint8_t* vec = sbin.allocate(bsoncxx::binary_sub_type::k_binary, len);
                memset(vec, 0x55, len);
            }),
        kvp("vector_int8",
            [&](sub_binary sbin) {
                auto vec = sbin.allocate(bsoncxx::vector::formats::f_int8{}, 1000);
                uint8_t i = 0;
                std::generate(vec.begin(), vec.end(), [&] { return (int8_t)++i; });
            }),
        kvp("vector_float32",
            [&](sub_binary sbin) {
                auto vec = sbin.allocate(bsoncxx::vector::formats::f_float32{}, 1000);
                vec[0] = 0.f;
                vec[1] = 1e-38f;
                for (size_t i = 2; i < vec.size(); i++) {
                    vec[i] = vec[i - 1] + vec[i - 2];
                }
                for (auto i = vec.begin(); i != vec.end(); i++) {
                    if (!(*i * 0.f < *i)) {
                        *i = float(std::sin(double(i - vec.begin()) * 1e-3));
                    }
                }
                std::fill(vec.end() - 10, vec.end() - 7, std::numeric_limits<float>::infinity());
                vec[0] += 1.f;
                vec[1] *= 1e38f;
                vec[1] /= 2.f;
                vec[1] -= 1.f + vec[0];
            }),
        kvp("vector_packed_bit", [&](sub_binary sbin) {
            auto vec = sbin.allocate(bsoncxx::vector::formats::f_packed_bit{}, 61);
            std::fill(vec.begin(), vec.end(), true);
            vec[5] = !vec[5];
            vec[6] = vec[1];
            vec[7] = vec[5];
            vec[8] = 0;
            vec[60] = false;
            std::fill(vec.end() - 20, vec.end() - 4, false);
            std::fill(vec.end() - 8, vec.end() - 5, true);
            for (auto i = vec.byte_begin(); i != vec.byte_end(); i++) {
                *i ^= 0xFF;
            }
            std::copy(vec.byte_begin(), vec.byte_begin() + 2, vec.byte_begin() + 2);
            std::copy(vec.begin() + 5, vec.begin() + 9, vec.begin() + 56);
        }));

    std::cout << bsoncxx::to_json(doc) << std::endl;

    {
        bsoncxx::vector::accessor<bsoncxx::vector::formats::f_int8 const> vec(doc["vector_int8"].get_binary());
        std::cout << "int8: " << vec.size() << std::endl;
        for (auto i = vec.begin(); i != vec.end(); i++) {
            std::cout << int(*i) << " ";
        }
        std::cout << std::endl;
    }

    {
        bsoncxx::vector::accessor<bsoncxx::vector::formats::f_int8 const> vec(doc["vector_int8"].get_binary());
        std::cout << "int8 bytes: " << vec.byte_size() << std::hex << std::endl;
        for (auto i = vec.byte_begin(); i != vec.byte_end(); i++) {
            std::cout << int(*i) << " ";
        }
        std::cout << std::dec << std::endl;
    }

    {
        bsoncxx::vector::accessor<bsoncxx::vector::formats::f_float32 const> vec(doc["vector_float32"].get_binary());
        std::cout << "float32: " << vec.size() << std::endl;
        for (auto i = vec.begin(); i != vec.end(); i++) {
            std::cout << *i << " ";
        }
        std::cout << std::endl;
    }

    {
        bsoncxx::vector::accessor<bsoncxx::vector::formats::f_float32 const> vec(doc["vector_float32"].get_binary());
        std::cout << "float32 bytes: " << vec.byte_size() << std::hex << std::endl;
        for (auto i = vec.byte_begin(); i != vec.byte_end(); i++) {
            std::cout << int(*i) << " ";
        }
        std::cout << std::dec << std::endl;
    }

    {
        bsoncxx::vector::accessor<bsoncxx::vector::formats::f_packed_bit const> vec(
            doc["vector_packed_bit"].get_binary());
        std::cout << "packed_bit: " << vec.size() << std::endl;
        for (auto i = vec.begin(); i != vec.end(); i++) {
            std::cout << *i << " ";
        }
        std::cout << std::endl;
    }

    {
        bsoncxx::vector::accessor<bsoncxx::vector::formats::f_packed_bit const> vec(
            doc["vector_packed_bit"].get_binary());
        std::cout << "packed_bit bytes: " << vec.byte_size() << std::hex << std::endl;
        for (auto i = vec.byte_begin(); i != vec.byte_end(); i++) {
            std::cout << int(*i) << " ";
        }
        std::cout << std::dec << std::endl;
    }

    return 0;
}
