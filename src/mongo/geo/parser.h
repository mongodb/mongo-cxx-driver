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

#include "mongo/base/string_data.h"
#include "mongo/client/export_macros.h"
#include "mongo/db/jsobj.h"
#include "mongo/geo/constants.h"
#include "mongo/geo/geoobj.h"

namespace mongo {
namespace geo {

template <typename TCoordinates>
class Parser {
public:
    /**
     * Parse the given BSON into a geometry type. Caller has ownership
     * of the returned pointer. An assertion is raised if the BSON is not a
     * valid GeoJSON object.
     *
     * @param bson The BSON defining the geometry type. Must be a valid GeoJSON
     * object and define a "type" field that describes its specific geometry.
     *
     * @return A pointer to the instantiated geometry object. This object will
     * be instantiated as the appropriate subclass of GeoObj. E.g., if the bson
     * parameter defines a LineString, the returned GeoObj* actually points to a
     * LineString. So the returned GeoObj* can be statically cast to a LineString*
     * in such case.
     */
    static GeoObj<TCoordinates>* parse(const BSONObj& bson);

private:
    static GeoObjType stringToType(const StringData& typeStr);
};

}  // namespace geo
}  // namespace mongo
