/*    Copyright 2015 MongoDB Inc.
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

#pragma once

#include "mongo/platform/float_utils.h"

namespace mongo {

/**
 * These functions compare numbers using the same rules as BSON. Care is taken to always give
 * numerically correct results when comparing different types. Returns are always -1, 0, or 1 to
 * ensure it is safe to negate the result to invert the direction of the comparison.
 */

inline int compareInts(int lhs, int rhs) {
    return lhs == rhs ? 0 : lhs < rhs ? -1 : 1;
}

inline int compareLongs(long long lhs, long long rhs) {
    return lhs == rhs ? 0 : lhs < rhs ? -1 : 1;
}

inline int compareDoubles(double lhs, double rhs) {
    if (lhs == rhs)
        return 0;
    if (lhs < rhs)
        return -1;
    if (lhs > rhs)
        return 1;

    // If none of the above cases returned, lhs or rhs must be NaN.
    if (isNaN(lhs))
        return isNaN(rhs) ? 0 : -1;
    return 1;
}

// This is the tricky one. Needs to support the following cases:
// * Doubles with a fractional component.
// * Longs that can't be precisely represented as a double.
// * Doubles outside of the range of Longs (including +/- Inf).
// * NaN (defined by us as less than all Longs)
// * Return value is always -1, 0, or 1 to ensure it is safe to negate.
inline int compareLongToDouble(long long lhs, double rhs) {
    // All Longs are > NaN
    if (isNaN(rhs))
        return 1;

    // Ints with magnitude <= 2**53 can be precisely represented as doubles.
    // Additionally, doubles outside of this range can't have a fractional component.
    static const long long kEndOfPreciseDoubles = 1ll << 53;
    if (lhs <= kEndOfPreciseDoubles && lhs >= -kEndOfPreciseDoubles) {
        return compareDoubles(lhs, rhs);
    }

    // Large magnitude doubles (including +/- Inf) are strictly > or < all Longs.
    static const double kBoundOfLongRange = -static_cast<double>(LLONG_MIN);  // positive 2**63
    if (rhs >= kBoundOfLongRange)
        return -1;  // Can't be represented in a Long.
    if (rhs < -kBoundOfLongRange)
        return 1;  // Can be represented in a Long.

    // Remaining Doubles can have their integer component precisely represented as long longs.
    // If they have a fractional component, they must be strictly > or < lhs even after
    // truncation of the fractional component since low-magnitude lhs were handled above.
    return compareLongs(lhs, rhs);
}

inline int compareDoubleToLong(double lhs, long long rhs) {
    // Only implement the real logic once.
    return -compareLongToDouble(rhs, lhs);
}

}  // namespace mongo
