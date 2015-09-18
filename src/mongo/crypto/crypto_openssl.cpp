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

#include "mongo/config.h"

#ifndef MONGO_SSL
#error This file should only be included in SSL-enabled builds
#endif

#include "mongo/platform/basic.h"

#include "mongo/util/scopeguard.h"

#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

namespace mongo {
namespace crypto {
/*
 * Computes a SHA-1 hash of 'input'.
 */
bool sha1(const unsigned char* input, const size_t inputLen, unsigned char* output) {
    EVP_MD_CTX digestCtx;
    EVP_MD_CTX_init(&digestCtx);
    ON_BLOCK_EXIT(EVP_MD_CTX_cleanup, &digestCtx);

    if (1 != EVP_DigestInit_ex(&digestCtx, EVP_sha1(), NULL)) {
        return false;
    }

    if (1 != EVP_DigestUpdate(&digestCtx, input, inputLen)) {
        return false;
    }

    return (1 == EVP_DigestFinal_ex(&digestCtx, output, NULL));
}

/*
 * Computes a HMAC SHA-1 keyed hash of 'input' using the key 'key'
 */
bool hmacSha1(const unsigned char* key,
              const size_t keyLen,
              const unsigned char* input,
              const size_t inputLen,
              unsigned char* output,
              unsigned int* outputLen) {
    return HMAC(EVP_sha1(), key, keyLen, input, inputLen, output, outputLen);
}

}  // namespace crypto
}  // namespace mongo
