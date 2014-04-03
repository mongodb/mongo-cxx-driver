#include <string>

#include "mongo/unittest/unittest.h"

#ifdef MONGO_EXPOSE_MACROS
#undef MONGO_EXPOSE_MACROS
#endif

#pragma once

typedef struct {
    std::string port;
} ParameterStruct;
