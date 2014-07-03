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
     * Represents a 2D position (x, y).
     *
     * Example Usage:
     *
     * Coordinates2D coords(1, 2);
     * Point<Coordinates2D> point(coords);
     * Query nearQuery = QUERY("<field_name>" << NEAR(point));
     * conn.query("<db_name>.<collection_name>", nearQuery);
     */
    class MONGO_CLIENT_API Coordinates2D : public Coordinates {
    public:
        Coordinates2D() : _x(0), _y(0) {}

        /**
         * Coordinates2D constructor
         *
         * @param coords The coordinate values.
         *
         * @pre coords.size() == Coordinates2D::dimensionality().
         */
        explicit Coordinates2D(const std::vector<double>& coords);

        /**
         * Coordinates2D constructor
         *
         * @param coords The coordinate values.
         */
        explicit Coordinates2D(const std::pair<double, double>& coords);

        /**
         * Coordinates2D constructor
         *
         * @param x The x coordinate value.
         * @param y The y coordinate value.
         */
        Coordinates2D(double x, double y) : _x(x), _y(y) {}

        /**
         * Get the x coordinate.
         *
         * @return double The x coordinate value.
         */
        double getX() const { return _x; }

        /**
         * Get the y coordinate.
         *
         * @return double The y coordinate value.
         */
        double getY() const { return _y; }

        /**
         * Get the x and y coordinates.
         *
         * @return std::vector<double> A vector [x, y] of this object's
         * coordinate values.
         */
        virtual std::vector<double> getValues() const;

        /**
         * Get the x and y coordinates as a std::pair.
         *
         * @return std::pair<double, double> A std::pair (x, y) of this
         * object's coordinate values.
         */
        std::pair<double, double> getValuesAsPair() const;

        /**
         * Get a BSON representation of the coordinates.
         *
         * @return BSONObj A BSONObj of the coordinates, structured as:
         * { "coordinates" : [ x, y ] }
         */
        virtual BSONObj toBSON() const;

        /**
         * Get the value of this coordinate at the given dimension.
         *
         * @param dimension 0 -> x, 1 -> y, any other input
         * besides 0 or 1 is an error.
         *
         * @return The coordinate value at the given dimension.
         *
         * @pre 0 <= dimension < Coordinates2D::dimensionality()
         */
        virtual double operator[](size_t dimension) const;

        /**
         * Get the dimensionality of this coordinate type.
         *
         * @return 2, the number of dimensions in Coordinates2D.
         */
        static size_t MONGO_CLIENT_FUNC dimensionality() { return 2; }

    private:
        double _x;
        double _y;
    };

} // namespace geo
} // namespace mongo
