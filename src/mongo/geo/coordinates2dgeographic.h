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

#include <utility>
#include <vector>

#include "mongo/client/export_macros.h"
#include "mongo/db/jsobj.h"
#include "mongo/geo/coordinates.h"

namespace mongo {
namespace geo {

/**
 * Represents a 2D geographic position (longitude, latitude).
 *
 * Example Usage:
 *
 * Coordinates2DGeographic coords(1, 2);
 * Point<Coordinates2DGeographic> point(coords);
 * Query nearQuery = QUERY("<field_name>" << NEAR(point));
 * conn.query("<db_name>.<collection_name>", nearQuery);
 */
class MONGO_CLIENT_API Coordinates2DGeographic : public Coordinates {
public:
    Coordinates2DGeographic() : _longitude(0), _latitude(0) {}

    /**
     * Coordinates2DGeographic constructor
     *
     * @param coords The coordinate values.
     *
     * @pre coords.size() == Coordinates2DGeographic::dimensionality().
     */
    explicit Coordinates2DGeographic(const std::vector<double>& coords);

    /**
     * Coordinates2DGeographic constructor
     *
     * @param coords The coordinate values.
     */
    explicit Coordinates2DGeographic(const std::pair<double, double>& coords);

    /**
     * Coordinates2DGeographic constructor
     *
     * @param longitude The longitude value.
     * @param latitude The latitude value.
     */
    Coordinates2DGeographic(double longitude, double latitude);

    /**
     * Get the longitude.
     *
     * @return double The longitude value.
     */
    double getLongitude() const {
        return _longitude;
    }

    /**
     * Get the latitude.
     *
     * @return double The latitude value.
     */
    double getLatitude() const {
        return _latitude;
    }

    /**
     * Get the longitude and latitude values as a vector.
     *
     * @return std::vector<double> A vector [longitude, latitude] of this object's
     * coordinate values.
     */
    virtual std::vector<double> getValues() const;

    /**
     * Get the longitude and latitude values as a pair.
     *
     * @return std::pair<double, double> A pair (longitude, latitude) of this
     * object's coordinate values.
     */
    virtual std::pair<double, double> getValuesAsPair() const;

    /**
     * Get a BSON representation of the coordinates.
     *
     * @return BSONObj A BSONObj of the coordinates, structured as:
     * { "coordinates" : [ longitude, latitude ] }
     */
    virtual BSONObj toBSON() const;

    /**
     * Get the value of this coordinate at the given dimension.
     *
     * @param dimension 0 -> longitude, 1 -> latitude, any other input
     * besides 0 or 1 is an error.
     *
     * @return The coordinate value at the given dimension.
     *
     * @pre 0 <= dimension < Coordinates2DGeographic::dimensionality()
     */
    virtual double operator[](size_t dimension) const;

    /**
     * Get the dimensionality of this coordinate type.
     *
     * @return 2, the number of dimensions in Coordinates2DGeographic.
     */
    static size_t MONGO_CLIENT_FUNC dimensionality() {
        return 2;
    }

private:
    double _longitude;
    double _latitude;
};

}  // namespace geo
}  // namespace mongo
