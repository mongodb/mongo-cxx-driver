/*    Copyright 2014 MongoDB Inc.
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

#include "mongo/platform/basic.h"

#include "mongo/util/password_digest.h"

namespace mongo {

std::string createPasswordDigest(const StringData& username, const StringData& clearTextPassword) {
    md5digest d;
    {
        md5_state_t st;
        md5_init(&st);
        md5_append(&st, (const md5_byte_t*)username.rawData(), username.size());
        md5_append(&st, (const md5_byte_t*)":mongo:", 7);
        md5_append(&st, (const md5_byte_t*)clearTextPassword.rawData(), clearTextPassword.size());
        md5_finish(&st, d);
    }
    return digestToString(d);
}

}  // namespace mongo
