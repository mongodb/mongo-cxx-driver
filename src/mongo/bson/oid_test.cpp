/*    Copyright 2013 10gen Inc.
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

#include "mongo/db/jsobj.h"
#include "mongo/db/json.h"

#include "mongo/unittest/unittest.h"

namespace mongo {

  TEST(foldInPid, smallPid) {
        OID::MachineAndPid machineAndPid = {{0x44, 0x66, 0x99}, 0xAACC};

        machineAndPid.foldInPid(0x01);

        OID::MachineAndPid expected = {{0x44, 0x66, 0x99}, 0xAACD};
        ASSERT_EQUALS(expected, machineAndPid);
    }

    TEST(foldInPid, bigPid) {
        OID::MachineAndPid machineAndPid = {{0x44, 0x66, 0x99}, 0xAACC};

        machineAndPid.foldInPid(0x12345678);

        OID::MachineAndPid expected = {{0x44, 0x52, 0x8B}, 0xFCB4};
        ASSERT_EQUALS(expected, machineAndPid);
    }
    
    TEST(OID, initMin) {
      OID oid;
      oid.init(0x471C71C4C0, false);
      ASSERT_EQUALS("123456780000000000000000", oid.str());
    }

    TEST(OID, initMax) {
      OID oid;
      oid.init(0x471C71C4C0, true);
      ASSERT_EQUALS("12345678ffffffffffffffff", oid.str());
    }
    
    TEST(OID, getMachineId) {
      OID::MachineAndPid machineAndPid = {{0x44, 0x66, 0x99}};
      ASSERT_EQUALS(0x00996644U, machineAndPid.getMachineNumber());
    }

    using mongo::OID;
    TEST(MachineAndPid, Equal) {
      OID::MachineAndPid a = { { 0x33, 0x66, 0x99 }, 0xccff };
      OID::MachineAndPid b(a);
      
      ASSERT_EQ(a, b);
    }

    TEST(MachineAndPid, NotEqual) {
      OID::MachineAndPid a = { { 0x33, 0x66, 0x99 }, 0xccff };
      OID::MachineAndPid b = { { 0x33, 0x00, 0x99 }, 0xccff };
      
      ASSERT_NE(a, b);
    }
    

} // unnamed namespace
