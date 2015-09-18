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

#include "mongo/client/export_macros.h"
#include "mongo/db/jsobj.h"

namespace mongo {
namespace geo {

class MONGO_CLIENT_API Coordinates {
public:
    virtual ~Coordinates() {}

    /**
     * Get the values of these coordinates as a vector of doubles.
     *
     * @return A vector<double> of the values of these coordinates.
     */
    virtual std::vector<double> getValues() const = 0;

    /**
     * Obtain a BSON representation of these coordinates.
     *
     * Example structure:
     * { "coordinates" : [ value1, value2 ] }
     *
     * @return a BSON representation of this point.
     */
    virtual BSONObj toBSON() const = 0;

    /**
     * Obtain the value of these coordinates in a particular dimension.
     *
     * @param  dimension The dimension whose coordinate value to return. 0 for x or longitude,
     * 1 for y or latitude
     *
     * @return The value of these coordinates in the given dimension.
     */
    virtual double operator[](size_t dimension) const = 0;
};

}  // namespace geo
}  // namespace mongo
