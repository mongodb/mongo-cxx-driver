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
#include <bsoncxx/vector/formats.hpp>
#include <bsoncxx/vector/view.hpp>

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
                auto view = sbin.allocate(bsoncxx::vector::formats::f_int8{}, 1000);
                uint8_t i = 0;
                std::generate(view.begin(), view.end(), [&] { return (int8_t)++i; });
            }),
        kvp("vector_float32",
            [&](sub_binary sbin) {
                auto view = sbin.allocate(bsoncxx::vector::formats::f_float32{}, 1000);
                view[0] = 0.f;
                view[1] = 1e-38f;
                for (size_t i = 2; i < view.size(); i++) {
                    view[i] = view[i - 1] + view[i - 2];
                }
                for (auto i = view.begin(); i != view.end(); i++) {
                    if (!(*i * 0.f < *i)) {
                        *i = float(std::sin(double(i - view.begin()) * 1e-3));
                    }
                }
                std::fill(view.end() - 10, view.end() - 7, std::numeric_limits<float>::infinity());
                view[0] += 1.f;
                view[1] *= 1e38f;
                view[1] /= 2.f;
                view[1] -= 1.f + view[0];
            }),
        kvp("vector_packed_bit", [&](sub_binary sbin) {
            auto view = sbin.allocate(bsoncxx::vector::formats::f_packed_bit{}, 61);
            std::fill(view.begin(), view.end(), true);
            view[5] = !view[5];
            view[6] = view[1];
            view[7] = view[5];
            view[8] = 0;
            view[60] = false;
            std::fill(view.end() - 20, view.end() - 4, false);
            std::fill(view.end() - 8, view.end() - 5, true);
            for (auto i = view.byte_begin(); i != view.byte_end(); i++) {
                *i ^= 0xFF;
            }
            std::copy(view.byte_begin(), view.byte_begin() + 2, view.byte_begin() + 2);
            std::copy(view.begin() + 5, view.begin() + 9, view.begin() + 56);
        }));

    std::cout << bsoncxx::to_json(doc) << std::endl;

    {
        bsoncxx::vector::view<bsoncxx::vector::formats::f_int8 const> v(doc["vector_int8"].get_binary());
        std::cout << "int8: " << v.size() << std::endl;
        for (auto i = v.begin(); i != v.end(); i++) {
            std::cout << int(*i) << " ";
        }
        std::cout << std::endl;
    }

    {
        bsoncxx::vector::view<bsoncxx::vector::formats::f_int8 const> v(doc["vector_int8"].get_binary());
        std::cout << "int8 bytes: " << v.byte_size() << std::hex << std::endl;
        for (auto i = v.byte_begin(); i != v.byte_end(); i++) {
            std::cout << int(*i) << " ";
        }
        std::cout << std::dec << std::endl;
    }

    {
        bsoncxx::vector::view<bsoncxx::vector::formats::f_float32 const> v(doc["vector_float32"].get_binary());
        std::cout << "float32: " << v.size() << std::endl;
        for (auto i = v.begin(); i != v.end(); i++) {
            std::cout << *i << " ";
        }
        std::cout << std::endl;
    }

    {
        bsoncxx::vector::view<bsoncxx::vector::formats::f_float32 const> v(doc["vector_float32"].get_binary());
        std::cout << "float32 bytes: " << v.byte_size() << std::hex << std::endl;
        for (auto i = v.byte_begin(); i != v.byte_end(); i++) {
            std::cout << int(*i) << " ";
        }
        std::cout << std::dec << std::endl;
    }

    {
        bsoncxx::vector::view<bsoncxx::vector::formats::f_packed_bit const> v(doc["vector_packed_bit"].get_binary());
        std::cout << "packed_bit: " << v.size() << std::endl;
        for (auto i = v.begin(); i != v.end(); i++) {
            std::cout << *i << " ";
        }
        std::cout << std::endl;
    }

    {
        bsoncxx::vector::view<bsoncxx::vector::formats::f_packed_bit const> v(doc["vector_packed_bit"].get_binary());
        std::cout << "packed_bit bytes: " << v.byte_size() << std::hex << std::endl;
        for (auto i = v.byte_begin(); i != v.byte_end(); i++) {
            std::cout << int(*i) << " ";
        }
        std::cout << std::dec << std::endl;
    }

    return 0;
}
