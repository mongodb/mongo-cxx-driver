#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#elif (_MSC_VER)
// TODO: CXX-1366 Disable MSVC warnings for libbson
#endif

#include <bson.h>

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#elif (_MSC_VER)
// TODO: CXX-1366 Disable MSVC warnings for libbson
#endif
