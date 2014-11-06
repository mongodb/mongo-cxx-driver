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

#include "mongo/platform/basic.h"

#include <vector>

#include "mongo/unittest/unittest.h"

#include "mongo/geo/interface.h"

using std::vector;

using namespace mongo::geo;
using namespace mongo::unittest;
using namespace mongo;

namespace {
    bool eq(const Coordinates2D& a, const Coordinates2D& b) {
        return a.getValues() == b.getValues();
    }
    bool eq(const Coordinates2DGeographic& a, const Coordinates2DGeographic& b) {
        return a.getValues() == b.getValues();
    }
    BSONObj point(double x, double y) {
        return BSON("type" << "Point" << "coordinates" << BSON_ARRAY(x << y));
    }

    const BSONObj kP1 = point(0, 0);
    const BSONObj kP2 = point(0, 5);
    const BSONObj kP3 = point(6, 7);
    const BSONObj kP4 = point(10, -5);
    const BSONObj kLowerBound = point(0, -5);
    const BSONObj kUpperBound = point(10, 7);

    vector<Point<Coordinates2D> > initPoints() {
        vector<Point<Coordinates2D> > points;
        points.push_back(Point<Coordinates2D>(kP1));
        points.push_back(Point<Coordinates2D>(kP2));
        points.push_back(Point<Coordinates2D>(kP3));
        points.push_back(Point<Coordinates2D>(kP4));
        return points;
    }

    const vector<Point<Coordinates2D> > kPoints = initPoints();


    /* Coordinates2D Class */
    TEST(Coordinates2D, GetValues) {
        const double x = 1;
        const double y = 2;
        Coordinates2D coords(x, y);
        vector<double> values = coords.getValues();
        ASSERT_EQUALS(values.size(), Coordinates2D::dimensionality());
        ASSERT_EQUALS(values[0], x);
        ASSERT_EQUALS(values[1], y);
    }


    /* Coordinates2DGeographic Class */
    TEST(Coordinates2DGeographic, GetValues) {
        const double lon = 1;
        const double lat = 2;
        Coordinates2DGeographic coords(lon, lat);
        vector<double> values = coords.getValues();
        ASSERT_EQUALS(values.size(), Coordinates2D::dimensionality());
        ASSERT_EQUALS(values[0], lon);
        ASSERT_EQUALS(values[1], lat);
    }


    /* Point Class */
    TEST(Point, BSONConstructor) {
        Point<Coordinates2D> p1(kP1);
        Point<Coordinates2D> p2(kP2);
        ASSERT_EQUALS(kP2, p2.toBSON());
        ASSERT_EQUALS(kP1, p1.toBSON());
    }

    class PointTest : public ::testing::Test {
    protected:
        PointTest()
            : p1x(0),  p1y(0)
            , p2x(0),  p2y(5)
            , p3x(6),  p3y(7)
            , p4x(10), p4y(-5)

            , p1(kP1)
            , p2(kP2)
            , p3(kP3)
            , p4(kP4) {}

        const double p1x;
        const double p1y;
        const double p2x;
        const double p2y;
        const double p3x;
        const double p3y;
        const double p4x;
        const double p4y;

        Point<Coordinates2D> p1;
        Point<Coordinates2D> p2;
        Point<Coordinates2D> p3;
        Point<Coordinates2D> p4;
    };

    TEST_F(PointTest, GetType) {
        ASSERT_EQUALS(p1.getType(), GeoObjType_Point);
    }

    TEST_F(PointTest, GetCoordinates) {
        ASSERT_TRUE(eq(p1.getCoordinates(), Coordinates2D(p1x, p1y)));
    }

    TEST_F(PointTest, BoundingBox) {
        BoundingBox<Coordinates2D> bb = p1.getBoundingBox();
        ASSERT_TRUE(eq(bb.getMin(), bb.getMax()));
        ASSERT_TRUE(eq(bb.getMin(), p1.getCoordinates()));
    }

    TEST_F(PointTest, OperatorBrackets) {
        ASSERT_EQUALS(p4[0], p4x);
        ASSERT_EQUALS(p4[1], p4y);
        ASSERT_ANY_THROW(p4[-1]); // out of bounds
        ASSERT_ANY_THROW(p4[2]);  // out of bounds
    }

    TEST_F(PointTest, Assignment) {
        Point<Coordinates2D> p = p1;
        ASSERT_TRUE(eq(p.getCoordinates(), p1.getCoordinates()));
        ASSERT_EQUALS(p.toBSON(), p1.toBSON());
    }


    /* LineString Class */
    class LineStringTest : public ::testing::Test {
    protected:
        LineStringTest()
            : ls(kPoints)
            , lowerBound(Point<Coordinates2D>(kLowerBound).getCoordinates())
            , upperBound(Point<Coordinates2D>(kUpperBound).getCoordinates())
        {}

        LineString<Coordinates2D> ls;
        Coordinates2D lowerBound;
        Coordinates2D upperBound;
    };

    TEST_F(LineStringTest, GetType) {
        ASSERT_EQUALS(ls.getType(), GeoObjType_LineString);
    }

    TEST_F(LineStringTest, BoundingBox) {
        BoundingBox<Coordinates2D> bb = ls.getBoundingBox();
        ASSERT_TRUE(eq(bb.getMin(), lowerBound));
        ASSERT_TRUE(eq(bb.getMax(), upperBound));
    }

    /* Polygon Class */
    class PolygonTest : public ::testing::Test {
    protected:
        PolygonTest()
            : poly(addLSToVector(LineString<Coordinates2D>(kPoints)))
            , lowerBound(Point<Coordinates2D>(kLowerBound).getCoordinates())
            , upperBound(Point<Coordinates2D>(kUpperBound).getCoordinates())
        {}

        vector<LineString<Coordinates2D> > addLSToVector(const LineString<Coordinates2D>& ls) {
            vector<LineString<Coordinates2D> > vec;
            vec.push_back(ls);
            return vec;
        }

        mongo::geo::Polygon<Coordinates2D> poly;
        Coordinates2D lowerBound;
        Coordinates2D upperBound;
    };

    TEST_F(PolygonTest, GetType) {
        ASSERT_EQUALS(poly.getType(), GeoObjType_Polygon);
    }

    TEST_F(PolygonTest, BoundingBox) {
        BoundingBox<Coordinates2D> bb = poly.getBoundingBox();
        ASSERT_TRUE(eq(bb.getMin(), lowerBound));
        ASSERT_TRUE(eq(bb.getMax(), upperBound));
    }

} // namespace
