/*    Copyright 2014 MongoDB Inc.
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

namespace mongo {

    enum UpdateOptions {
        /** Upsert - that is, insert the item if no matching item is found. */
        UpdateOption_Upsert = 1 << 0,

        /** Update multiple documents (if multiple documents match query expression).
           (Default is update a single document and stop.) */
        UpdateOption_Multi = 1 << 1,

        /** flag from mongo saying this update went everywhere */
        UpdateOption_Broadcast = 1 << 2
    };

    enum RemoveOptions {
        /** only delete one option */
        RemoveOption_JustOne = 1 << 0,

        /** flag from mongo saying this update went everywhere */
        RemoveOption_Broadcast = 1 << 1
    };

    /**
     * need to put in DbMesssage::ReservedOptions as well
     */
    enum InsertOptions {
        /** With muli-insert keep processing inserts if one fails */
        InsertOption_ContinueOnError = 1 << 0
    };

} // namespace mongo
