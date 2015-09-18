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

/** @file */

#pragma once

#include <string>
#include <vector>

#include "mongo/base/string_data.h"
#include "mongo/client/export_macros.h"
#include "mongo/db/jsobj.h"
#include "mongo/geo/boundingbox.h"
#include "mongo/geo/constants.h"
#include "mongo/util/assert_util.h"

namespace mongo {
namespace geo {

template <typename TCoordinates>
class GeoObj {
public:
    virtual ~GeoObj() {}

    /**
     * Obtain a BSON representation of this GeoObj.
     *
     * @return A BSON representation of this GeoObj.
     */
    virtual BSONObj toBSON() const = 0;

    /**
     * Obtain the bounding box of this GeoObj.
     *
     * @return The bounding box of this GeoObj.
     */
    virtual BoundingBox<TCoordinates> getBoundingBox() const = 0;

    /**
     * Get the geometry type of this object.
     *
     * @return The appropriate GeoObjType enum value for this object.
     */
    virtual GeoObjType getType() const = 0;

protected:
    static BSONObj validateType(const BSONObj& bson, const StringData& typeStr);
};

template <typename TCoordinates>
BSONObj GeoObj<TCoordinates>::validateType(const BSONObj& bson, const StringData& typeStr) {
    BSONElement typeField = bson.getField(kTypeFieldName);
    uassert(0,
            "bson must be a valid " + typeStr.toString() + " type",
            !typeField.eoo() && ((typeField.type() == String) || (typeField.String() == typeStr)));
    return bson;
}

}  // namespace geo
}  // namespace mongo
