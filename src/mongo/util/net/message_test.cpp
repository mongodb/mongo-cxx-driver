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

#include "mongo/util/net/message.h"

#include "mongo/unittest/unittest.h"

namespace {
  
    using mongo::MsgData;

    TEST(dataAsInt, get) {
        MsgData msg_data;
        msg_data._data[0] = 0x01;
        msg_data._data[1] = 0x02;
        msg_data._data[2] = 0x03;
        msg_data._data[3] = 0x04;
        ASSERT_EQUALS(msg_data.dataAsInt(), 0x04030201);
    }

    TEST(dataAsInt, set) {
        MsgData msg_data;
        msg_data.setData(0x04030201);
        ASSERT_EQUALS(msg_data._data[0], 0x01);
        ASSERT_EQUALS(msg_data._data[1], 0x02);
        ASSERT_EQUALS(msg_data._data[2], 0x03);
        ASSERT_EQUALS(msg_data._data[3], 0x04);
    }

} // unnamed namespace

