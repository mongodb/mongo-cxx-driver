#include <iostream>
#include <list>
#include <vector>
#include "mongo/bson/bson.h"

using mongo::BSONArray;
using mongo::BSONArrayBuilder;
using mongo::BSONObj;
using mongo::BSONObjBuilder;
using mongo::BSONElement;
using std::cout;
using std::endl;
using std::list;
using std::vector;

int main() {
    // Build an object
    BSONObjBuilder bob;

    // Build an array
    BSONArrayBuilder bab;
    bab.append("first");
    bab.append("second");
    bab.append("third");
    bab.append("fourth").append("fifth");

    // Place array in object at key "x"
    bob.appendArray("x", bab.arr());

    // Use BSON_ARRAY macro like BSON macro, but without keys
    BSONArray arr = BSON_ARRAY( "hello" << 1 << BSON( "foo" << BSON_ARRAY( "bar" << "baz" << "qux" ) ) );

    // Place the second array in object at key "y"
    bob.appendArray("y", arr);

    // Create the object
    BSONObj an_obj = bob.obj();

    /*
     * Extract the array directly from the BSONObj.
     *
     * Transforms a BSON array into a vector of BSONElements.
     *
     * We match array # positions with their vector position, and ignore
     * any fields with non-numeric field names.
     */
    vector<BSONElement> elements = an_obj["x"].Array();

    // Print the array out
    cout << "Our Array:" << endl;
    for (vector<BSONElement>::iterator it = elements.begin(); it != elements.end(); ++it){
        cout << *it << endl;
    }
    cout << endl;

    // Extract the array as a BSONObj
    BSONObj myarray = an_obj["y"].Obj();

    // Convert it to a vector
    vector<BSONElement> v;
    myarray.elems(v);

    // Convert it to a list
    list<BSONElement> L;
    myarray.elems(L);

    // Print the vector out
    cout << "The Vector Version:" << endl;
    for (vector<BSONElement>::iterator it = v.begin(); it != v.end(); ++it){
        cout << *it << endl;
    }

    cout << endl;

    // Print the list out
    cout << "The List Version:" << endl;
    for (list<BSONElement>::iterator it = L.begin(); it != L.end(); ++it){
        cout << *it << endl;
    }
}
