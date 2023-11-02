// Copyright 2023 MongoDB Inc.
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

///
/// @file bsoncxx/v1/bsoncxx/config/prelude.hpp
///
/// Macro guard prelude header for bsoncxx.
///
/// Ensures any config macro defined in config headers do not conflict with existing definitions
/// prior to inclusion. To obtain config macros without macro guard behavior, include the
/// corresponding config header directly.
///
/// Prior definitions of conflicting macros are restored by including the @ref
/// bsoncxx/v1/bsoncxx/config/postlude.hpp "matching postlude header".
///
/// @warning Include of the prelude header twice in a row without a matching include of the postlude
/// header for the first include is forbidden (e.g. prelude -> prelude -> postlude -> postlude).
/// This is to enforce correct use of macro guard headers across transitive includes.
///
/// @warning Include of the postlude header must be preceeded by a matching include of the prelude
/// header.
///
/// @note Include of the matching postlude header may be omitted if restoring prior definitions of
/// conflicting macros is not necessary, e.g. in source (.cpp) files.
///

#if defined(BSONCXX_V1_INSIDE_MACRO_GUARD_SCOPE)
#error "prelude header included twice without matching postlude"
#endif

/// Used to validate correct use of macro guard headers.
#define BSONCXX_V1_INSIDE_MACRO_GUARD_SCOPE

// Doxygen does not account for generated header files.
// Document globally applicable macros and namespaces here.

/// @namespace bsoncxx
/// The top-level namespace for bsoncxx library entities.

/// @namespace bsoncxx::v1
/// Declares entities compatible with stable ABI version 1.
