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
#include "mongo/geo/point.h"

namespace mongo {
namespace geo {

    template <typename TCoordinates>
    class LineString : public Geometry<TCoordinates> {
    public:

        /**
         * LineString constructor
         *
         * @param bson A BSON representation of the line string.
         */
        explicit LineString(const BSONObj& bson);

        /**
         * LineString constructor
         *
         * @param points The points that make up the line string.
         */
        explicit LineString(const std::vector<Point<TCoordinates> >& points);

        LineString(const LineString<TCoordinates>& other);
        LineString& operator=(LineString<TCoordinates> other);

        /**
         * Obtain a BSON representation of the line string.
         *
         * @return a BSON representation of the line string.
         */
        virtual BSONObj toBSON() const { return _bson; }

        /**
         * Obtain the bounding box surrounding this line string.
         *
         * @return A bounding box surrounding this line string.
         */
        virtual BoundingBox<TCoordinates> getBoundingBox() const;

        /**
         * Get the geometry type of this object.
         *
         * @return GeoObjType_LineString
         */
        virtual GeoObjType getType() const { return GeoObjType_LineString; }

        /**
         * Obtain the points that make up this LineString.
         *
         * @return a vector of points making up this LineString.
         */
        std::vector<Point<TCoordinates> > getPoints() const { return _points; }

    private:
        static BSONObj createBSON(const std::vector<Point<TCoordinates> >& points);

        BSONObj _bson;
        std::vector<Point<TCoordinates> > _points;
        mutable boost::scoped_ptr<BoundingBox<TCoordinates> > _boundingBox;

        /**
         * Compute the bounding box arround this LineString. Caller has ownership of the
         * returned pointer.
         *
         * @return a pointer to the bounding box of this LineString.
         */
        BoundingBox<TCoordinates>* computeBoundingBox() const;
    };

    template<typename TCoordinates>
    LineString<TCoordinates>::LineString(const BSONObj& bson)
        : _bson(GeoObj<TCoordinates>::validateType(bson, kLineStringTypeStr))
        , _points(Geometry<TCoordinates>::parseAllPoints(bson))
        , _boundingBox(Geometry<TCoordinates>::parseBoundingBox(bson)) {
    }

    template<typename TCoordinates>
    LineString<TCoordinates>::LineString(const std::vector<Point<TCoordinates> >& points)
        : _bson(createBSON(points))
        , _points(points) {
    }

    template<typename TCoordinates>
    LineString<TCoordinates>::LineString(const LineString<TCoordinates>& other)
        : _bson(other._bson)
        , _points(other._points) {
        // TODO: consider refactoring this to not make deep copies,
        // and instead use a boost::shared_ptr to share the same bounding
        // box across all copies of a Point. This would also let the
        // compiler generate copy and assignment constructors, so we can drop
        // them from the implementation.
        if (other._boundingBox)
            _boundingBox.reset(new BoundingBox<TCoordinates>(*other._boundingBox));
    }

    template<typename TCoordinates>
    LineString<TCoordinates>& LineString<TCoordinates>::operator=(LineString<TCoordinates> other) {
        using std::swap;
        swap(_bson, other._bson);
        swap(_points, other._points);
        swap(_boundingBox, other._boundingBox);
        return *this;
    }

    template<typename TCoordinates>
    BoundingBox<TCoordinates> LineString<TCoordinates>::getBoundingBox() const {
        if (!_boundingBox)
            _boundingBox.reset(computeBoundingBox());
        return *_boundingBox.get();
    }

    template<typename TCoordinates>
    BSONObj LineString<TCoordinates>::createBSON(const std::vector<Point<TCoordinates> >& points) {
        BSONArrayBuilder bab;
        for (size_t i = 0; i < points.size(); ++i)
            bab.append(points[i].toBSON()[kCoordsFieldName]);
        BSONObjBuilder bob;
        return bob.append(kTypeFieldName, kLineStringTypeStr)
                  .append(kCoordsFieldName, bab.arr())
                  .obj();
    }

    template<typename TCoordinates>
    BoundingBox<TCoordinates>* LineString<TCoordinates>::computeBoundingBox() const {
        return Geometry<TCoordinates>::computeBoundingBox(_points);
    }

} // namespace geo
} // namespace mongo
