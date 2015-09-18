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

const std::string iterationCountFieldName = "iterationCount";
const std::string saltFieldName = "salt";
const std::string storedKeyFieldName = "storedKey";
const std::string serverKeyFieldName = "serverKey";

/*
 * Computes the SaltedPassword from password, salt and iterationCount.
 */
void generateSaltedPassword(const StringData& hashedPassword,
                            const unsigned char* salt,
                            const int saltLen,
                            const int iterationCount,
                            unsigned char saltedPassword[hashSize]);

/*
 * Computes the SCRAM secrets storedKey and serverKey using the salt 'salt'
 * and iteration count 'iterationCount' as defined in RFC5802 (server side).
 */
void generateSecrets(const std::string& hashedPassword,
                     const unsigned char salt[],
                     size_t saltLen,
                     size_t iterationCount,
                     unsigned char storedKey[hashSize],
                     unsigned char serverKey[hashSize]);

/*
 * Generates the user salt and the SCRAM secrets storedKey and serverKey as
 * defined in RFC5802 (server side).
 */
BSONObj generateCredentials(const std::string& hashedPassword, int iterationCount);

/*
 * Computes the ClientProof from SaltedPassword and authMessage (client side).
 */
std::string generateClientProof(const unsigned char saltedPassword[hashSize],
                                const std::string& authMessage);

/*
 * Validates that the provided password 'hashedPassword' generates the serverKey
 * 'serverKey' given iteration count 'iterationCount' and salt 'salt'.
 */
bool validatePassword(const std::string& hashedPassword,
                      int iterationCount,
                      const std::string& salt,
                      const std::string& storedKey);

/*
 * Verifies ServerSignature (client side).
 */
bool verifyServerSignature(const unsigned char saltedPassword[hashSize],
                           const std::string& authMessage,
                           const std::string& serverSignature);
}  // namespace scram
}  // namespace mongo
