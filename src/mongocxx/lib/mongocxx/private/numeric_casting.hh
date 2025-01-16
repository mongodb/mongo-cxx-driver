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

#pragma once

#include <cstddef>
#include <cstdint>

#include <mongocxx/private/export.hh>

namespace mongocxx {

// size_t_to_int64_safe checks if @in is within the bounds of an int64_t.
// If yes, it safely casts into @out and returns true.
// If no, @out is not modified and returns false.
MONGOCXX_ABI_EXPORT_CDECL_TESTING(bool) size_t_to_int64_safe(std::size_t const in, int64_t& out);

// int64_to_int32_safe checks if @in is within the bounds of an int32_t.
// If yes, it safely casts into @out and returns true.
// If no, @out is not modified and returns false.
MONGOCXX_ABI_EXPORT_CDECL_TESTING(bool) int64_to_int32_safe(int64_t const in, int32_t& out);

// int32_to_size_t_safe checks if @in is within the bounds of an size_t.
// If yes, it safely casts into @out and returns true.
// If no, @out is not modified and returns false.
MONGOCXX_ABI_EXPORT_CDECL_TESTING(bool) int32_to_size_t_safe(int32_t const in, std::size_t& out);

// int64_to_size_t_safe checks if @in is within the bounds of an size_t.
// If yes, it safely casts into @out and returns true.
// If no, @out is not modified and returns false.
MONGOCXX_ABI_EXPORT_CDECL_TESTING(bool) int64_to_size_t_safe(int64_t const in, std::size_t& out);

} // namespace mongocxx
