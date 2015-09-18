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

#include "mongo/db/jsobj.h"
#include "mongo/geo/boundingbox.h"
#include "mongo/geo/constants.h"
#include "mongo/geo/geometry.h"
#include "mongo/geo/linestring.h"
#include "mongo/geo/geoobj.h"
#include "mongo/geo/point.h"

namespace mongo {
namespace geo {

template <typename TCoordinates>
class Polygon : public Geometry<TCoordinates> {
public:
    /**
     * Polygon constructor
     *
     * @param  bson A BSON representation of the polygon.
     */
    explicit Polygon(const BSONObj& bson);

    /**
     * Polygon constructor
     *
     * @param  linearRings The linear rings that define this polygon.
     * The first element represents the exterior linear ring. Any subsequent
     * elements represent interior rings (or holes).
     */
    explicit Polygon(const std::vector<LineString<TCoordinates> >& linearRings);

    Polygon(const Polygon<TCoordinates>& other);
    Polygon& operator=(Polygon<TCoordinates> other);

    /**
     * Obtain a BSON representation of the polygon.
     *
     * @return a BSON representation of the polygon.
     */
    virtual BSONObj toBSON() const {
        return _bson;
    }

    /**
     * Obtain the bounding box surrounding this Polygon.
     *
     * @return A bounding box surrounding this Polygon.
     */
    virtual BoundingBox<TCoordinates> getBoundingBox() const;

    /**
     * Get the geometry type of this object.
     *
     * @return GeoObjType_Polygon
     */
    virtual GeoObjType getType() const {
        return GeoObjType_Polygon;
    }

    /**
     * Obtain the points that make up this Polygon.
     *
     * @return a vector of points making up this Polygon.
     */
    std::vector<Point<TCoordinates> > getPoints() const;

    /**
     * Obtain the linear rings that make up this Polygon.
     *
     * @return a vector of LineStrings that define the linear rings
     * making up this Polygon.
     */
    std::vector<LineString<TCoordinates> > getLinearRings() const;

private:
    static BSONObj createBSON(const std::vector<LineString<TCoordinates> >& linearRings);
    static std::vector<LineString<TCoordinates> > parseLinearRings(const BSONObj& bson);

    BSONObj _bson;
    std::vector<LineString<TCoordinates> > _linearRings;
    mutable boost::scoped_ptr<BoundingBox<TCoordinates> > _boundingBox;

    /**
     * Compute the bounding box arround this Polygon. Caller has ownership of the
     * returned pointer.
     *
     * @return a pointer to the bounding box of this Polygon.
     */
    BoundingBox<TCoordinates>* computeBoundingBox() const;
};

template <typename TCoordinates>
Polygon<TCoordinates>::Polygon(const BSONObj& bson)
    : _bson(GeoObj<TCoordinates>::validateType(bson, kPolygonTypeStr)),
      _linearRings(parseLinearRings(bson)),
      _boundingBox(Geometry<TCoordinates>::parseBoundingBox(bson)) {}

template <typename TCoordinates>
Polygon<TCoordinates>::Polygon(const std::vector<LineString<TCoordinates> >& linearRings)
    : _bson(createBSON(linearRings)), _linearRings(linearRings) {}

template <typename TCoordinates>
Polygon<TCoordinates>::Polygon(const Polygon<TCoordinates>& other)
    : _bson(other._bson), _linearRings(other._linearRings) {
    // TODO: consider refactoring this to not make deep copies,
    // and instead use a boost::shared_ptr to share the same bounding
    // box across all copies of a Point. This would also let the
    // compiler generate copy and assignment constructors, so we can drop
    // them from the implementation.
    if (other._boundingBox)
        _boundingBox.reset(new BoundingBox<TCoordinates>(*other._boundingBox));
}

template <typename TCoordinates>
Polygon<TCoordinates>& Polygon<TCoordinates>::operator=(Polygon<TCoordinates> other) {
    using std::swap;
    swap(_bson, other._bson);
    swap(_linearRings, other._linearRings);
    swap(_boundingBox, other._boundingBox);
    return *this;
}

template <typename TCoordinates>
BoundingBox<TCoordinates> Polygon<TCoordinates>::getBoundingBox() const {
    if (!_boundingBox)
        _boundingBox.reset(computeBoundingBox());
    return *_boundingBox.get();
}

template <typename TCoordinates>
std::vector<Point<TCoordinates> > Polygon<TCoordinates>::getPoints() const {
    std::vector<Point<TCoordinates> > allPoints, lineStringPoints;
    for (size_t i = 0; i < _linearRings.size(); ++i) {
        lineStringPoints = _linearRings[i].getPoints();
        for (size_t j = 0; j < lineStringPoints.size(); ++j)
            allPoints.push_back(lineStringPoints[j]);
    }
    return allPoints;
}

template <typename TCoordinates>
std::vector<LineString<TCoordinates> > Polygon<TCoordinates>::getLinearRings() const {
    return _linearRings;
}

template <typename TCoordinates>
BSONObj Polygon<TCoordinates>::createBSON(
    const std::vector<LineString<TCoordinates> >& linearRings) {
    BSONArrayBuilder bab;
    for (size_t i = 0; i < linearRings.size(); ++i)
        bab.append(linearRings[i].toBSON()[kCoordsFieldName]);
    BSONObjBuilder bob;
    return bob.append(kTypeFieldName, kPolygonTypeStr).append(kCoordsFieldName, bab.arr()).obj();
}

template <typename TCoordinates>
std::vector<LineString<TCoordinates> > Polygon<TCoordinates>::parseLinearRings(
    const BSONObj& bson) {
    std::vector<BSONElement> linearRingElems = Geometry<TCoordinates>::getCoordsField(bson).Array();

    std::vector<LineString<TCoordinates> > linearRings;
    for (size_t i = 0; i < linearRingElems.size(); ++i) {
        linearRings.push_back(LineString<TCoordinates>(
            Geometry<TCoordinates>::parsePointArray(linearRingElems[i].Array())));
    }
    return linearRings;
}

template <typename TCoordinates>
BoundingBox<TCoordinates>* Polygon<TCoordinates>::computeBoundingBox() const {
    return Geometry<TCoordinates>::computeBoundingBox(getPoints());
}

}  // namespace geo
}  // namespace mongo
