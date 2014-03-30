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

#include <boost/scoped_ptr.hpp>
#include <boost/functional/hash.hpp>

#include "mongo/platform/atomic_word.h"
#include "mongo/platform/process_id.h"
#include "mongo/platform/random.h"
#include "mongo/bson/bsonobjbuilder.h"
#include "mongo/bson/oid.h"
#include "mongo/bson/util/atomic_int.h"

#define verify MONGO_verify

BOOST_STATIC_ASSERT( sizeof(mongo::OID) == mongo::OID::kOIDSize );
BOOST_STATIC_ASSERT( sizeof(mongo::OID) == 12 );

namespace mongo {

    using std::ostream;
    using std::string;

    using boost::scoped_ptr;

    void OID::hash_combine(size_t &seed) const {
        boost::hash_combine(seed, x);
        boost::hash_combine(seed, y);
        boost::hash_combine(seed, z);
    }

    // machine # before folding in the process id
    OID::MachineAndPid OID::ourMachine;

    ostream& operator<<( ostream &s, const OID &o ) {
        s << o.str();
        return s;
    }
    
    void OID::MachineAndPid::foldInPid(unsigned p) {
        this->_pid ^= static_cast<unsigned short>(p);
        // when the pid is greater than 16 bits, let the high bits modulate the machine id field.
        this->_machineNumber[1] ^= p >> 16;
        this->_machineNumber[2] ^= p >> 24;
    }


    void OID::foldInPid(OID::MachineAndPid& x) {
        unsigned p = ProcessId::getCurrent().asUInt32();
        x.foldInPid(p);
    }

    OID::MachineAndPid OID::genMachineAndPid() {
        BOOST_STATIC_ASSERT( sizeof(mongo::OID::MachineAndPid) == 5 );

        // we only call this once per process
        scoped_ptr<SecureRandom> sr( SecureRandom::create() );
        int64_t n = sr->nextInt64();
        OID::MachineAndPid x = ourMachine = reinterpret_cast<OID::MachineAndPid&>(n);
        foldInPid(x);
        return x;
    }

    // after folding in the process id
    OID::MachineAndPid OID::ourMachineAndPid = OID::genMachineAndPid();

    void OID::regenMachineId() {
        ourMachineAndPid = genMachineAndPid();
    }

    bool OID::MachineAndPid::operator==(const OID::MachineAndPid& rhs) const {
        return (_pid == rhs._pid) && (memcmp(_machineNumber, rhs._machineNumber, sizeof(_machineNumber)) == 0);
    }

    bool OID::MachineAndPid::operator!=(const OID::MachineAndPid& rhs) const {
        return ! (*this == rhs);
    }
    
    unsigned OID::MachineAndPid::getMachineNumber()
    {
        return _machineNumber[0] |
              (_machineNumber[1] << 8) |
              (_machineNumber[2] << 16);
    }

    unsigned OID::getMachineId() {
        return ourMachineAndPid.getMachineNumber();
    }

    void OID::justForked() {
        MachineAndPid x = ourMachine;
        // we let the random # for machine go into all 5 bytes of MachineAndPid, and then
        // xor in the pid into _pid.  this reduces the probability of collisions.
        foldInPid(x);
        ourMachineAndPid = genMachineAndPid();
        verify( x != ourMachineAndPid );
        ourMachineAndPid = x;
    }

    void OID::init() {
        static AtomicUInt inc = static_cast<unsigned>(
            scoped_ptr<SecureRandom>(SecureRandom::create())->nextInt64());

        {
            unsigned t = (unsigned) time(0);
            unsigned char *T = (unsigned char *) &t;
            _time[0] = T[3]; // big endian order because we use memcmp() to compare OID's
            _time[1] = T[2];
            _time[2] = T[1];
            _time[3] = T[0];
        }

        _machineAndPid = ourMachineAndPid;

        {
            int new_inc = inc++;
            unsigned char *T = (unsigned char *) &new_inc;
            _inc[0] = T[2];
            _inc[1] = T[1];
            _inc[2] = T[0];
        }
    }

    static AtomicUInt64 _initSequential_sequence;
    void OID::initSequential() {

        {
            unsigned t = (unsigned) time(0);
            unsigned char *T = (unsigned char *) &t;
            _time[0] = T[3]; // big endian order because we use memcmp() to compare OID's
            _time[1] = T[2];
            _time[2] = T[1];
            _time[3] = T[0];
        }
        
        {
            unsigned long long nextNumber = _initSequential_sequence.fetchAndAdd(1);
            unsigned char* numberData = reinterpret_cast<unsigned char*>(&nextNumber);
            for ( int i=0; i<8; i++ ) {
                data[4+i] = numberData[7-i];
            }
        }
    }

    void OID::init( const std::string& s ) {
        verify( s.size() == 24 );
        const char *p = s.c_str();
        for( size_t i = 0; i < kOIDSize; i++ ) {
            data[i] = fromHex(p);
            p += 2;
        }
    }

    void OID::init(Date_t date, bool max) {
        int time = (int) (date / 1000);

        // use big-endian
        char* T = (char *) &time;
        data[0] = T[3];
        data[1] = T[2];
        data[2] = T[1];
        data[3] = T[0];

        std::fill(&data[4], &data[kOIDSize], max ? 0xff : 0x00);
    }

    time_t OID::asTimeT() {
        int time;
        char* T = (char *) &time;
        T[0] = data[3];
        T[1] = data[2];
        T[2] = data[1];
        T[3] = data[0];
        return time;
    }

    const string BSONObjBuilder::numStrs[] = {
        "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",
        "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
        "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
        "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
        "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
        "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
        "60", "61", "62", "63", "64", "65", "66", "67", "68", "69",
        "70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
        "80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
        "90", "91", "92", "93", "94", "95", "96", "97", "98", "99",
    };

    // This is to ensure that BSONObjBuilder doesn't try to use numStrs before the strings have been constructed
    // I've tested just making numStrs a char[][], but the overhead of constructing the strings each time was too high
    // numStrsReady will be 0 until after numStrs is initialized because it is a static variable
    bool BSONObjBuilder::numStrsReady = (numStrs[0].size() > 0);

}
