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

namespace mongo {
namespace geo {

template <typename TCoordinates>
class MultiLineString : public Geometry<TCoordinates> {
public:
    /**
     * MultiLineString constructor
     *
     * @param  bson A BSON representation of this MultiLineString.
     */
    explicit MultiLineString(const BSONObj& bson);

    /**
     * MultiLineString constructor
     *
     * @param  lineStrings the LineStrings that collectively make up this MultiLineString.
     */
    explicit MultiLineString(const std::vector<LineString<TCoordinates> >& lineStrings);

    /**
     * Obtain a BSON representation of the MultiLineString.
     *
     * @return a BSON representation of the MultiLineString.
     */
    virtual BSONObj toBSON() const {
        return _bson;
    }

    /**
     * Obtain the bounding box surrounding this MultiLineString.
     *
     * @return A bounding box surrounding this MultiLineString.
     */
    virtual BoundingBox<TCoordinates> getBoundingBox() const;

    /**
     * Get the geometry type of this object.
     *
     * @return GeoObjType_MultiLineString
     */
    virtual GeoObjType getType() const {
        return GeoObjType_MultiLineString;
    }

    /**
     * Obtain the points that make up this MultiLineString.
     *
     * @return a vector of points making up this MultiLineString.
     */
    std::vector<Point<TCoordinates> > getPoints() const;

    /**
     * Obtain the line strings that make up this MultiLineString.
     *
     * @return a vector of LineStrings making up this MultiLineString.
     */
    std::vector<LineString<TCoordinates> > getLineStrings() const;

private:
    static BSONObj createBSON(const std::vector<LineString<TCoordinates> >& lineStrings);
    static std::vector<LineString<TCoordinates> > parseLineStrings(const BSONObj& bson);

    BSONObj _bson;
    std::vector<LineString<TCoordinates> > _lineStrings;
    mutable boost::scoped_ptr<BoundingBox<TCoordinates> > _boundingBox;

    /**
     * Compute the bounding box arround this MultiLineString. Caller has ownership of the
     * returned pointer.
     *
     * @return a pointer to the bounding box of this MultiLineString.
     */
    BoundingBox<TCoordinates>* computeBoundingBox() const;
};

template <typename TCoordinates>
MultiLineString<TCoordinates>::MultiLineString(const BSONObj& bson)
    : _bson(GeoObj<TCoordinates>::validateType(bson, kMultiLineStringTypeStr)),
      _lineStrings(parseLineStrings(bson)),
      _boundingBox(Geometry<TCoordinates>::parseBoundingBox(bson)) {}

template <typename TCoordinates>
MultiLineString<TCoordinates>::MultiLineString(
    const std::vector<LineString<TCoordinates> >& lineStrings)
    : _bson(createBSON(lineStrings)), _lineStrings(lineStrings) {}

template <typename TCoordinates>
BoundingBox<TCoordinates> MultiLineString<TCoordinates>::getBoundingBox() const {
    if (!_boundingBox)
        _boundingBox.reset(computeBoundingBox());
    return *_boundingBox.get();
}

template <typename TCoordinates>
std::vector<Point<TCoordinates> > MultiLineString<TCoordinates>::getPoints() const {
    std::vector<Point<TCoordinates> > points, curLineStringPoints;
    for (size_t i = 0; i < _lineStrings.size(); ++i) {
        curLineStringPoints = _lineStrings[i].getPoints();
        points.insert(points.end(), curLineStringPoints.begin(), curLineStringPoints.end());
    }
    return points;
}

template <typename TCoordinates>
std::vector<LineString<TCoordinates> > MultiLineString<TCoordinates>::getLineStrings() const {
    return _lineStrings;
}

template <typename TCoordinates>
BSONObj MultiLineString<TCoordinates>::createBSON(
    const std::vector<LineString<TCoordinates> >& lineStrings) {
    BSONArrayBuilder bab;
    for (size_t i = 0; i < lineStrings.size(); ++i)
        bab.append(lineStrings[i].toBSON()[kCoordsFieldName]);
    BSONObjBuilder bob;
    return bob.append(kTypeFieldName, kMultiLineStringTypeStr)
        .append(kCoordsFieldName, bab.arr())
        .obj();
}

template <typename TCoordinates>
std::vector<LineString<TCoordinates> > MultiLineString<TCoordinates>::parseLineStrings(
    const BSONObj& bson) {
    std::vector<BSONElement> lineStringArr = Geometry<TCoordinates>::getCoordsField(bson).Array();

    std::vector<LineString<TCoordinates> > lineStrings;
    for (size_t i = 0; i < lineStringArr.size(); ++i) {
        LineString<TCoordinates> line(
            Geometry<TCoordinates>::parsePointArray(lineStringArr[i].Array()));
        lineStrings.push_back(line);
    }
    return lineStrings;
}

template <typename TCoordinates>
BoundingBox<TCoordinates>* MultiLineString<TCoordinates>::computeBoundingBox() const {
    return Geometry<TCoordinates>::computeBoundingBox(getPoints());
}

}  // namespace geo
}  // namespace mongo
