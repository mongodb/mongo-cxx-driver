/*    Copyright (C) 2014 10gen Inc.
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

#include <string>

#include "mongo/base/status.h"
#include "mongo/db/jsobj.h"

namespace mongo {
namespace scram {
    const unsigned int hashSize = 20;
    const std::string serverKeyConst = "Server Key";
    const std::string clientKeyConst = "Client Key";

    /*
     * Computes the SaltedPassword from password, salt and iterationCount.
     */
    void generateSaltedPassword(const StringData& password,
                                const unsigned char* salt,
                                const int saltLen,
                                const int iterationCount,
                                unsigned char saltedPassword[hashSize]);

    /*
     * Generates the user salt and the SCRAM secrets storedKey and serverKey as
     * defined in RFC5802 (server side).
     */
    BSONObj generateCredentials(const std::string& hashedPassword);

    /*
     * Computes the ClientProof from SaltedPassword and authMessage (client side).
     */
    std::string generateClientProof(const unsigned char saltedPassword[hashSize],
                                    const std::string& authMessage);

    /*
     * Verifies ServerSignature (client side).
     */
    bool verifyServerSignature(const unsigned char saltedPassword[hashSize],
                               const std::string& authMessage,
                               const std::string& serverSignature);
} // namespace scram
} // namespace mongo
