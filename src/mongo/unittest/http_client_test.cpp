#include <iostream>

#include "mongo/unittest/integration_test.h"
#include "mongo/util/net/httpclient.h"

using namespace std;
using namespace mongo;
using namespace mongo::unittest;

TEST(HTTPClient, BasicTest) {
    int mongo_port = atoi(integrationTestParams.port.c_str());
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
