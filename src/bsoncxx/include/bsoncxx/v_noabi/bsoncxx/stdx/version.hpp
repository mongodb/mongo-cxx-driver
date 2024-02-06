#pragma once

#define BSONCXX_FEATURE_HaveCxxStdlibVersionMacros -
#ifdef __has_include
#if __has_include(<version>)
#include <version>
#undef BSONCXX_FEATURE_HaveCxxStdlibVersionMacros
#define BSONCXX_FEATURE_HaveCxxStdlibVersionMacros +
#endif
#endif
