// @file bsondemo.cpp

/*
 *    Copyright 2010 10gen Inc.
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

/*
    Example of use of BSON from C++.  Example is solely BSON, no MongoDB involved.

    Requires boost (headers only).
    Works as c++ "headers-only" (the parts actually exercised herein that is - some functions require .cpp files).

    To build and run:
      # "../../.." is the directory mongo/src/
      g++ -o bsondemo -I ../../.. bsondemo.cpp
      ./bsondemo

    Windows: project files are available in this directory for bsondemo.cpp for use with Visual Studio.
*/

// It is the responsibility of the mongo client consumer to ensure that any necessary windows
// headers have already been included before including the driver facade headers.
#if defined(_WIN32)
#include <winsock2.h>
#include <windows.h>
#endif

#include "mongo/client/dbclient.h"

#include <iostream>
#include <vector>

using namespace std;
using mongo::be;
using mongo::bo;
using mongo::bob;

void iter(bo o) {
    /* iterator example */
    cout << "\niter()\n";
    for( bo::iterator i(o); i.more(); ) {
        cout << ' ' << i.next().toString() << '\n';
    }
}

int main() {
    cout << "build bits: " << 8 * sizeof(char *) << '\n' <<  endl;

    /* a bson object defaults on construction to { } */
    bo empty;
    cout << "empty: " << empty << endl;

    /* make a simple { _id : <generated>, when : <now>, name : 'joe', age : 33.7 } object */
    {
        const mongo::OID generated = mongo::OID::gen();
        cout << "Generated an OID: " << generated << endl;

        bob b;
        b.append("_id", generated);
        b.append("when", mongo::jsTime());
        b.append("name", "joe");
        b.append("age", 33.7);
        bo result = b.obj();

        cout << "json for object with _id: " << result << endl;
    }

    /* make { name : 'joe', age : 33.7 } with a more compact notation. */
    bo x = bob().append("name", "joe").append("age", 33.7).obj();

    /* convert from bson to json */
    string json = x.toString();
    cout << "json for x:" << json << endl;

    /* access some fields of bson object x */
    cout << "Some x things: " << x["name"] << ' ' << x["age"].Number() << ' ' << x.isEmpty() << endl;

    /* make a bit more complex object with some nesting
       { x : 'asdf', y : true, subobj : { z : 3, q : 4 } }
    */
    bo y = BSON( "x" << "asdf" << "y" << true << "subobj" << BSON( "z" << 3 << "q" << 4 ) );

    /* print it */
    cout << "y: " << y << endl;

    /* reach in and get subobj.z */
    cout << "subobj.z: " << y.getFieldDotted("subobj.z").Number() << endl;

    /* alternate syntax: */
    cout << "subobj.z: " << y["subobj"]["z"].Number() << endl;

    /* fetch all *top level* elements from object y into a vector */
    vector<be> v;
    y.elems(v);
    cout << v[0] << endl;

    /* into an array */
    list<be> L;
    y.elems(L);

    bo sub = y["subobj"].Obj();

    iter(y);
    return 0;
}

