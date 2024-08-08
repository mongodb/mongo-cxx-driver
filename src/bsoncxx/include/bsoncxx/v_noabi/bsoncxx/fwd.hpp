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

#include <bsoncxx/array/element-fwd.hpp>
#include <bsoncxx/array/value-fwd.hpp>
#include <bsoncxx/array/view-fwd.hpp>
#include <bsoncxx/builder/basic/array-fwd.hpp>
#include <bsoncxx/builder/basic/document-fwd.hpp>
#include <bsoncxx/builder/basic/sub_array-fwd.hpp>
#include <bsoncxx/builder/basic/sub_document-fwd.hpp>
#include <bsoncxx/builder/concatenate-fwd.hpp>
#include <bsoncxx/builder/core-fwd.hpp>
#include <bsoncxx/builder/list-fwd.hpp>
#include <bsoncxx/builder/stream/array-fwd.hpp>
#include <bsoncxx/builder/stream/array_context-fwd.hpp>
#include <bsoncxx/builder/stream/closed_context-fwd.hpp>
#include <bsoncxx/builder/stream/document-fwd.hpp>
#include <bsoncxx/builder/stream/helpers-fwd.hpp>
#include <bsoncxx/builder/stream/key_context-fwd.hpp>
#include <bsoncxx/builder/stream/single_context-fwd.hpp>
#include <bsoncxx/builder/stream/value_context-fwd.hpp>
#include <bsoncxx/decimal128-fwd.hpp>
#include <bsoncxx/document/element-fwd.hpp>
#include <bsoncxx/document/value-fwd.hpp>
#include <bsoncxx/document/view-fwd.hpp>
#include <bsoncxx/exception/error_code-fwd.hpp>
#include <bsoncxx/exception/exception-fwd.hpp>
#include <bsoncxx/json-fwd.hpp>
#include <bsoncxx/oid-fwd.hpp>
#include <bsoncxx/string/view_or_value-fwd.hpp>
#include <bsoncxx/types-fwd.hpp>
#include <bsoncxx/types/bson_value/value-fwd.hpp>
#include <bsoncxx/types/bson_value/view-fwd.hpp>
#include <bsoncxx/validate-fwd.hpp>
#include <bsoncxx/view_or_value-fwd.hpp>

///
/// @file
/// Aggregate of all forward headers declaring entities in @ref bsoncxx::v_noabi.
///
/// @par "Includes" @parblock
/// @li All header files under @ref src/bsoncxx/include/bsoncxx/v_noabi/bsoncxx
/// "bsoncxx/v_noabi/bsoncxx" whose filename ends with `-fwd.hpp`.
/// @endparblock
///

///
/// @dir bsoncxx/v_noabi
/// The directory relative to which headers declaring entities in @ref bsoncxx::v_noabi are
/// typically included.
///
/// For backward compatibility, unstable ABI headers may be included using the syntax
/// `<bsoncxx/foo.hpp>`, which is equivalent to `<bsoncxx/v_noabi/bsoncxx/foo.hpp>`.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx
/// Provides headers declaring entities in @ref bsoncxx::v_noabi.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/array
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::array.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/builder
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::builder.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/builder/basic
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::builder::basic.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/builder/stream
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::builder::stream.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/config
/// Provides headers related to bsoncxx library configuration.
///
/// @par "Generated Headers"
///
/// Generated headers are documented by the following pages:
///
/// @li @ref bsoncxx-v_noabi-bsoncxx-config-config-hpp
/// @li @ref bsoncxx-v_noabi-bsoncxx-config-export-hpp
/// @li @ref bsoncxx-v_noabi-bsoncxx-config-version-hpp
///

#if defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

///
/// @defgroup bsoncxx-v_noabi-bsoncxx-config-config-hpp bsoncxx/v_noabi/bsoncxx/config/config.hpp
/// This page documents macros provided by the generated header
/// `bsoncxx/v_noabi/bsoncxx/config/config.hpp`.
///
/// @name bsoncxx/v_noabi/bsoncxx/config/config.hpp
///
/// These macros are provided by @ref bsoncxx-v_noabi-bsoncxx-config-config-hpp "config.hpp",
/// **NOT** by @ref bsoncxx/v_noabi/bsoncxx/fwd.hpp "fwd.hpp" (where this is documented in source)!
///
/// @{
///

