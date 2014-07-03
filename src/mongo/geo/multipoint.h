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
#include "mongo/geo/geoobj.h"
#include "mongo/geo/point.h"

namespace mongo {
namespace geo {

    template <typename TCoordinates>
    class MultiPoint : public Geometry<TCoordinates> {
    public:

        /**
         * MultiPoint constructor
         *
         * @param  bson A BSON representation of this MultiPoint.
         */
        explicit MultiPoint(const BSONObj& bson);

        /**
         * Obtain a BSON representation of the MultiPoint.
         *
         * @return a BSON representation of the MultiPoint.
         */
        virtual BSONObj toBSON() const { return _bson; }

        /**
         * Obtain the bounding box surrounding this MultiPoint.
         *
         * @return A bounding box surrounding this MultiPoint.
         */
        virtual BoundingBox<TCoordinates> getBoundingBox() const;

        /**
         * Get the geometry type of this object.
         *
         * @return GeoObjType_MultiPoint
         */
        virtual GeoObjType getType() const { return GeoObjType_MultiPoint; }

        /**
         * Obtain the points that make up this MultiPoint.
         *
         * @return a vector of points making up this MultiPoint.
         */
        std::vector<Point<TCoordinates> > getPoints() const { return _points; }

    private:
        BSONObj _bson;
        std::vector<Point<TCoordinates> > _points;
        mutable boost::scoped_ptr<BoundingBox<TCoordinates> > _boundingBox;

        /**
         * Compute the bounding box arround this MultiPoint. Caller has ownership of the
         * returned pointer.
         *
         * @return a pointer to the bounding box of this MultiPoint.
         */
        BoundingBox<TCoordinates>* computeBoundingBox() const;
    };

    template<typename TCoordinates>
    MultiPoint<TCoordinates>::MultiPoint(const BSONObj& bson)
        : _bson(GeoObj<TCoordinates>::validateType(bson, kMultiPointTypeStr))
        , _points(Geometry<TCoordinates>::parseAllPoints(bson))
        , _boundingBox(Geometry<TCoordinates>::parseBoundingBox(bson)) {
    }

    template<typename TCoordinates>
    BoundingBox<TCoordinates> MultiPoint<TCoordinates>::getBoundingBox() const {
        if (!_boundingBox)
            _boundingBox.reset(computeBoundingBox());
        return *_boundingBox.get();
    }

    template<typename TCoordinates>
    BoundingBox<TCoordinates>* MultiPoint<TCoordinates>::computeBoundingBox() const {
        return Geometry<TCoordinates>::computeBoundingBox(_points);
    }

} // namespace geo
} // namespace mongo
