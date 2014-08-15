// message.cpp

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

#include "mongo/util/net/message.h"

#include <fcntl.h>
#include <errno.h>
#include <time.h>

#include "mongo/util/net/message_port.h"

namespace mongo {

    void Message::send( MessagingPort &p, const char *context ) {
        if ( empty() ) {
            return;
        }
        if ( _buf != 0 ) {
            p.send( _buf, MsgData::ConstView(_buf).getLen(), context );
        }
        else {
            p.send( _data, context );
        }
    }

    AtomicWord<MSGID> NextMsgId;

    /*struct MsgStart {
        MsgStart() {
            NextMsgId = (((unsigned) time(0)) << 16) ^ curTimeMillis();
            verify(MsgDataHeaderSize == 16);
        }
    } msgstart;*/

    MSGID nextMessageId() {
        return NextMsgId.fetchAndAdd(1);
    }

    bool doesOpGetAResponse( int op ) {
        return op == dbQuery || op == dbGetMore;
    }


} // namespace mongo