///
/// @ingroup bsoncxx-v_noabi-bsoncxx-config-config-hpp
/// Defined when configured to use a bundled mnmlstc/core as the C++17 polyfill library.
///
/// @deprecated Support for external polyfill libraries is deprecated and will be removed in an
/// upcoming major release. Set `ENABLE_BSONCXX_POLY_USE_IMPLS=ON` or `BSONCXX_POLY_USE_IMPLS=ON`
/// when configuring the bsoncxx library.
///
#define BSONCXX_POLY_USE_MNMLSTC

///
/// @ingroup bsoncxx-v_noabi-bsoncxx-config-config-hpp
/// Defined when configured to use an external mnmlstc/core as the C++17 polyfill library.
///
/// @deprecated Support for external polyfill libraries is deprecated and will be removed in an
/// upcoming major release. Set `ENABLE_BSONCXX_POLY_USE_IMPLS=ON` or `BSONCXX_POLY_USE_IMPLS=ON`
/// when configuring the bsoncxx library.
///
#define BSONCXX_POLY_USE_SYSTEM_MNMLSTC

///
/// @ingroup bsoncxx-v_noabi-bsoncxx-config-config-hpp
/// Defined when configured to use Boost as the C++17 polyfill library.
///
/// @deprecated Support for external polyfill libraries is deprecated and will be removed in an
/// upcoming major release. Set `ENABLE_BSONCXX_POLY_USE_IMPLS=ON` or `BSONCXX_POLY_USE_IMPLS=ON`
/// when configuring the bsoncxx library.
///
#define BSONCXX_POLY_USE_BOOST

///
/// @ingroup bsoncxx-v_noabi-bsoncxx-config-config-hpp
/// Defined when configured to use bsoncxx's own C++17 polyfill implementations.
///
#define BSONCXX_POLY_USE_IMPLS

///
/// @ingroup bsoncxx-v_noabi-bsoncxx-config-config-hpp
/// Defined when configured to use the C++ standard library for C++17 features.
///
#define BSONCXX_POLY_USE_STD

///
/// @ingroup bsoncxx-v_noabi-bsoncxx-config-config-hpp
/// For internal use only!
///
/// @deprecated To be removed in an upcoming major release.
/// <!-- Explicitly documented due to long-standing availability. -->
///
#define BSONCXX_INLINE_NAMESPACE_BEGIN

///
/// @ingroup bsoncxx-v_noabi-bsoncxx-config-config-hpp
/// For internal use only!
///
/// @deprecated To be removed in an upcoming major release.
/// <!-- Explicitly documented due to long-standing availability. -->
///
#define BSONCXX_INLINE_NAMESPACE_END

///
/// @} <!-- bsoncxx/v_noabi/bsoncxx/config/config.hpp -->
///

///
/// @defgroup bsoncxx-v_noabi-bsoncxx-config-export-hpp bsoncxx/v_noabi/bsoncxx/config/export.hpp
/// This page documents macros provided by the generated header
/// `bsoncxx/v_noabi/bsoncxx/config/export.hpp`.
///
/// @name bsoncxx/v_noabi/bsoncxx/config/export.hpp
///
/// These macros are provided by @ref bsoncxx-v_noabi-bsoncxx-config-export-hpp "export.hpp",
/// **NOT** by @ref bsoncxx/v_noabi/bsoncxx/fwd.hpp "fwd.hpp" (where this is documented in source)!
///
/// @{
///

///
/// @ingroup bsoncxx-v_noabi-bsoncxx-config-export-hpp
/// Declares the associated entity as part of the ABI.
///
/// @warning For internal use only!
///
/// @note This is a symbol visibility control macro, NOT a declaration of public API. Public API vs.
/// private API is determined by the placement of the entity being declared (e.g. in a public
/// header, as a public class member, etc.) or by documentation (e.g. absence of "For internal use
/// only!" warnings). See [API and ABI
/// Versioning](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/api-abi-versioning/).
///
#define BSONCXX_API

///
/// @ingroup bsoncxx-v_noabi-bsoncxx-config-export-hpp
/// Excludes the associated entity from being part of the ABI.
///
/// @warning For internal use only!
///
/// @note This is a symbol visibility control macro, NOT a declaration of private API. Public API
/// vs. private API is determined by the placement of the entity being declared (e.g. in a detail
/// header, as a private class member, etc.) or by documentation (e.g. "For internal use only!",
/// absence of documentation, etc.). See [API and ABI
/// Versioning](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/api-abi-versioning/).
///
#define BSONCXX_PRIVATE

