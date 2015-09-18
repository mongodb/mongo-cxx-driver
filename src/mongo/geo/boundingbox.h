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

#include <vector>

#include "mongo/db/jsobj.h"
#include "mongo/geo/constants.h"
#include "mongo/util/assert_util.h"

namespace mongo {
namespace geo {

/**
 * Represents a bounding box.
 *
 * A bounding box describes the smallest box that can contain some
 * set of enclosed geometries. For example, the bounding box around
 * the LineString [(0, 0), (5, 0), (2, 6)] would be the rectangle with
 * coordinates [(0, 0), (5, 0), (5, 6), (0, 6)].
 *
 * A bounding box is defined by the two points on its corners with the lowest and
 * highest values for all axes. The above bounding box is represented as:
 *
 * { "bbox" : [ 0, 0, 5, 6 ] }
 *
 * The first half of the array defines the coordinates of the low point and the
 * second half defines the coordinates of the high point.
 *
 * Bounding boxes are optional members of Geometries. The BSON for
 * the above LineString, if it included a defined bounding box, would be:
 *
 * {
 *   "type" : "LineString",
 *   "coordinates" : [ [ 0, 0 ], [ 5, 0 ], [ 2, 6 ] ],
 *   "bbox" : [ 0, 0, 5, 6 ]
 * }
 *
 * Example Usage:
 *
 * BSONObj shapeBSON = cursor->next();
 * MultiLineString<Coordinates2D> mls(shapeBSON);
 *
 * // Find all geometry objects inside the bounding box of mls
 * BoundingBox<Coordinates2D> bbox = mls.getBoundingBox();
 * Query geowithinQuery = MONGO_QUERY("<field_name>" << GEOWITHIN(bbox));
 * conn.query("<db_name>.<collection_name>", geowithinQuery);
 *
 * @see http://geojson.org/geojson-spec.html#bounding-boxes
 */
template <typename TCoordinates>
class BoundingBox {
public:
    /**
     * BoundingBox constructor
     *
     * @param min The minimum point of the box
     * @param max The maximum point of the box
     *
     * Example Usage:
     *
     * Coordinates2D min(0, 0);
     * Coordinates2D max(5, 6);
     * BoundingBox<Coordinates2D> bbox(min, max);
     */
    BoundingBox(const TCoordinates& min, const TCoordinates& max);

    /**
     * BoundingBox constructor
     *
     * @param bson BSON defining a Geometry that has a member "bbox".
     *
     * Note that to use this constructor, the BSON *must* already define
     * the bounding box. If the given BSON has no "bbox" member that defines
     * the bounding box of the shape, an assertion error is raised.
     *
     * If you have BSON defining a Geometry that has no bbox member
     * and you would like to find the shape's bounding box, call that shape's
     * constructor instead and use its .getBoundingBox() method.
     *
     * Example usage:
     *
     * BSONObj bson = BSON(
     *     "type" << "LineString"
     *     "coordinates" << BSON_ARRAY(BSON_ARRAY(0 << 0) << BSON_ARRAY(5 << 6))
     *     "bbox" << BSON_ARRAY(0 << 0 << 5 << 6));
     * BoundingBox<Coordinates2D> bbox(bson);
     */
    explicit BoundingBox(const BSONObj& bson);

    /**
     * Get the minimum coordinates of this bounding box.
     *
     * @return TCoordinates that contain the minimum values
     * for each axis of this bounding box.
     */
    TCoordinates getMin() const {
        return _min;
    }

    /**
     * Get the maximum coordinates of this bounding box.
     *
     * @return TCoordinates that contain the maximum values
     * for each axis of this bounding box.
     */
    TCoordinates getMax() const {
        return _max;
    }

    /**
     * Obtain a BSON representation of this bounding box.
     *
     * Example BSON, representing minimum coordinates (0, 0) and maximum
     * coordinates (5, 6):
     *
     * { "bbox" : [ 0, 0, 5, 6 ] }
     *
     * @return A BSON representation of this bounding box.
     */
    BSONObj toBSON() const;

    /**
     * Obtain a flat BSONArray representation of the coordinate values
     * defined by this bounding box.
     *
     * Example BSONArray, representing minimum coordinates (0, 0) and maximum
     * coordinates (5, 6):
     *
     * [ 0, 0, 5, 6 ]
     *
     * @return A flat BSONArray representation of the coordinate values.
     */
    BSONArray toBSONArray() const;

    /**
     * Obtain a nested BSONArray representation of the coordinate values
     * defined by this bounding box.
     *
     * Example BSONArray, representing minimum coordinates (0, 0) and maximum
     * coordinates (5, 6):
     *
     * [ [ 0, 0 ], [ 5, 6 ] ]
     *
     * @return A nested BSONArray representation of the coordinate values.
     */
    BSONArray toNestedBSONArray() const;

private:
    TCoordinates _min;
    TCoordinates _max;
};

template <typename TCoordinates>
BoundingBox<TCoordinates>::BoundingBox(const TCoordinates& min, const TCoordinates& max)
    : _min(min), _max(max) {}

template <typename TCoordinates>
BoundingBox<TCoordinates>::BoundingBox(const BSONObj& bson) {
    BSONElement bbox = bson.getField(kBoundingBoxFieldName);

    // Ensure bson defines a bounding box.
    uassert(0, "bson argument to BoundingBox ctor must define the field \"bbox\"", !bbox.eoo());

    std::vector<BSONElement> bboxCoords = bbox.Array();

    // The GeoJSON spec dictates that bboxCoords is an array of
    // length 2*n, where n is the number of dimensions represented
    // in the contained geometries.
    const size_t n = TCoordinates::dimensionality();
    uassert(0,
            "bbox field must have exactly 2 * n elements, where n is the number of dimensions "
            "in the coordinate system",
            bboxCoords.size() == 2 * n);

    // Construct _min with the first n elements and _max with the second n elements.
    std::vector<double> minCoords, maxCoords;
    for (size_t i = 0; i < n; ++i) {
        minCoords.push_back(bboxCoords[i].Double());
        maxCoords.push_back(bboxCoords[n + i].Double());
    }
    _min = TCoordinates(minCoords);
    _max = TCoordinates(maxCoords);
}

template <typename TCoordinates>
BSONObj BoundingBox<TCoordinates>::toBSON() const {
    return BSON(kBoundingBoxFieldName << toBSONArray());
}

template <typename TCoordinates>
BSONArray BoundingBox<TCoordinates>::toBSONArray() const {
    BSONArrayBuilder bab;
    std::vector<double> minCoords = _min.getValues();
    std::vector<double> maxCoords = _max.getValues();
    for (size_t i = 0; i < minCoords.size(); ++i)
        bab.append(minCoords[i]);
    for (size_t i = 0; i < maxCoords.size(); ++i)
        bab.append(maxCoords[i]);
    return bab.arr();
}

template <typename TCoordinates>
BSONArray BoundingBox<TCoordinates>::toNestedBSONArray() const {
    BSONArrayBuilder minBab, maxBab;
    std::vector<double> minCoords = _min.getValues();
    std::vector<double> maxCoords = _max.getValues();
    for (size_t i = 0; i < minCoords.size(); ++i)
        minBab.append(minCoords[i]);
    for (size_t i = 0; i < maxCoords.size(); ++i)
        maxBab.append(maxCoords[i]);
    return BSON_ARRAY(minBab.arr() << maxBab.arr());
}

}  // namespace geo
}  // namespace mongo
