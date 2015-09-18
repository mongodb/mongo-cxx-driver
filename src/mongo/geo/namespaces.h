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

#include "mongo/geo/coordinates2d.h"
#include "mongo/geo/coordinates2dgeographic.h"
#include "mongo/geo/boundingbox.h"
#include "mongo/geo/constants.h"
#include "mongo/geo/coordinates.h"
#include "mongo/geo/geometry.h"
#include "mongo/geo/geometrycollection.h"
#include "mongo/geo/linestring.h"
#include "mongo/geo/multilinestring.h"
#include "mongo/geo/multipoint.h"
#include "mongo/geo/multipolygon.h"
#include "mongo/geo/geoobj.h"
#include "mongo/geo/point.h"
#include "mongo/geo/polygon.h"
#include "mongo/geo/parser.h"

namespace mongo {
namespace geo {

namespace coords2d {

typedef BoundingBox<Coordinates2D> BoundingBox;
typedef Geometry<Coordinates2D> Geometry;
typedef GeometryCollection<Coordinates2D> GeometryCollection;
typedef LineString<Coordinates2D> LineString;
typedef MultiLineString<Coordinates2D> MultiLineString;
typedef MultiPoint<Coordinates2D> MultiPoint;
typedef MultiPolygon<Coordinates2D> MultiPolygon;
typedef GeoObj<Coordinates2D> GeoObj;
typedef Point<Coordinates2D> Point;
typedef Polygon<Coordinates2D> Polygon;
typedef Parser<Coordinates2D> Parser;

}  // namespace coords2d

namespace coords2dgeographic {

typedef BoundingBox<Coordinates2DGeographic> BoundingBox;
typedef Geometry<Coordinates2DGeographic> Geometry;
typedef GeometryCollection<Coordinates2DGeographic> GeometryCollection;
typedef LineString<Coordinates2DGeographic> LineString;
typedef MultiLineString<Coordinates2DGeographic> MultiLineString;
typedef MultiPoint<Coordinates2DGeographic> MultiPoint;
typedef MultiPolygon<Coordinates2DGeographic> MultiPolygon;
typedef GeoObj<Coordinates2DGeographic> GeoObj;
typedef Point<Coordinates2DGeographic> Point;
typedef Polygon<Coordinates2DGeographic> Polygon;
typedef Parser<Coordinates2DGeographic> Parser;

}  // namespace coords2dgeographic

}  // namespace geo
}  // namespace mongo
