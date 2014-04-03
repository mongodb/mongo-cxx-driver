#include <iostream>

#include "mongo/integration/integration.h"
#include "mongo/client/dbclientinterface.h"
#include "mongo/util/net/httpclient.h"

using namespace std;
using namespace mongo;

extern ParameterStruct TestParams;

TEST(HTTPClient, BasicTest) {
    int mongo_port = atoi(TestParams.port.c_str());
    int http_port = mongo_port + 1000;

    HttpClient c;
    HttpClient::Result r;

    str::stream url;
    url << "http://localhost:" << http_port << "/";

    int http_code = c.get(url, &r);
    ASSERT_EQUALS(http_code, 200);

    HttpClient::Headers h = r.getHeaders();
    ASSERT_EQUALS(h["Content-Type"], "text/html;charset=utf-8");
}
