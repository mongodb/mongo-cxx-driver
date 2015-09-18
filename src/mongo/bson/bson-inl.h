/** @file bsoninlines.h
          a goal here is that the most common bson methods can be used inline-only, a la boost.
          thus some things are inline that wouldn't necessarily be otherwise.
*/

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

#pragma once

namespace mongo {

template <class T>
inline BSONObjBuilder& BSONObjBuilderValueStream::operator<<(T value) {
    _builder->append(_fieldName, value);
    _fieldName = StringData();
    return *_builder;
}

template <class T>
inline BSONObjBuilder& Labeler::operator<<(T value) {
    s_->subobj()->append(l_.l_, value);
    return *s_->_builder;
}
}
