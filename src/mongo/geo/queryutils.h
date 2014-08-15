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

#include "mongo/client/export_macros.h"
#include "mongo/db/jsobj.h"
#include "mongo/geo/boundingbox.h"
#include "mongo/geo/constants.h"
#include "mongo/geo/geoobj.h"

namespace mongo {
namespace geo {

    template<typename TCoordinates>
    inline BSONObj MONGO_CLIENT_FUNC GEOQUERY(const char* op, const GeoObj<TCoordinates>& geoObj) {
        return BSON(op << BSON("$geometry" << geoObj.toBSON()));
    }

    template<typename TCoordinates>
    inline BSONObj MONGO_CLIENT_API GEOWITHIN(const GeoObj<TCoordinates>& geoObj) {
        return GEOQUERY("$geoWithin", geoObj);
    }

    template<typename TCoordinates>
    inline BSONObj MONGO_CLIENT_API GEOWITHIN(const BoundingBox<TCoordinates>& geoBB) {
        return BSON("$geoWithin" << BSON("$box" << geoBB.toNestedBSONArray()));
    }

    template<typename TCoordinates>
    inline BSONObj MONGO_CLIENT_API GEOINTERSECTS(const GeoObj<TCoordinates>& geoObj) {
        return GEOQUERY("$geoIntersects", geoObj);
    }

    template<typename TCoordinates>
    inline BSONObj MONGO_CLIENT_API NEAR(const GeoObj<TCoordinates>& geoObj) {
        return GEOQUERY("$near", geoObj);
    }

    template<typename TCoordinates>
    inline BSONObj MONGO_CLIENT_API NEAR(const GeoObj<TCoordinates>& geoObj, double distance) {
        return BSON("$near" << BSON("$geometry" << geoObj.toBSON()) <<
                    "$maxDistance" << distance);
    }

    template<typename TCoordinates>
    inline BSONObj MONGO_CLIENT_API NEARSPHERE(const GeoObj<TCoordinates>& geoObj) {
        return GEOQUERY("$nearSphere", geoObj);
    }

    template<typename TCoordinates>
    inline BSONObj MONGO_CLIENT_API NEARSPHERE(const GeoObj<TCoordinates>& geoObj,
        double distance) {
        return BSON("$nearSphere" << BSON("$geometry" << geoObj.toBSON()) <<
                    "$maxDistance" << distance);
    }

} // namespace geo
} // namespace mongo
