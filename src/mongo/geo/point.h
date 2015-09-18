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

/** @file Represents a Point geometry.
 *
 * Example BSON structure:
 *
 * {
 *    type : "Point",
 *    coordinates : [ 1, 2 ]
 * }
 *
 */

#pragma once

#include <algorithm>
#include <boost/scoped_ptr.hpp>

#include "mongo/client/export_macros.h"
#include "mongo/db/jsobj.h"
#include "mongo/geo/boundingbox.h"
#include "mongo/geo/constants.h"
#include "mongo/geo/geometry.h"

namespace mongo {
namespace geo {

/**
 * Represents a Point.
 *
 * Example Usage:
 *
 * Coordinates2D coords(1, 2);
 * Point<Coordinates2D> point(coords);
 * Query nearPointQuery = MONGO_QUERY("loc" << NEAR(point));
 * conn.query("foo.bar", nearPointQuery);
 */
template <typename TCoordinates>
class Point : public Geometry<TCoordinates> {
public:
    /**
     * Point constructor
     *
     * @param bson A BSON representation of the point.
     *
     * Example Usage:
     *
     * Point<Coordinates2D> point(BSON(
     *     "type" << "Point" <<
     *     "coordinates" << BSON_ARRAY(1 << 2)));
     */
    explicit Point(const BSONObj& bson);

    /**
     * Point constructor
     *
     * @param coords The coordinates of the point.
     *
     * Example Usage:
     *
     * Coordinates2D coords(1, 2);
     * Point<Coordinates2D> point(coords);
     */
    explicit Point(const TCoordinates& coords);

    Point(const Point<TCoordinates>& other);
    Point& operator=(Point<TCoordinates> other);

    /**
     * Obtain a BSON representation of this point.
     *
     * @return a BSON representation of this point.
     *
     * Example Usage:
     *
     * Coordinates2D coords(1, 2);
     * Point<Coordinates2D> point(coords);
     * BSONObj bson = point.toBSON();
     *
     * std::cout << bson.jsonString() << std::endl;
     * { "type" : "Point", "coordinates" : [ 1, 2 ] }
     */
    virtual BSONObj toBSON() const {
        return _bson;
    }

    /**
     * Get the bounding box of this point. The bounding box of any
     * single Point is just the point itself.
     *
     * @return A bounding box with minimum and maximum coordinates
     * equal to the coordinates of this point.
     *
     * Example Usage:
     *
     * Coordinates2D coords(1, 2);
     * Point<Coordinates2D> point(coords);
     * BoundingBox<Coordinates2D> bbox = point.getBoundingBox();
     */
    virtual BoundingBox<TCoordinates> getBoundingBox() const;

    /**
     * Get the geometry type of this object. Useful
     * if you have a generic GeoObj<TCoordinates>* and you
     * want to determine what specific type it is.
     *
     * @see Parser
     *
     * @return GeoObjType_Point
     *
     * Example Usage:
     *
     * BSONObj unknownGeoObject = cursor->next();
     * GeoObj<Coordinates2D>* obj = Parser<Coordinates2D>::parse(unknownGeoObject);
     * if (obj->getType() == GeoObjType_Point) {
     *     Point<Coordinates2D>* pointPtr = static_cast< Point<Coordinates2D>* >obj;
     *     // do things with pointPtr...
     * } else if ...
     */
    virtual GeoObjType getType() const {
        return GeoObjType_Point;
    }

    /**
     * Get the coordinates of this point.
     *
     * @return The coordinates of this point.
     *
     * Example Usage:
     *
     * Point<Coordinates2D> point(BSON(
     *     "type" << "Point" <<
     *     "coordinates" << BSON_ARRAY(1 << 2)));
     * Coordinates2D coords = point.getCoordinates();
     * std::cout << coords.toBSON().jsonString() << std::endl;
     * { "coordinates": [ 1, 2 ] }
     */
    TCoordinates getCoordinates() const {
        return _coords;
    }

    /**
     * Get the position of this point in the given dimension.
     *
     * @param dimension The coordinate dimension sought.
     *
     * @return The value of this point's position in the given dimension.
     *
     * Example Usage:
     *
     * Coordinates2D coords(1, 2);
     * Point<Coordinates2D> point(coords);
     * for (size_t i = 0; i < coords::dimensionality(); ++i)
     *     std::cout << point[i] << " ";
     * "1 2 "
     */
    double operator[](size_t dimension) const {
        return _coords[dimension];
    }

private:
    static BSONObj createBSON(const TCoordinates& coords);

    BSONObj _bson;
    TCoordinates _coords;
    mutable boost::scoped_ptr<BoundingBox<TCoordinates> > _boundingBox;

    BoundingBox<TCoordinates>* computeBoundingBox() const;
};

template <typename TCoordinates>
Point<TCoordinates>::Point(const BSONObj& bson)
    : _bson(GeoObj<TCoordinates>::validateType(bson, kPointTypeStr)),
      _coords(Geometry<TCoordinates>::parseCoords(bson)),
      _boundingBox(Geometry<TCoordinates>::parseBoundingBox(bson)) {}

template <typename TCoordinates>
Point<TCoordinates>::Point(const TCoordinates& coords)
    : _bson(createBSON(coords)), _coords(coords) {}

template <typename TCoordinates>
Point<TCoordinates>::Point(const Point<TCoordinates>& other)
    : _bson(other._bson), _coords(other._coords) {
    // TODO: consider refactoring this to not make deep copies,
    // and instead use a boost::shared_ptr to share the same bounding
    // box across all copies of a Point. This would also let the
    // compiler generate copy and assignment constructors, so we can drop
    // them from the implementation.
    if (other._boundingBox)
        _boundingBox.reset(new BoundingBox<TCoordinates>(*other._boundingBox));
}

template <typename TCoordinates>
Point<TCoordinates>& Point<TCoordinates>::operator=(Point<TCoordinates> other) {
    using std::swap;
    swap(_bson, other._bson);
    swap(_coords, other._coords);
    swap(_boundingBox, other._boundingBox);
    return *this;
}

template <typename TCoordinates>
BoundingBox<TCoordinates> Point<TCoordinates>::getBoundingBox() const {
    if (!_boundingBox)
        _boundingBox.reset(computeBoundingBox());
    return *_boundingBox.get();
}

template <typename TCoordinates>
BSONObj Point<TCoordinates>::createBSON(const TCoordinates& coords) {
    BSONObjBuilder bob;
    return bob.append(kTypeFieldName, kPointTypeStr).appendElements(coords.toBSON()).obj();
}

template <typename TCoordinates>
BoundingBox<TCoordinates>* Point<TCoordinates>::computeBoundingBox() const {
    return new BoundingBox<TCoordinates>(_coords, _coords);
}

}  // namespace geo
}  // namespace mongo
