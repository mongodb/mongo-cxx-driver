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

void insertGeoData(DBClientBase* conn) {
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

    conn->insert(kDbCollectionName, BSON(kLocField << p1.toBSON()));
    conn->insert(kDbCollectionName, BSON(kLocField << p2.toBSON()));
    conn->insert(kDbCollectionName, BSON(kLocField << p3.toBSON()));
    conn->insert(kDbCollectionName, BSON(kLocField << p4.toBSON()));
    conn->insert(kDbCollectionName, BSON(kLocField << line.toBSON()));
    conn->insert(kDbCollectionName, BSON(kLocField << mls.toBSON()));
    conn->insert(kDbCollectionName, BSON(kLocField << mp.toBSON()));
    conn->insert(kDbCollectionName, BSON(kLocField << gcol.toBSON()));

    conn->createIndex(kDbCollectionName, fromjson("{loc:\"2dsphere\"}"));

    cout << "Coordinates p1 toBSON().toString():" << endl;
    cout << p1.getCoordinates().toBSON().toString() << endl;
    cout << "MultiLineString mls toBSON().jsonString():" << endl;
    cout << mls.toBSON().jsonString() << endl << endl;
}

void queryGeoData(DBClientBase* conn) {
    BSONObj lineBson = BSON("type" << "LineString" << "coordinates" <<
        BSON_ARRAY(BSON_ARRAY(0.0 << 0.0) <<
                   BSON_ARRAY(50.0 << 50.0)));
    LineString line(lineBson);
    Query q = MONGO_QUERY(kLocField << WITHINQUERY(line.getBoundingBox()));
    Query q2 = MONGO_QUERY(kLocField << INTERSECTSQUERY(line));

    BSONObj multipointBson = BSON("type" << "MultiPoint" << "coordinates" <<
        BSON_ARRAY(BSON_ARRAY(0.0 << 0.0) <<
                   BSON_ARRAY(50.0 << 50.0)));

    MultiPoint mPoint(multipointBson);
    Query q3 = MONGO_QUERY(kLocField << WITHINQUERY(mPoint.getBoundingBox()));
    Query q4 = MONGO_QUERY(kLocField << INTERSECTSQUERY(mPoint));

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

    Query q5 = MONGO_QUERY(kLocField << WITHINQUERY(poly.getBoundingBox()));
    Query q6 = MONGO_QUERY(kLocField << INTERSECTSQUERY(poly));

    geo::Coordinates2DGeographic coords(1, 2);
    geo::coords2dgeographic::Point point(coords);

    Query nearQuery = MONGO_QUERY(kLocField << NEARQUERY(point, 0.5));
    Query nearSphereQuery = MONGO_QUERY(kLocField << NEARSPHEREQUERY(point, 0.5));

    cout << "*** Testing LineString ***" << endl;

    auto_ptr<DBClientCursor> cursor = conn->query(kDbCollectionName, q);
    cout << "Results from WITHINQUERY" << endl;
    while (cursor->more())
        cout << cursor->next().toString() << endl;
    cout << "---------------" << endl;

    auto_ptr<DBClientCursor> cursor2 = conn->query(kDbCollectionName, q2);
    cout << "Results from INTERSECTSQUERY" << endl;
    while (cursor2->more())
        cout << cursor2->next().toString() << endl;
    cout << "---------------" << endl;

    cout << "*** Testing MultiPoint ***" << endl;

    auto_ptr<DBClientCursor> cursor3 = conn->query(kDbCollectionName, q3);
    cout << "Results from WITHINQUERY" << endl;
    while (cursor3->more())
        cout << cursor3->next().toString() << endl;
    cout << "---------------" << endl;

    auto_ptr<DBClientCursor> cursor4 = conn->query(kDbCollectionName, q4);
    cout << "Results from INTERSECTSQUERY" << endl;
    while (cursor4->more())
        cout << cursor4->next().toString() << endl;
    cout << "---------------" << endl;

    cout << "*** Testing Polygon ***" << endl;

    auto_ptr<DBClientCursor> cursor5 = conn->query(kDbCollectionName, q5);
    cout << "Results from GEOWITHIN" << endl;
    while (cursor5->more())
        cout << cursor5->next().toString() << endl;
    cout << "---------------" << endl;

    auto_ptr<DBClientCursor> cursor6 = conn->query(kDbCollectionName, q6);
    cout << "Results from INTERSECTSQUERY" << endl;
    while (cursor6->more())
        cout << cursor6->next().toString() << endl;
    cout << "---------------" << endl;

    auto_ptr<DBClientCursor> nearQueryCursor = conn->query(kDbCollectionName, nearQuery);
    cout << "Results from NEAR" << endl;
    while (nearQueryCursor->more())
        cout << nearQueryCursor->next().toString() << endl;
    cout << "---------------" << endl;

    auto_ptr<DBClientCursor> nearSphereQueryCursor = conn->query(kDbCollectionName, nearSphereQuery);
    cout << "Results from NEARSPHERE" << endl;
    while (nearSphereQueryCursor->more())
        cout << nearSphereQueryCursor->next().toString() << endl;
    cout << "---------------" << endl;

}

int main( int argc, const char **argv ) {

    if ( argc > 2 ) {
        std::cout << "usage: " << argv[0] << " [MONGODB_URI]"  << std::endl;
        return EXIT_FAILURE;
    }

    client::GlobalInstance instance;
    if (!instance.initialized()) {
        std::cout << "failed to initialize the client driver: " << instance.status() << std::endl;
        return EXIT_FAILURE;
    }

    std::string uri = argc == 2 ? argv[1] : "mongodb://localhost:27017";
    std::string errmsg;

    ConnectionString cs = ConnectionString::parse(uri, errmsg);

    if (!cs.isValid()) {
        std::cout << "Error parsing connection string " << uri << ": " << errmsg << std::endl;
        return EXIT_FAILURE;
    }

    boost::scoped_ptr<DBClientBase> conn(cs.connect(errmsg));
    if ( !conn ) {
        cout << "couldn't connect : " << errmsg << endl;
        return EXIT_FAILURE;
    }

    try {
        BSONObj cmdResult;
        conn->runCommand("admin", BSON("buildinfo" << true), cmdResult);
        if (cmdResult["versionArray"].Array()[1].Int() < 6)
            return EXIT_SUCCESS;

        insertGeoData(conn.get());
        queryGeoData(conn.get());
        conn->dropCollection(kDbCollectionName);
    }
    catch(const DBException& dbe) {
        cout << "caught DBException " << dbe.toString() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
