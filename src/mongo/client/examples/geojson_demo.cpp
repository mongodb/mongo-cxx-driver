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

// It is the responsibility of the mongo client consumer to ensure that any necessary windows
// headers have already been included before including the driver facade headers.
#if defined(_WIN32)
#include <winsock2.h>
#include <windows.h>
#endif

#include <iostream>
#include <cstdlib>

#include "mongo/bson/bson.h"
#include "mongo/client/dbclient.h"

using namespace std;
using namespace mongo;
using namespace mongo::geo::coords2dgeographic;

static const char* kDbCollectionName = "geotest.data";
static const char* kLocField = "loc";

void insertGeoData(DBClientConnection& conn) {
    Point p1(BSON("type" << "Point" << "coordinates" << BSON_ARRAY(-5.0 << -5.0)));
    Point p2(BSON("type" << "Point" << "coordinates" << BSON_ARRAY(100.0 << 0.0)));
    Point p3(BSON("type" << "Point" << "coordinates" << BSON_ARRAY(20.0 << 30.0)));
    Point p4(BSON("type" << "Point" << "coordinates" << BSON_ARRAY(50.0 << 50.0)));
    cout << p4.toBSON().jsonString() << endl;

    BSONObj lineBson = BSON("type" << "LineString" << "coordinates" <<
        BSON_ARRAY(BSON_ARRAY(0.0 << 10.0) << BSON_ARRAY(100.0 << 10.0)));
    LineString line(lineBson);
    std::vector<LineString> lineStrings;
    lineStrings.push_back(line);
    lineStrings.push_back(line);
    MultiLineString mls(lineStrings);
    geo::coords2dgeographic::Polygon poly(lineStrings);

    BSONObj mpBson = BSON("type" << "MultiPolygon" << "coordinates" << BSON_ARRAY(
        BSON_ARRAY(
            BSON_ARRAY(
                BSON_ARRAY(0.0 << 10.0) <<
                BSON_ARRAY(100.0 << 10.0) <<
                BSON_ARRAY(5.0 << 5.0) <<
                BSON_ARRAY(0.0 << 10.0)))));

    cout << "MULTIPOLYGON BSON:" << endl;
    cout << mpBson.jsonString() << endl;
    MultiPolygon mp(mpBson);
    cout << mp.toBSON().jsonString() << endl;

    BSONObj gcolBson = BSON("type" << "GeometryCollection" << "geometries" <<
        BSON_ARRAY(p1.toBSON() << p2.toBSON() << line.toBSON()));
    GeometryCollection gcol(gcolBson);
    cout << "GEO COLLECTION BSON:" << endl;
    cout << gcol.toBSON() << endl;
    const vector<const GeoObj*>& geoms = gcol.getGeometries();
    for (size_t i = 0; i < geoms.size(); ++i) {
        cout << geoms[i]->getType() << endl;
    }

    conn.insert(kDbCollectionName, BSON(kLocField << p1.toBSON()));
    conn.insert(kDbCollectionName, BSON(kLocField << p2.toBSON()));
    conn.insert(kDbCollectionName, BSON(kLocField << p3.toBSON()));
    conn.insert(kDbCollectionName, BSON(kLocField << p4.toBSON()));
    conn.insert(kDbCollectionName, BSON(kLocField << line.toBSON()));
    conn.insert(kDbCollectionName, BSON(kLocField << mls.toBSON()));
    conn.insert(kDbCollectionName, BSON(kLocField << mp.toBSON()));
    conn.insert(kDbCollectionName, BSON(kLocField << gcol.toBSON()));

    conn.createIndex(kDbCollectionName, fromjson("{loc:\"2dsphere\"}"));

    cout << "Coordinates p1 toBSON().toString():" << endl;
    cout << p1.getCoordinates().toBSON().toString() << endl;
    cout << "MultiLineString mls toBSON().jsonString():" << endl;
    cout << mls.toBSON().jsonString() << endl << endl;
}

