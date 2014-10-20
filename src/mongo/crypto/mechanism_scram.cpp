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

#ifdef MONGO_SSL
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#endif

#include "mongo/platform/random.h"
#include "mongo/util/base64.h"

namespace mongo {
namespace scram {

// Need to #ifdef this until our SCRAM implementation
// is independent of libcrypto
#ifdef MONGO_SSL
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
        fassert(17494, HMAC(EVP_sha1(),
                            input,
                            inputLen,
                            startKey,
                            saltLen + 4,
                            output,
                            &hashLen));

        memcpy(intermediateDigest, output, hashSize);

        // intermediateDigest contains Ui and output contains the accumulated XOR:ed result
        for (size_t i = 2; i <= iterationCount; i++) {
            fassert(17495, HMAC(EVP_sha1(),
                                input,
                                inputLen,
                                intermediateDigest,
                                hashSize,
                                intermediateDigest,
                                &hashLen));

            for (size_t k = 0; k < hashSize; k++) {
                output[k] ^= intermediateDigest[k];
            }
        }
    }

    // Iterate the hash function to generate SaltedPassword
    void generateSaltedPassword(const StringData& password,
                                const unsigned char* salt,
                                const int saltLen,
                                const int iterationCount,
                                unsigned char saltedPassword[hashSize]) {
        // saltedPassword = Hi(password, salt)
        HMACIteration(password.rawData(),
                      password.size(),
                      salt,
                      saltLen,
                      iterationCount,
                      saltedPassword);
    }

    /* Compute the SCRAM secrets storedKey and serverKey
     * as defined in RFC5802 */
    static void computeProperties(const std::string& password,
                                  const unsigned char salt[],
                                  size_t saltLen,
                                  size_t iterationCount,
                                  unsigned char storedKey[hashSize],
                                  unsigned char serverKey[hashSize]) {

        unsigned char saltedPassword[hashSize];
        unsigned char clientKey[hashSize];
        unsigned int hashLen = 0;

        generateSaltedPassword(password,
                               salt,
                               saltLen,
                               iterationCount,
                               saltedPassword);

        // clientKey = HMAC(saltedPassword, "Client Key")
        fassert(17498, HMAC(EVP_sha1(),
                            saltedPassword,
                            hashSize,
                            clientKeyConst.data(),
                            clientKeyConst.size(),
                            clientKey,
                            &hashLen));

        // storedKey = H(clientKey)
        fassert(17499, SHA1(clientKey, hashSize, storedKey));

        // serverKey = HMAC(saltedPassword, "Server Key")
        fassert(17500, HMAC(EVP_sha1(),
                            saltedPassword,
                            hashSize,
                            serverKeyConst.data(),
                            serverKeyConst.size(),
                            serverKey,
                            &hashLen));
    }

#endif //MONGO_SSL

    BSONObj generateCredentials(const std::string& hashedPassword) {
#ifndef MONGO_SSL
        return BSONObj();
#else

        // TODO: configure the default iteration count via setParameter
        const int iterationCount = 10000;
        const int saltLenQWords = 2;

        // Generate salt
        uint64_t userSalt[saltLenQWords];
        boost::scoped_ptr<SecureRandom> sr(SecureRandom::create());

        userSalt[0] = sr->nextInt64();
        userSalt[1] = sr->nextInt64();
        std::string encodedUserSalt =
            base64::encode(&userSalt, sizeof(userSalt));

        // Compute SCRAM secrets serverKey and storedKey
        unsigned char storedKey[hashSize];
        unsigned char serverKey[hashSize];

        computeProperties(hashedPassword,
                          &userSalt,
                          saltLenQWords*sizeof(uint64_t),
                          iterationCount,
                          storedKey,
                          serverKey);

        std::string encodedStoredKey =
            base64::encode(&storedKey, hashSize);
        std::string encodedServerKey =
            base64::encode(&serverKey, hashSize);

        return BSON("iterationCount" << iterationCount <<
                    "salt" << encodedUserSalt <<
                    "storedKey" << encodedStoredKey <<
                    "serverKey" << encodedServerKey);
#endif
    }

    std::string generateClientProof(const unsigned char saltedPassword[hashSize],
                                    const std::string& authMessage) {
#ifndef MONGO_SSL
        return "";
#else

        // ClientKey := HMAC(saltedPassword, "Client Key")
        unsigned char clientKey[hashSize];
        unsigned int hashLen = 0;
        fassert(18689, HMAC(EVP_sha1(),
                            saltedPassword,
                            hashSize,
                            clientKeyConst.data(),
                            clientKeyConst.size(),
                            clientKey,
                            &hashLen));

        // StoredKey := H(clientKey)
        unsigned char storedKey[hashSize];
        fassert(18701, SHA1(clientKey, hashSize, storedKey));

        // ClientSignature := HMAC(StoredKey, AuthMessage)
        unsigned char clientSignature[hashSize];
        fassert(18702, HMAC(EVP_sha1(),
                            storedKey,
                            hashSize,
                            authMessage.c_str(),
                            authMessage.size(),
                            clientSignature,
                            &hashLen));

        // ClientProof   := ClientKey XOR ClientSignature
        unsigned char clientProof[hashSize];
        for (size_t i = 0; i<hashSize; i++) {
            clientProof[i] = clientKey[i] ^ clientSignature[i];
        }

        return base64::encode(&clientProof, hashSize);

#endif // MONGO_SSL
    }

    bool verifyServerSignature(const unsigned char saltedPassword[hashSize],
                               const std::string& authMessage,
                               const std::string& receivedServerSignature) {
#ifndef MONGO_SSL
        return false;
#else
        // ServerKey       := HMAC(SaltedPassword, "Server Key")
        unsigned int hashLen;
        unsigned char serverKey[hashSize];
        fassert(18703, HMAC(EVP_sha1(),
                            saltedPassword,
                            hashSize,
                            serverKeyConst.data(),
                            serverKeyConst.size(),
                            serverKey,
                            &hashLen));

        // ServerSignature := HMAC(ServerKey, AuthMessage)
        unsigned char serverSignature[hashSize];
        fassert(18704, HMAC(EVP_sha1(),
                            serverKey,
                            hashSize,
                            authMessage.c_str(),
                            authMessage.size(),
                            serverSignature,
                            &hashLen));

        std::string encodedServerSignature =
            base64::encode(&serverSignature, sizeof(serverSignature));
        return (receivedServerSignature == encodedServerSignature);
#endif
    }

} // namespace scram
} // namespace mongo
