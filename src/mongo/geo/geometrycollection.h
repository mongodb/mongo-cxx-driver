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
#include "mongo/geo/parser.h"
#include "mongo/util/assert_util.h"

namespace mongo {
namespace geo {

// forward declaration needed to parse arbitrary geometries
template <typename TCoordinates>
class Parser;

template <typename TCoordinates>
class GeometryCollection : public Geometry<TCoordinates> {
public:
    /**
     * GeometryCollection constructor
     *
     * @param  bson A BSON representation of the geometry collection.
     */
    explicit GeometryCollection(const BSONObj& bson);

    /**
     * Frees the heap memory for each geometry stored in this collection.
     */
    ~GeometryCollection();

    /**
     * Obtain a BSON representation of this geometry collection.
     *
     * Example structure:
     *
     * {
     *   "type" : "GeometryCollection",
     *   "geometries" : [
     *     {
     *       "type": "Point",
     *       "coordinates": [ 100.0, 0.0 ]
     *     },
     *     {
     *       "type": "LineString",
     *       "coordinates": [ [ 101.0, 0.0 ], [ 102.0, 1.0 ] ]
     *     }
     *   ]
     * }
     *
     * @return a BSON representation of this geometry collection.
     */
    virtual BSONObj toBSON() const {
        return _bson;
    }

    /**
     * Obtain the bounding box surrounding the set of geometries in this
     * geometry collection.
     *
     * @return A bounding box containing every geometry in this collection.
     */
    virtual BoundingBox<TCoordinates> getBoundingBox() const;

    /**
     * Get the geometry type of this object.
     *
     * @return GeoObjType_GeometryCollection
     */
    virtual GeoObjType getType() const {
        return GeoObjType_GeometryCollection;
    }

    /**
     * Get a vector of pointers to the geometries contained in this geometry collection.
     * Each GeoObj has been instantiated as the appropriate subclass of GeoObj. To determine
     * the specific type of each pointed-to object, call ->getType() on the pointer.
     *
     * This GeometryCollection object has ownership of the const GeoObj*'s returned.
     *
     * @return a vector of const pointers to the geometries in this collection.
     */
    const std::vector<const GeoObj<TCoordinates>*>& getGeometries() const;

private:
    /**
     * Parse the geometries defined in a geometry collection that is represented in BSON.
     * Caller has ownership of the returned pointers.
     *
     * @return a vector of const pointers to each geometry in defined in the collection.
     */
    static std::vector<const GeoObj<TCoordinates>*> parseGeometries(const BSONObj& bson);

    BSONObj _bson;
    std::vector<const GeoObj<TCoordinates>*> _geometries;
    mutable boost::scoped_ptr<BoundingBox<TCoordinates> > _boundingBox;

    /**
     * Compute the bounding box arround this GeometryCollection. Caller has ownership of the
     * returned pointer.
     *
     * @return a pointer to the bounding box of this GeometryCollection.
     */
    BoundingBox<TCoordinates>* computeBoundingBox() const;
};

template <typename TCoordinates>
GeometryCollection<TCoordinates>::GeometryCollection(const BSONObj& bson)
    : _bson(GeoObj<TCoordinates>::validateType(bson, kGeometryCollectionTypeStr)),
      _geometries(parseGeometries(bson)),
      _boundingBox(Geometry<TCoordinates>::parseBoundingBox(bson)) {}

template <typename TCoordinates>
GeometryCollection<TCoordinates>::~GeometryCollection() {
    for (size_t i = 0; i < _geometries.size(); ++i)
        delete _geometries[i];
}

template <typename TCoordinates>
BoundingBox<TCoordinates> GeometryCollection<TCoordinates>::getBoundingBox() const {
    if (!_boundingBox)
        _boundingBox.reset(computeBoundingBox());
    return *_boundingBox.get();
}

template <typename TCoordinates>
const std::vector<const GeoObj<TCoordinates>*>& GeometryCollection<TCoordinates>::getGeometries()
    const {
    return _geometries;
}

template <typename TCoordinates>
std::vector<const GeoObj<TCoordinates>*> GeometryCollection<TCoordinates>::parseGeometries(
    const BSONObj& bson) {
    BSONElement geometriesField = bson.getField(kGeometriesFieldName);
    uassert(0,
            "bson must contain a field \"geometries\" of type Array",
            !geometriesField.eoo() && geometriesField.type() == Array);

    std::vector<BSONElement> geometriesArr = geometriesField.Array();
    std::vector<const GeoObj<TCoordinates>*> geometries;
    for (size_t i = 0; i < geometriesArr.size(); ++i) {
        geometries.push_back(Parser<TCoordinates>::parse(geometriesArr[i].Obj()));
    }
    return geometries;
}

template <typename TCoordinates>
BoundingBox<TCoordinates>* GeometryCollection<TCoordinates>::computeBoundingBox() const {
    std::vector<BoundingBox<TCoordinates> > bboxes;
    for (size_t i = 0; i < _geometries.size(); ++i)
        bboxes.push_back(_geometries[i]->getBoundingBox());
    return Geometry<TCoordinates>::computeBoundingBox(bboxes);
}

}  // namespace geo
}  // namespace mongo