///
/// @ingroup bsoncxx-v_noabi-bsoncxx-config-export-hpp
/// Declares the associated entity as deprecated.
///
/// @warning For internal use only!
///
#define BSONCXX_DEPRECATED

///
/// @ingroup bsoncxx-v_noabi-bsoncxx-config-export-hpp
/// Equivalent to @ref BSONCXX_API and @ref BSONCXX_DEPRECATED.
///
/// @warning For internal use only!
///
#define BSONCXX_DEPRECATED_EXPORT

///
/// @ingroup bsoncxx-v_noabi-bsoncxx-config-export-hpp
/// Equivalent to @ref BSONCXX_PRIVATE and @ref BSONCXX_DEPRECATED.
///
/// @warning For internal use only!
///
#define BSONCXX_DEPRECATED_NO_EXPORT

///
/// @} <!-- bsoncxx/v_noabi/bsoncxx/config/export.hpp -->
///

///
/// @defgroup bsoncxx-v_noabi-bsoncxx-config-version-hpp bsoncxx/v_noabi/bsoncxx/config/version.hpp
/// This page documents macros provided by the generated header
/// `bsoncxx/v_noabi/bsoncxx/config/version.hpp`.
///
/// @name bsoncxx/v_noabi/bsoncxx/config/version.hpp
///
/// These macros are provided by @ref bsoncxx-v_noabi-bsoncxx-config-version-hpp "version.hpp",
/// **NOT** by @ref bsoncxx/v_noabi/bsoncxx/fwd.hpp "fwd.hpp" (where this is documented in source)!
///
/// @{
///

///
/// @ingroup bsoncxx-v_noabi-bsoncxx-config-version-hpp
/// Defined as this bsoncxx library's API major version number.
///
/// This value is derived from the `BUILD_VERSION` variable during configuration.
///
#define BSONCXX_VERSION_MAJOR

///
/// @ingroup bsoncxx-v_noabi-bsoncxx-config-version-hpp
/// Defined as this bsoncxx library's API minor version number.
///
/// This value is derived from the `BUILD_VERSION` variable during configuration.
///
#define BSONCXX_VERSION_MINOR

///
/// @ingroup bsoncxx-v_noabi-bsoncxx-config-version-hpp
/// Defined as this bsoncxx library's API patch version number.
///
/// This value is derived from the `BUILD_VERSION` variable during configuration.
///
#define BSONCXX_VERSION_PATCH

///
/// @ingroup bsoncxx-v_noabi-bsoncxx-config-version-hpp
/// Defined as this bsoncxx library's API version number suffix if any.
///
/// This value is derived from the `BUILD_VERSION` variable during configuration.
///
#define BSONCXX_VERSION_EXTRA

///
/// @} <!-- bsoncxx/v_noabi/bsoncxx/config/version.hpp -->
///

#endif  // defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

///
/// @dir bsoncxx/v_noabi/bsoncxx/document
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::document.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/enums
/// Provides X macro headers over BSON types and binary subtypes.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/exception
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::exception.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/stdx
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::stdx.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/string
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::string.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/types
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::types.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/types/bson_value
/// Provides headers declaring entities in @ref bsoncxx::v_noabi::types::bson_value.
///

///
/// @dir bsoncxx/v_noabi/bsoncxx/util
/// Headers provided by this directory are deprecated.
///
/// @deprecated To be removed in an upcoming major release.
///

///
/// @namespace bsoncxx::v_noabi
/// Declares entities whose ABI stability is NOT guaranteed.
///

///
/// @namespace bsoncxx::v_noabi::array
/// @copydoc bsoncxx::array
///

///
/// @namespace bsoncxx::v_noabi::builder
/// @copydoc bsoncxx::builder
///

///
/// @namespace bsoncxx::v_noabi::builder::basic
/// @copydoc bsoncxx::builder::basic
///

///
/// @namespace bsoncxx::v_noabi::builder::stream
/// @copydoc bsoncxx::builder::stream
///

///
/// @namespace bsoncxx::v_noabi::document
/// @copydoc bsoncxx::document
///

///
/// @namespace bsoncxx::v_noabi::stdx
/// @copydoc bsoncxx::stdx
///

///
/// @namespace bsoncxx::v_noabi::string
/// @copydoc bsoncxx::string
///

///
/// @namespace bsoncxx::v_noabi::types
/// @copydoc bsoncxx::types
///
/// @see bsoncxx::v_noabi::types::bson_value
///

///
/// @namespace bsoncxx::v_noabi::types::bson_value
/// Declares entities representing any BSON value type.
///
