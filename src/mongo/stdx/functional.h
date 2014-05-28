/**
 *    Copyright (C) 2014 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once

#if __cplusplus >= 201103L

#include <functional>

namespace mongo {
namespace stdx {

    using ::std::bind;
    using ::std::function;
    namespace placeholders = ::std::placeholders;

}  // namespace stdx
}  // namespace mongo

#else

#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace mongo {
namespace stdx {

    using boost::bind;
    using boost::function;

    namespace placeholders {
        static boost::arg<1> _1;
        static boost::arg<2> _2;
        static boost::arg<3> _3;
        static boost::arg<4> _4;
        static boost::arg<5> _5;
        static boost::arg<6> _6;
        static boost::arg<7> _7;
        static boost::arg<8> _8;
        static boost::arg<9> _9;
    }  // namespace placeholders

}  // namespace stdx
}  // namespace mongo

#endif

