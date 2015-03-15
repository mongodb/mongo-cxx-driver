// @file oid.cpp

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

#include "mongo/platform/basic.h"

#include "mongo/bson/oid.h"

#include <boost/functional/hash.hpp>
#include <boost/scoped_ptr.hpp>

#include "mongo/base/init.h"
#include "mongo/bson/bsonobjbuilder.h"
#include "mongo/platform/atomic_word.h"
#include "mongo/platform/random.h"
#include "mongo/util/hex.h"

namespace mongo {

namespace {
    boost::scoped_ptr<AtomicUInt32> counter;

    const std::size_t kTimestampOffset = 0;
    const std::size_t kInstanceUniqueOffset = kTimestampOffset +
                                              OID::kTimestampSize;
    const std::size_t kIncrementOffset = kInstanceUniqueOffset +
                                         OID::kInstanceUniqueSize;
    OID::InstanceUnique _instanceUnique;
}  // namespace

    MONGO_INITIALIZER_GENERAL(OIDGeneration, MONGO_NO_PREREQUISITES, ("default"))
        (InitializerContext* context) {
        boost::scoped_ptr<SecureRandom> entropy(SecureRandom::create());
        counter.reset(new AtomicUInt32(uint32_t(entropy->nextInt64())));
        _instanceUnique = OID::InstanceUnique::generate(*entropy);
        return Status::OK();
    }

    OID::Increment OID::Increment::next() {
        uint64_t nextCtr = counter->fetchAndAdd(1);
        OID::Increment incr;

        incr.bytes[0] = uint8_t(nextCtr >> 16);
        incr.bytes[1] = uint8_t(nextCtr >> 8);
        incr.bytes[2] = uint8_t(nextCtr);

        return incr;
    }

    OID::InstanceUnique OID::InstanceUnique::generate(SecureRandom& entropy) {
        int64_t rand = entropy.nextInt64();
        OID::InstanceUnique u;
        std::memcpy(u.bytes, &rand, kInstanceUniqueSize);
        return u;
    }

    void OID::setTimestamp(const OID::Timestamp timestamp) {
        _view().writeBE<Timestamp>(timestamp, kTimestampOffset);
    }

    void OID::setInstanceUnique(const OID::InstanceUnique unique) {
        // Byte order doesn't matter here
        _view().writeNative<InstanceUnique>(unique, kInstanceUniqueOffset);
    }

    void OID::setIncrement(const OID::Increment inc) {
        _view().writeNative<Increment>(inc, kIncrementOffset);
    }

    OID::Timestamp OID::getTimestamp() const {
        return view().readBE<Timestamp>(kTimestampOffset);
    }

    OID::InstanceUnique OID::getInstanceUnique() const {
        // Byte order doesn't matter here
        return view().readNative<InstanceUnique>(kInstanceUniqueOffset);
    }

    OID::Increment OID::getIncrement() const {
        return view().readNative<Increment>(kIncrementOffset);
    }

    void OID::hash_combine(size_t &seed) const {
        uint32_t v;
        for (int i = 0; i != kOIDSize; i += sizeof(uint32_t)) {
            memcpy(&v, _data + i, sizeof(uint32_t));
            boost::hash_combine(seed, v);
        }
    }

    size_t OID::Hasher::operator() (const OID& oid) const {
        size_t seed = 0;
        oid.hash_combine(seed);
        return seed;
    }

    void OID::regenMachineId() {
        boost::scoped_ptr<SecureRandom> entropy(SecureRandom::create());
        _instanceUnique = InstanceUnique::generate(*entropy);
    }

    unsigned OID::getMachineId() {
        uint32_t ret = 0;
        std::memcpy(&ret, _instanceUnique.bytes, sizeof(uint32_t));
        return ret;
    }

    void OID::justForked() {
        regenMachineId();
    }

    void OID::init() {
        // each set* method handles endianness
        setTimestamp(time(0));
        setInstanceUnique(_instanceUnique);
        setIncrement(Increment::next());
    }

    void OID::init( const std::string& s ) {
        verify( s.size() == 24 );
        const char *p = s.c_str();
        for (std::size_t i = 0; i < kOIDSize; i++) {
            _data[i] = fromHex(p);
            p += 2;
        }
    }

    void OID::init(Date_t date, bool max) {
        setTimestamp(uint32_t(date / 1000));
        uint64_t rest = max ? std::numeric_limits<uint64_t>::max() : 0u;
        std::memcpy(_view().view(kInstanceUniqueOffset), &rest,
                    kInstanceUniqueSize + kIncrementSize);
    }

    time_t OID::asTimeT() const {
        return getTimestamp();
    }

    std::string OID::toString() const {
        return toHexLower(_data, kOIDSize);
    }

    std::string OID::toIncString() const {
        return toHexLower(getIncrement().bytes, kIncrementSize);
    }

}  // namespace mongo

MONGO_INITIALIZER_FUNCTION_ASSURE_FILE(bson_oid)
