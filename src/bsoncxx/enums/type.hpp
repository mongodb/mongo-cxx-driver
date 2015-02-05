// Copyright 2014 MongoDB Inc.
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

#ifndef LIBBSONCXX_ENUM
#error "This header is only meant to be included as an X-macro over LIBBSONCXX_ENUM"
#endif

LIBBSONCXX_ENUM(eod, 0x00)
LIBBSONCXX_ENUM(double, 0x01)
LIBBSONCXX_ENUM(utf8, 0x02)
LIBBSONCXX_ENUM(document, 0x03)
LIBBSONCXX_ENUM(array, 0x04)
LIBBSONCXX_ENUM(binary, 0x05)
LIBBSONCXX_ENUM(undefined, 0x06)
LIBBSONCXX_ENUM(oid, 0x07)
LIBBSONCXX_ENUM(bool, 0x08)
LIBBSONCXX_ENUM(date, 0x09)
LIBBSONCXX_ENUM(null, 0x0A)
LIBBSONCXX_ENUM(regex, 0x0B)
LIBBSONCXX_ENUM(dbpointer, 0x0C)
LIBBSONCXX_ENUM(code, 0x0D)
LIBBSONCXX_ENUM(symbol, 0x0E)
LIBBSONCXX_ENUM(codewscope, 0x0F)
LIBBSONCXX_ENUM(int32, 0x10)
LIBBSONCXX_ENUM(timestamp, 0x11)
LIBBSONCXX_ENUM(int64, 0x12)
LIBBSONCXX_ENUM(maxkey, 0x7F)
LIBBSONCXX_ENUM(minkey, 0xFF)
