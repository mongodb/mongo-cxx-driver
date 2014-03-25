#include <iostream>
#include <string>
#include "mongo/client/dbclientinterface.h"
#include "mongo/client/gridfs.h"
#include "mongo/unittest/unittest.h"

namespace mongo {

    class GridFSTest : public unittest::Test {
    public:
        GridFSTest() {
            _conn.connect("localhost:27999");
            _gfs = new GridFS(_conn, "gridfs-test");
        }

        virtual ~GridFSTest() {
            delete _gfs;
        }

    protected:
        GridFS* getGrid() {
            return _gfs;
        }

    private:
        DBClientConnection _conn;
        GridFS* _gfs;
    };

    TEST_F(GridFSTest, DefaultChunkSize) {
        ASSERT_EQUALS(256 * 1024, getGrid()->getChunkSize());
    }

    TEST_F(GridFSTest, SetChunkSize) {
        GridFS* gfs = getGrid();
        gfs->setChunkSize(1234);
        ASSERT_EQUALS(1234, gfs->getChunkSize());
    }

    TEST_F(GridFSTest, StoreFile) {
        std::string data = "this is the data";
        GridFS* gfs = getGrid();
        gfs->storeFile(data.c_str(), data.length(), "data.txt");
    }

}
