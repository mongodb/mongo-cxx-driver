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

#include <cassert>
#include <cstdint>

#include <bsoncxx/decimal128.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// See: https://speleotrove.com/decimal/daconvs.html
void example() {
    using d128 = bsoncxx::decimal128;  // For brevity.

    // Default ctor.
    assert(d128{}.to_string() == "0E-6176");

    assert(d128{"0"}.to_string() == "0");
    assert(d128{"12"}.to_string() == "12");
    assert(d128{"-76"}.to_string() == "-76");
    assert(d128{"12.70"}.to_string() == "12.70");
    assert(d128{"+0.003"}.to_string() == "0.003");
    assert(d128{"017."}.to_string() == "17");
    assert(d128{".5"}.to_string() == "0.5");
    assert(d128{"4E+9"}.to_string() == "4E+9");
    assert(d128{"0.73e-7"}.to_string() == "7.3E-8");
    assert(d128{"Inf"}.to_string() == "Infinity");
    assert(d128{"-infinity"}.to_string() == "-Infinity");
    assert(d128{"NaN"}.to_string() == "NaN");
    // "NaN8275" is not a supported string representation.

    assert(d128{"123"}.to_string() == "123");
    assert(d128{"-123"}.to_string() == "-123");
    assert(d128{"1.23E+3"}.to_string() == "1.23E+3");
    assert(d128{"1.23E+5"}.to_string() == "1.23E+5");
    assert(d128{"12.3"}.to_string() == "12.3");
    assert(d128{"0.00123"}.to_string() == "0.00123");
    assert(d128{"1.23E-8"}.to_string() == "1.23E-8");
    assert(d128{"-1.23E-10"}.to_string() == "-1.23E-10");
    assert(d128{"0"}.to_string() == "0");
    assert(d128{"0.00"}.to_string() == "0.00");
    assert(d128{"0E+2"}.to_string() == "0E+2");
    assert(d128{"-0"}.to_string() == "-0");
    assert(d128{"0.000005"}.to_string() == "0.000005");
    assert(d128{"0.0000050"}.to_string() == "0.0000050");
    assert(d128{"5E-7"}.to_string() == "5E-7");
    assert(d128{"Infinity"}.to_string() == "Infinity");
    assert(d128{"-Infinity"}.to_string() == "-Infinity");
    assert(d128{"NaN"}.to_string() == "NaN");
    // "NaN123" is not a supported string representation.
    // "-sNaN" is not a supported string representation.

    assert(d128{"1.23E+3"}.to_string() == "1.23E+3");
    assert(d128{"123E+3"}.to_string() == "1.23E+5");
    assert(d128{"12.3E-9"}.to_string() == "1.23E-8");
    assert(d128{"-123E-12"}.to_string() == "-1.23E-10");
    assert(d128{"700E-9"}.to_string() == "7.00E-7");
    assert(d128{"70"}.to_string() == "70");
    assert(d128{"0.00E+3"}.to_string() == "0E+1");

    assert(d128{"0"}.to_string() == "0");
    assert(d128{"0.00"}.to_string() == "0.00");
    assert(d128{"123"}.to_string() == "123");
    assert(d128{"-123"}.to_string() == "-123");
    assert(d128{"1.23E3"}.to_string() == "1.23E+3");
    assert(d128{"1.23E+3"}.to_string() == "1.23E+3");
    assert(d128{"12.3E+7"}.to_string() == "1.23E+8");
    assert(d128{"12.0"}.to_string() == "12.0");
    assert(d128{"12.3"}.to_string() == "12.3");
    assert(d128{"0.00123"}.to_string() == "0.00123");
    assert(d128{"-1.23E-12"}.to_string() == "-1.23E-12");
    assert(d128{"1234.5E-4"}.to_string() == "0.12345");
    assert(d128{"-0"}.to_string() == "-0");
    assert(d128{"-0.00"}.to_string() == "-0.00");
    assert(d128{"0E+7"}.to_string() == "0E+7");
    assert(d128{"-0E-7"}.to_string() == "-0E-7");
    assert(d128{"inf"}.to_string() == "Infinity");
    assert(d128{"+inFiniTy"}.to_string() == "Infinity");
    assert(d128{"-Infinity"}.to_string() == "-Infinity");
    assert(d128{"NaN"}.to_string() == "NaN");
    assert(d128{"-NAN"}.to_string() == "NaN");
    // "SNaN" is not a supported string representation.
    // "Fred" is not a supported string representation.
}
// [Example]

}  // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
