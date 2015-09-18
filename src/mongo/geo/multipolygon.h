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

#include <boost/scoped_ptr.hpp>
#include <vector>

#include "mongo/client/export_macros.h"
#include "mongo/db/jsobj.h"
#include "mongo/geo/boundingbox.h"
#include "mongo/geo/constants.h"
#include "mongo/geo/geometry.h"
#include "mongo/geo/geoobj.h"
#include "mongo/geo/point.h"
#include "mongo/geo/polygon.h"

namespace mongo {
namespace geo {

template <typename TCoordinates>
class MultiPolygon : public Geometry<TCoordinates> {
public:
    /**
     * MultiPolygon constructor
     *
     * @param  bson A BSON representation of this MultiPolygon.
     */
    explicit MultiPolygon(const BSONObj& bson);

    /**
     * Obtain a BSON representation of the MultiPolygon.
     *
     * @return a BSON representation of the MultiPolygon.
     */
    virtual BSONObj toBSON() const {
        return _bson;
    }

    /**
     * Obtain the bounding box surrounding this MultiPolygon.
     *
     * @return A bounding box surrounding this MultiPolygon.
     */
    virtual BoundingBox<TCoordinates> getBoundingBox() const;

    /**
     * Get the geometry type of this object.
     *
     * @return GeoObjType_MultiPolygon
     */
    virtual GeoObjType getType() const {
        return GeoObjType_MultiPolygon;
    }

    /**
     * Obtain the points that make up this MultiPolygon.
     *
     * @return a vector of points making up this MultiPolygon.
     */
    std::vector<Point<TCoordinates> > getPoints() const;

    /**
     * Obtain the Polygons that make up this MultiPolygon.
     *
     * @return a vector of Polygons making up this MultiPolygon.
     */
    std::vector<Polygon<TCoordinates> > getPolygons() const {
        return _polygons;
    }

private:
    static Polygon<TCoordinates> parsePolygon(const BSONElement& polygon);
    static std::vector<Polygon<TCoordinates> > parseAllPolygons(const BSONObj& bson);

    BSONObj _bson;
    std::vector<Polygon<TCoordinates> > _polygons;
    mutable boost::scoped_ptr<BoundingBox<TCoordinates> > _boundingBox;

    /**
     * Compute the bounding box arround this MultiPolygon. Caller has ownership of the
     * returned pointer.
     *
     * @return a pointer to the bounding box of this MultiPolygon.
     */
    BoundingBox<TCoordinates>* computeBoundingBox() const;
};

template <typename TCoordinates>
MultiPolygon<TCoordinates>::MultiPolygon(const BSONObj& bson)
    : _bson(GeoObj<TCoordinates>::validateType(bson, kMultiPolygonTypeStr)),
      _polygons(parseAllPolygons(bson)),
      _boundingBox(Geometry<TCoordinates>::parseBoundingBox(bson)) {}

template <typename TCoordinates>
BoundingBox<TCoordinates> MultiPolygon<TCoordinates>::getBoundingBox() const {
    if (!_boundingBox)
        _boundingBox.reset(computeBoundingBox());
    return *_boundingBox.get();
}

template <typename TCoordinates>
std::vector<Point<TCoordinates> > MultiPolygon<TCoordinates>::getPoints() const {
    std::vector<Point<TCoordinates> > points, curPolygonPoints;
    for (size_t i = 0; i < _polygons.size(); ++i) {
        curPolygonPoints = _polygons[i].getPoints();
        points.insert(points.end(), curPolygonPoints.begin(), curPolygonPoints.end());
    }
    return points;
}

template <typename TCoordinates>
Polygon<TCoordinates> MultiPolygon<TCoordinates>::parsePolygon(const BSONElement& polygon) {
    std::vector<BSONElement> linearRingElems = polygon.Array();
    std::vector<LineString<TCoordinates> > linearRings;
    for (size_t i = 0; i < linearRingElems.size(); ++i) {
        linearRings.push_back(LineString<TCoordinates>(
            Geometry<TCoordinates>::parsePointArray(linearRingElems[i].Array())));
    }
    return Polygon<TCoordinates>(linearRings);
}

template <typename TCoordinates>
std::vector<Polygon<TCoordinates> > MultiPolygon<TCoordinates>::parseAllPolygons(
    const BSONObj& bson) {
    std::vector<BSONElement> polygonArr = Geometry<TCoordinates>::getCoordsField(bson).Array();
    std::vector<Polygon<TCoordinates> > polygons;
    for (size_t i = 0; i < polygonArr.size(); ++i) {
        polygons.push_back(parsePolygon(polygonArr[i]));
    }
    return polygons;
}

template <typename TCoordinates>
BoundingBox<TCoordinates>* MultiPolygon<TCoordinates>::computeBoundingBox() const {
    return Geometry<TCoordinates>::computeBoundingBox(getPoints());
}

}  // namespace geo
}  // namespace mongo
