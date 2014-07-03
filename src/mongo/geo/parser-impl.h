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
#include "mongo/geo/constants.h"
#include "mongo/geo/geometry.h"
#include "mongo/geo/linestring.h"
#include "mongo/geo/multilinestring.h"
#include "mongo/geo/multipoint.h"
#include "mongo/geo/multipolygon.h"
#include "mongo/geo/geoobj.h"
#include "mongo/geo/point.h"
#include "mongo/geo/polygon.h"
#include "mongo/geo/parser.h"
#include "mongo/util/assert_util.h"

namespace mongo {
namespace geo {

    template<typename TCoordinates>
    GeoObj<TCoordinates>* Parser<TCoordinates>::parse(const BSONObj& bson) {
        BSONElement typeField = bson.getField(kTypeFieldName);

        uassert(0, "bson argument must have field \"type\" that has value of type string.",
                !typeField.eoo() && typeField.type() == String);

        switch(stringToType(typeField.String())) {
        case GeoObjType_Point:
            return new Point<TCoordinates>(bson);
        case GeoObjType_MultiPoint:
            return new MultiPoint<TCoordinates>(bson);
        case GeoObjType_LineString:
            return new LineString<TCoordinates>(bson);
        case GeoObjType_MultiLineString:
            return new MultiLineString<TCoordinates>(bson);
        case GeoObjType_Polygon:
            return new Polygon<TCoordinates>(bson);
        case GeoObjType_MultiPolygon:
            return new MultiPolygon<TCoordinates>(bson);
        case GeoObjType_GeometryCollection:
            return new GeometryCollection<TCoordinates>(bson);
        default:
            uassert(0, "bson must contain a type supported by MongoDB.", false);
        }
    }

    template<typename TCoordinates>
    GeoObjType Parser<TCoordinates>::stringToType(const StringData& typeStr) {
        if (typeStr == kPointTypeStr)
            return GeoObjType_Point;
        if (typeStr == kLineStringTypeStr)
            return GeoObjType_LineString;
        if (typeStr == kPolygonTypeStr)
            return GeoObjType_Polygon;
        if (typeStr == kMultiPointTypeStr)
            return GeoObjType_MultiPoint;
        if (typeStr == kMultiLineStringTypeStr)
            return GeoObjType_MultiLineString;
        if (typeStr == kMultiPolygonTypeStr)
            return GeoObjType_MultiPolygon;
        if (typeStr == kGeometryCollectionTypeStr)
            return GeoObjType_GeometryCollection;

        uassert(0, "typeStr must contain a GeoJSON type supported by MongoDB", false);
    }

} // namespace geo
} // namespace mongo
