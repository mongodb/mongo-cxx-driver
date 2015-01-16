// debug_util.h

/*    Copyright 2009 10gen Inc.
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


namespace mongo {

#if defined(MONGO_DEBUG_BUILD)
    const bool debug=true;
#else
    const bool debug=false;
#endif

#define MONGO_DEV if( debug )
#define DEV MONGO_DEV

    /* dassert is 'debug assert' -- might want to turn off for production as these
       could be slow.
    */
#if defined(MONGO_DEBUG_BUILD)
# define MONGO_dassert(x) invariant(x)
#else
# define MONGO_dassert(x)
#endif

#ifdef MONGO_EXPOSE_MACROS
# define dassert MONGO_dassert
#endif

} // namespace mongo
