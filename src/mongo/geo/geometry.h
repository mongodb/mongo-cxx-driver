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

#include <algorithm>
#include <limits>
#include <vector>

#include "mongo/client/export_macros.h"
#include "mongo/db/jsobj.h"
#include "mongo/geo/boundingbox.h"
#include "mongo/geo/constants.h"
#include "mongo/geo/geoobj.h"
#include "mongo/util/assert_util.h"

namespace mongo {
namespace geo {

// forward declaration needed for Geometry's static helpers
template <typename TCoordinates>
class Point;

template <typename TCoordinates>
class Geometry : public GeoObj<TCoordinates> {
protected:
    static BSONElement getCoordsField(const BSONObj& bson);
    static std::vector<double> parseCoords(const BSONElement& coordArr);
    static std::vector<double> parseCoords(const BSONObj& bson);
    static Point<TCoordinates> parsePoint(const BSONElement& coordArr);
    static std::vector<Point<TCoordinates> > parsePointArray(
        const std::vector<BSONElement>& pointArr);
    static std::vector<Point<TCoordinates> > parseAllPoints(const BSONObj& bson);

    /**
     * Compute the bounding box around the given points. Caller has ownership of
     * the returned pointer.
     *
     * @param  points The points that the computed bounding box will surround.
     *
     * @return A pointer to a bounding box around the given points.
     */
    static BoundingBox<TCoordinates>* computeBoundingBox(
        const std::vector<Point<TCoordinates> >& points);

    /**
     * Compute the smallest bounding box that contains the given bounding boxes.
     * This can also be thought of as the one large bounding box around the union
     * of points contained in the several smaller bounding boxes passed to this function.
     * Caller had ownership of the returned pointer.
     *
     * @param  bboxes the bounding boxes that the computed bounding box will surround.
     *
     * @return A pointer to a bounding box around the given bounding boxes.
     */
    static BoundingBox<TCoordinates>* computeBoundingBox(
        const std::vector<BoundingBox<TCoordinates> >& bboxes);

    /**
     * Parses the bounding box defined by the given geometry shape, represented
     * in BSON. Caller had ownership of the returned pointer.
     *
     * @param  bson The BSON for the geometry shape.
     *
     * @return If the shape defines its own bounding box with the "bbox" field,
     * returns a pointer to an instantiated bounding box around the shape.
     * Otherwise returns NULL.
     */
    static BoundingBox<TCoordinates>* parseBoundingBox(const BSONObj& bson);

private:
    static void findMinAndMaxCoordinatesOfDimension(const std::vector<Point<TCoordinates> >& points,
                                                    size_t dimension,
                                                    double* min,
                                                    double* max);
};

template <typename TCoordinates>
BSONElement Geometry<TCoordinates>::getCoordsField(const BSONObj& bson) {
    BSONElement coordsField = bson.getField(kCoordsFieldName);
    uassert(0,
            "bson must contain a field \"coordinates\" of type Array",
            !coordsField.eoo() && coordsField.type() == Array);
    return coordsField;
}

template <typename TCoordinates>
std::vector<double> Geometry<TCoordinates>::parseCoords(const BSONElement& coordArr) {
    std::vector<BSONElement> coordElems = coordArr.Array();
    std::vector<double> coords;
    for (size_t i = 0; i < coordElems.size(); ++i)
        coords.push_back(coordElems[i].Double());
    return coords;
}

template <typename TCoordinates>
std::vector<double> Geometry<TCoordinates>::parseCoords(const BSONObj& bson) {
    return parseCoords(getCoordsField(bson));
}

template <typename TCoordinates>
Point<TCoordinates> Geometry<TCoordinates>::parsePoint(const BSONElement& coordArr) {
    TCoordinates pointCoords(parseCoords(coordArr));
    return Point<TCoordinates>(pointCoords);
}

template <typename TCoordinates>
std::vector<Point<TCoordinates> > Geometry<TCoordinates>::parsePointArray(
    const std::vector<BSONElement>& pointArr) {
    std::vector<Point<TCoordinates> > points;
    for (size_t i = 0; i < pointArr.size(); ++i) {
        points.push_back(parsePoint(pointArr[i]));
    }
    return points;
}

template <typename TCoordinates>
std::vector<Point<TCoordinates> > Geometry<TCoordinates>::parseAllPoints(const BSONObj& bson) {
    return parsePointArray(getCoordsField(bson).Array());
}

template <typename TCoordinates>
BoundingBox<TCoordinates>* Geometry<TCoordinates>::computeBoundingBox(
    const std::vector<Point<TCoordinates> >& points) {
    // For a TCoordinates type with dimensions d1, d2, ..., dn,
    // the bounding box of these points will have min coordinates
    // whose d1 value is the minimum of all d1-axis values in points,
    // whose d2 value is the minimum of all d2-axis values in points,
    // and so on up through dn. Similarly for the max coordinates.
    //
    // So to compute the bounding box, we iterate through each dimension
    // and find the min / max values in points for that dimension.
    std::vector<double> minCoordComponents, maxCoordComponents;
    for (size_t i = 0; i < TCoordinates::dimensionality(); ++i) {
        double min, max;
        findMinAndMaxCoordinatesOfDimension(points, i, &min, &max);
        minCoordComponents.push_back(min);
        maxCoordComponents.push_back(max);
    }
    TCoordinates minCoords(minCoordComponents);
    TCoordinates maxCoords(maxCoordComponents);
    return new BoundingBox<TCoordinates>(minCoords, maxCoords);
}

template <typename TCoordinates>
void Geometry<TCoordinates>::findMinAndMaxCoordinatesOfDimension(
    const std::vector<Point<TCoordinates> >& points, size_t dimension, double* min, double* max) {
    // Initialize min and max to the positive and negative double values
    // farthest from 0, respectively. Note that we initialize max to
    // -::max(). Initializing to ::min() is incorrect because it returns
    // the smallest positive normalized double, which is still greater than
    // every negative double (and coordinates can have negative values).
    *min = std::numeric_limits<double>::max();
    *max = -std::numeric_limits<double>::max();

    // Update min and max with the smallest and largest values for the given dimension.
    for (size_t i = 0; i < points.size(); ++i) {
        if (points[i][dimension] < *min)
            *min = points[i][dimension];
        if (points[i][dimension] > *max)
            *max = points[i][dimension];
    }
}

template <typename TCoordinates>
BoundingBox<TCoordinates>* Geometry<TCoordinates>::computeBoundingBox(
    const std::vector<BoundingBox<TCoordinates> >& bboxes) {
    if (bboxes.empty())
        return NULL;

    std::vector<double> minCoords = bboxes[0].getMin().getValues();
    std::vector<double> maxCoords = bboxes[0].getMax().getValues();
    for (size_t i = 1; i < bboxes.size(); ++i) {
        std::vector<double> curMin = bboxes[i].getMin().getValues();
        std::vector<double> curMax = bboxes[i].getMax().getValues();
        for (size_t j = 0; j < curMin.size(); ++j) {
            minCoords[j] = std::min(minCoords[j], curMin[j]);
            maxCoords[j] = std::max(maxCoords[j], curMax[j]);
        }
    }

    TCoordinates globalMin(minCoords), globalMax(maxCoords);
    return new BoundingBox<TCoordinates>(globalMin, globalMax);
}

template <typename TCoordinates>
BoundingBox<TCoordinates>* Geometry<TCoordinates>::parseBoundingBox(const BSONObj& bson) {
    if (bson.hasField(kBoundingBoxFieldName))
        return new BoundingBox<TCoordinates>(bson);
    return NULL;
}

}  // namespace geo
}  // namespace mongo
