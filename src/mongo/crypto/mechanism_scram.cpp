/*
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

#include "mongo/platform/basic.h"

#include "mongo/crypto/mechanism_scram.h"

#include <vector>

#include "mongo/crypto/crypto.h"
#include "mongo/platform/random.h"
#include "mongo/util/base64.h"

namespace mongo {
namespace scram {

    // Compute the SCRAM step Hi() as defined in RFC5802
    static void HMACIteration(const unsigned char input[],
                              size_t inputLen,
                              const unsigned char salt[],
                              size_t saltLen,
                              unsigned int iterationCount,
                              unsigned char output[]){
        unsigned char intermediateDigest[hashSize];
        unsigned char startKey[hashSize];
        // Placeholder for HMAC return size, will always be scram::hashSize for HMAC SHA-1
        unsigned int hashLen = 0;

        uassert(17450, "invalid salt length provided", saltLen + 4 == hashSize);
        memcpy (startKey, salt, saltLen);

        startKey[saltLen] = 0;
        startKey[saltLen+1] = 0;
        startKey[saltLen+2] = 0;
        startKey[saltLen+3] = 1;

        // U1 = HMAC(input, salt + 0001)
        fassert(17494, crypto::hmacSha1(input,
                                        inputLen,
                                        startKey,
                                        saltLen + 4,
                                        output,
                                        &hashLen));

        memcpy(intermediateDigest, output, hashSize);

        // intermediateDigest contains Ui and output contains the accumulated XOR:ed result
        for (size_t i = 2; i <= iterationCount; i++) {
            unsigned char intermediateOutput[hashSize];
            fassert(17495, crypto::hmacSha1(input,
                                            inputLen,
                                            intermediateDigest,
                                            hashSize,
                                            intermediateOutput,
                                            &hashLen));
            memcpy(intermediateDigest, intermediateOutput, hashSize);
            for (size_t k = 0; k < hashSize; k++) {
                output[k] ^= intermediateDigest[k];
            }
        }
    }

    // Iterate the hash function to generate SaltedPassword
    void generateSaltedPassword(const StringData& hashedPassword,
                                const unsigned char* salt,
                                const int saltLen,
                                const int iterationCount,
                                unsigned char saltedPassword[hashSize]) {
        // saltedPassword = Hi(hashedPassword, salt)
        HMACIteration(reinterpret_cast<const unsigned char*>(hashedPassword.rawData()),
                      hashedPassword.size(),
                      salt,
                      saltLen,
                      iterationCount,
                      saltedPassword);
    }

    void generateSecrets(const std::string& hashedPassword,
                         const unsigned char salt[],
                         size_t saltLen,
                         size_t iterationCount,
                         unsigned char storedKey[hashSize],
                         unsigned char serverKey[hashSize]) {

        unsigned char saltedPassword[hashSize];
        unsigned char clientKey[hashSize];
        unsigned int hashLen = 0;

        generateSaltedPassword(hashedPassword,
                               salt,
                               saltLen,
                               iterationCount,
                               saltedPassword);

        // clientKey = HMAC(saltedPassword, "Client Key")
        fassert(17498, 
                crypto::hmacSha1(saltedPassword,
                                 hashSize,
                                 reinterpret_cast<const unsigned char*>(clientKeyConst.data()),
                                 clientKeyConst.size(),
                                 clientKey,
                                 &hashLen));

        // storedKey = H(clientKey)
        fassert(17499, crypto::sha1(clientKey, hashSize, storedKey));

        // serverKey = HMAC(saltedPassword, "Server Key")
        fassert(17500, 
                crypto::hmacSha1(saltedPassword,
                                 hashSize,
                                 reinterpret_cast<const unsigned char*>(serverKeyConst.data()),
                                 serverKeyConst.size(),
                                 serverKey,
                                 &hashLen));
    }

    BSONObj generateCredentials(const std::string& hashedPassword, int iterationCount) {

        const int saltLenQWords = 2;

        // Generate salt
        uint64_t userSalt[saltLenQWords];
        boost::scoped_ptr<SecureRandom> sr(SecureRandom::create());

        userSalt[0] = sr->nextInt64();
        userSalt[1] = sr->nextInt64();
        std::string encodedUserSalt =
            base64::encode(reinterpret_cast<char*>(userSalt), sizeof(userSalt));

        // Compute SCRAM secrets serverKey and storedKey
        unsigned char storedKey[hashSize];
        unsigned char serverKey[hashSize];

        generateSecrets(hashedPassword,
                        reinterpret_cast<unsigned char*>(userSalt),
                        saltLenQWords*sizeof(uint64_t),
                        iterationCount,
                        storedKey,
                        serverKey);

        std::string encodedStoredKey =
            base64::encode(reinterpret_cast<char*>(storedKey), hashSize);
        std::string encodedServerKey =
            base64::encode(reinterpret_cast<char*>(serverKey), hashSize);

        return BSON(iterationCountFieldName << iterationCount <<
                    saltFieldName << encodedUserSalt <<
                    storedKeyFieldName << encodedStoredKey <<
                    serverKeyFieldName << encodedServerKey);
    }

    std::string generateClientProof(const unsigned char saltedPassword[hashSize],
                                    const std::string& authMessage) {

        // ClientKey := HMAC(saltedPassword, "Client Key")
        unsigned char clientKey[hashSize];
        unsigned int hashLen = 0;
        fassert(18689,
                crypto::hmacSha1(saltedPassword,
                                 hashSize,
                                 reinterpret_cast<const unsigned char*>(clientKeyConst.data()),
                                 clientKeyConst.size(),
                                 clientKey,
                                 &hashLen));

        // StoredKey := H(clientKey)
        unsigned char storedKey[hashSize];
        fassert(18701, crypto::sha1(clientKey, hashSize, storedKey));

        // ClientSignature := HMAC(StoredKey, AuthMessage)
        unsigned char clientSignature[hashSize];
        fassert(18702,
                crypto::hmacSha1(storedKey,
                                 hashSize,
                                 reinterpret_cast<const unsigned char*>(authMessage.c_str()),
                                 authMessage.size(),
                                 clientSignature,
                                 &hashLen));

        // ClientProof   := ClientKey XOR ClientSignature
        unsigned char clientProof[hashSize];
        for (size_t i = 0; i<hashSize; i++) {
            clientProof[i] = clientKey[i] ^ clientSignature[i];
        }

        return base64::encode(reinterpret_cast<char*>(clientProof), hashSize);

    }

    bool verifyServerSignature(const unsigned char saltedPassword[hashSize],
                               const std::string& authMessage,
                               const std::string& receivedServerSignature) {

        // ServerKey       := HMAC(SaltedPassword, "Server Key")
        unsigned int hashLen;
        unsigned char serverKey[hashSize];
        fassert(18703,
                crypto::hmacSha1(saltedPassword,
                                 hashSize,
                                 reinterpret_cast<const unsigned char*>(serverKeyConst.data()),
                                 serverKeyConst.size(),
                                 serverKey,
                                 &hashLen));

        // ServerSignature := HMAC(ServerKey, AuthMessage)
        unsigned char serverSignature[hashSize];
        fassert(18704,
                crypto::hmacSha1(serverKey,
                                 hashSize,
                                 reinterpret_cast<const unsigned char*>(authMessage.c_str()),
                                 authMessage.size(),
                                 serverSignature,
                                 &hashLen));

        std::string encodedServerSignature =
            base64::encode(reinterpret_cast<char*>(serverSignature), sizeof(serverSignature));
        return (receivedServerSignature == encodedServerSignature);
    }

} // namespace scram
} // namespace mongo
