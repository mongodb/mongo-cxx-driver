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

#include "mongo/geo/coordinates2dgeographic.h"

#include <vector>

#include "mongo/db/jsobj.h"
#include "mongo/geo/constants.h"
#include "mongo/util/assert_util.h"

namespace mongo {
namespace geo {

Coordinates2DGeographic::Coordinates2DGeographic(const std::vector<double>& coords) {
    uassert(0,
            "Number of elements in coords must match dimensionality of coordinate type",
            coords.size() == dimensionality());
    _longitude = coords[0];
    _latitude = coords[1];
}

Coordinates2DGeographic::Coordinates2DGeographic(const std::pair<double, double>& coords) {
    _longitude = coords.first;
    _latitude = coords.second;
}

Coordinates2DGeographic::Coordinates2DGeographic(double longitude, double latitude)
    : _longitude(longitude), _latitude(latitude) {}

std::vector<double> Coordinates2DGeographic::getValues() const {
    double vals[] = {_longitude, _latitude};
    return std::vector<double>(vals, vals + sizeof(vals) / sizeof(vals[0]));
}

std::pair<double, double> Coordinates2DGeographic::getValuesAsPair() const {
    return std::make_pair(_longitude, _latitude);
}

BSONObj Coordinates2DGeographic::toBSON() const {
    return BSON(kCoordsFieldName << BSON_ARRAY(_longitude << _latitude));
}

double Coordinates2DGeographic::operator[](size_t dimension) const {
    switch (dimension) {
        case 0:
            return _longitude;
        case 1:
            return _latitude;
        default:
            uassert(0,
                    "Attempt to access out-of-bounds element (only valid indices are 0 and 1 for"
                    " Coordinates2DGeographic)",
                    false);
    }
}

}  // namespace geo
}  // namespace mongo