void queryGeoData(DBClientConnection& conn) {
    BSONObj lineBson = BSON("type" << "LineString" << "coordinates" <<
        BSON_ARRAY(BSON_ARRAY(0.0 << 0.0) <<
                   BSON_ARRAY(50.0 << 50.0)));
    LineString line(lineBson);
    Query q = QUERY(kLocField << GEOWITHIN(line.getBoundingBox()));
    Query q2 = QUERY(kLocField << GEOINTERSECTS(line));

    BSONObj multipointBson = BSON("type" << "MultiPoint" << "coordinates" <<
        BSON_ARRAY(BSON_ARRAY(0.0 << 0.0) <<
                   BSON_ARRAY(50.0 << 50.0)));

    MultiPoint mPoint(multipointBson);
    Query q3 = QUERY(kLocField << GEOWITHIN(mPoint.getBoundingBox()));
    Query q4 = QUERY(kLocField << GEOINTERSECTS(mPoint));

    BSONObj polygonBson = BSON("type" << "Polygon" << "coordinates" <<
        BSON_ARRAY( // list of linear rings
            BSON_ARRAY( // a single linear ring
                BSON_ARRAY(0.0 << 0.0) <<
                BSON_ARRAY(0.0 << 50.0) <<
                BSON_ARRAY(80.0 << 80.0) <<
                BSON_ARRAY(0.0 << 0.0)
            )
        )
    );
    geo::coords2dgeographic::Polygon poly(polygonBson);

    // Make sure printing is ok:
    cout << "LineString json:" << endl;
    cout << line.toBSON().jsonString() << endl;
    cout << "MultiPoint json:" << endl;
    cout << mPoint.toBSON().jsonString() << endl;
    cout << "Polygon json:" << endl;
    cout << poly.toBSON().jsonString() << endl;
    cout << endl << endl;

    Query q5 = QUERY(kLocField << GEOWITHIN(poly.getBoundingBox()));
    Query q6 = QUERY(kLocField << GEOINTERSECTS(poly));

    cout << "*** Testing LineString ***" << endl;

    auto_ptr<DBClientCursor> cursor = conn.query(kDbCollectionName, q);
    cout << "Results from GEOWITHIN" << endl;
    while (cursor->more())
        cout << cursor->next().toString() << endl;
    cout << "---------------" << endl;

    auto_ptr<DBClientCursor> cursor2 = conn.query(kDbCollectionName, q2);
    cout << "Results from GEOINTERSECTS" << endl;
    while (cursor2->more())
        cout << cursor2->next().toString() << endl;
    cout << "---------------" << endl;

    cout << "*** Testing MultiPoint ***" << endl;

    auto_ptr<DBClientCursor> cursor3 = conn.query(kDbCollectionName, q3);
    cout << "Results from GEOWITHIN" << endl;
    while (cursor3->more())
        cout << cursor3->next().toString() << endl;
    cout << "---------------" << endl;

    auto_ptr<DBClientCursor> cursor4 = conn.query(kDbCollectionName, q4);
    cout << "Results from GEOINTERSECTS" << endl;
    while (cursor4->more())
        cout << cursor4->next().toString() << endl;
    cout << "---------------" << endl;

    cout << "*** Testing Polygon ***" << endl;

    auto_ptr<DBClientCursor> cursor5 = conn.query(kDbCollectionName, q5);
    cout << "Results from GEOWITHIN" << endl;
    while (cursor5->more())
        cout << cursor5->next().toString() << endl;
    cout << "---------------" << endl;

    auto_ptr<DBClientCursor> cursor6 = conn.query(kDbCollectionName, q6);
    cout << "Results from GEOINTERSECTS" << endl;
    while (cursor6->more())
        cout << cursor6->next().toString() << endl;
    cout << "---------------" << endl;

}

int main( int argc, const char **argv ) {

    Status status = client::initialize();
    if (!status.isOK()) {
        std::cout << "failed to initialize the client driver: " << status.toString() << endl;
        return EXIT_FAILURE;
    }

    const char *port = "27017";
    if (argc != 1) {
        if (argc != 3) {
            std::cout << "need to pass port as second param" << endl;
            return EXIT_FAILURE;
        }
        port = argv[2];
    }

    DBClientConnection conn;
    try {
        cout << "connecting to localhost..." << endl;
        conn.connect(string("localhost:") + port);
        cout << "connected ok" << endl;
        insertGeoData(conn);
        queryGeoData(conn);
        conn.dropCollection(kDbCollectionName);
    }
    catch(const DBException& dbe) {
        cout << "caught DBException " << dbe.toString() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
