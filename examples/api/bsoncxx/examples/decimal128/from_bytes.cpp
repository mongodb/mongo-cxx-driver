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

#include <cstdint>

#include <bsoncxx/decimal128.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    using d128 = bsoncxx::decimal128; // For brevity.

    // Default ctor.
    EXPECT((d128{0u, 0u}) == d128{});

    // [0,0,0]
    EXPECT((d128{0x3040000000000000, 0x0000000000000000}) == d128{"0"});

    // [1,0,0]
    EXPECT((d128{0xb040000000000000, 0x0000000000000000}) == d128{"-0"});

    // [0,0,1]
    EXPECT((d128{0x3040000000000000, 0x0000000000000000}) == d128{"00E0"});

    // [0,0,-1]
    EXPECT((d128{0x303e000000000000, 0x0000000000000000}) == d128{"0.0E0"});

    // [0,123,0]
    EXPECT((d128{0x3040000000000000, 0x000000000000007b}) == d128{"123"});

    // [1,123,0]
    EXPECT((d128{0xb040000000000000, 0x000000000000007b}) == d128{"-123"});

    // [0,123,1]
    EXPECT((d128{0x3042000000000000, 0x000000000000007b}) == d128{"1.23E3"});

    // [1,123,1]
    EXPECT((d128{0xb042000000000000, 0x000000000000007b}) == d128{"-1.23E3"});

    // [0,123,-1]
    EXPECT((d128{0x303e000000000000, 0x000000000000007b}) == d128{"12.3"});

    // [1,123,-1]
    EXPECT((d128{0xb03e000000000000, 0x000000000000007b}) == d128{"-12.3"});

    // [0,inf]
    EXPECT((d128{0x7800000000000000, 0x0000000000000000}) == d128{"Infinity"});

    // [1,inf]
    EXPECT((d128{0xf800000000000000, 0x0000000000000000}) == d128{"-Infinity"});

    // [0,qNaN]
    EXPECT((d128{0x7c00000000000000, 0x0000000000000000}) == d128{"NaN"});

    // [1,qNaN]: Negative NaN is string-represented as NaN.
    EXPECT((d128{0xfc00000000000000, 0x0000000000000000}).to_string() == "NaN");

    // [0,sNaN]: Signaling NaN is string-represented as NaN.
    EXPECT((d128{0x7e00000000000000, 0x0000000000000000}).to_string() == "NaN");

    // [1,sNaN]: Negative Signaling NaN is string-represented as NaN.
    EXPECT((d128{0x7e00000000000000, 0x0000000000000000}).to_string() == "NaN");
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
