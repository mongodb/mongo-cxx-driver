// Copyright 2015 MongoDB Inc.
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

// clang-format off

#define bsoncxx_if_msvc(...)
#define bsoncxx_if_gcc(...)
#define bsoncxx_if_clang(...)
#define bsoncxx_if_gnu_like(...) \
    bsoncxx_if_gcc(__VA_ARGS__) \
    bsoncxx_if_clang(__VA_ARGS__)

#ifdef __GNUC__
    #ifdef __clang__
        #undef bsoncxx_if_clang
        #define bsoncxx_if_clang(...) __VA_ARGS__
    #else
        #undef bsoncxx_if_gcc
        #define bsoncxx_if_gcc(...) __VA_ARGS__
    #endif
#elif defined(_MSC_VER)
    #undef bsoncxx_if_msvc
    #define bsoncxx_if_msvc(...) __VA_ARGS__
#endif

// clang-format on

// Disable MSVC warnings that cause a lot of noise related to DLL visibility
// for types that we don't control (like std::unique_ptr).
bsoncxx_push_warnings();
bsoncxx_disable_warning(MSVC(4251));
bsoncxx_disable_warning(MSVC(5275));

#define BSONCXX_INLINE inline BSONCXX_PRIVATE
#define BSONCXX_CALL bsoncxx_if_msvc(__cdecl)
