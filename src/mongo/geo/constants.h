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

namespace mongo {
namespace geo {

/**
 * The special field names defined by the GeoJSON specification.
 */
const char kBoundingBoxFieldName[] = "bbox";
const char kCoordsFieldName[] = "coordinates";
const char kGeometriesFieldName[] = "geometries";
const char kTypeFieldName[] = "type";

/**
 * The different possible geometry type names in GeoJSON.
 */
const char kGeometryCollectionTypeStr[] = "GeometryCollection";
const char kLineStringTypeStr[] = "LineString";
const char kMultiLineStringTypeStr[] = "MultiLineString";
const char kMultiPointTypeStr[] = "MultiPoint";
const char kMultiPolygonTypeStr[] = "MultiPolygon";
const char kPointTypeStr[] = "Point";
const char kPolygonTypeStr[] = "Polygon";

/**
 * An enum to represent the type of geometry of a Geo object.
 */
enum GeoObjType {
    GeoObjType_GeometryCollection,
    GeoObjType_LineString,
    GeoObjType_MultiLineString,
    GeoObjType_MultiPoint,
    GeoObjType_MultiPolygon,
    GeoObjType_Point,
    GeoObjType_Polygon
};

}  // namespace geo
}  // namespace mongo
