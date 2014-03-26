#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "mongo/client/dbclientinterface.h"
#include "mongo/client/gridfs.h"
#include "mongo/unittest/unittest.h"

using std::cout;
using std::endl;
using std::ios;
using std::auto_ptr;
using std::ifstream;
using std::string;
using std::stringstream;

namespace {
    const int DEFAULT_CHUNK_SIZE = 256 * 1024;
    const string TEST_DB = "gridfs-test";
}

namespace mongo {

    class GridFSTest : public unittest::Test {
    public:
        GridFSTest() {
            _conn = new DBClientConnection();
            _conn->connect("localhost:27999");
            _conn->dropDatabase(TEST_DB);
            _gfs = new GridFS(*_conn, TEST_DB);
        }

        virtual ~GridFSTest() {
            delete _gfs;
            delete _conn;
        }

    protected:
        DBClientConnection* getConn() {
            return _conn;
        }

        GridFS* getGrid() {
            return _gfs;
        }

    private:
        DBClientConnection* _conn;
        GridFS* _gfs;
    };

    TEST_F(GridFSTest, DefaultChunkSize) {
        ASSERT_EQUALS(DEFAULT_CHUNK_SIZE, getGrid()->getChunkSize());
    }

    TEST_F(GridFSTest, SetChunkSize) {
        GridFS* gfs = getGrid();
        gfs->setChunkSize(1234);
        ASSERT_EQUALS(1234, gfs->getChunkSize());
    }

    TEST_F(GridFSTest, StoreFile) {
        // Store the data
        string data = "this is the data";
        BSONObj result;
        GridFS* gfs = getGrid();
        result = gfs->storeFile(data.c_str(), data.length(), "data.txt");

        // Inspect the resulting BSONObj
        ASSERT_EQUALS(result["filename"].String(), "data.txt");
        ASSERT_EQUALS(result["length"].numberInt(), data.length());
        ASSERT_EQUALS(result["chunkSize"].numberInt(), DEFAULT_CHUNK_SIZE);
        ASSERT_TRUE(result.hasField("uploadDate"));
        ASSERT_TRUE(result.hasField("md5"));
        ASSERT_TRUE(result.hasField("_id"));
    }

    TEST_F(GridFSTest, StoreFileFromFile) {
        // Store the data
        string data = "this is the data";
        BSONObj result;
        GridFS* gfs = getGrid();
        string filename = "./src/mongo/unittest/data.txt";
        result = gfs->storeFile(filename, "data.txt");

        // Inspect the resulting BSONObj
        ASSERT_EQUALS(result["filename"].String(), "data.txt");
        ASSERT_EQUALS(result["length"].numberInt(), 20);
        ASSERT_EQUALS(result["chunkSize"].numberInt(), DEFAULT_CHUNK_SIZE);
        ASSERT_TRUE(result.hasField("uploadDate"));
        ASSERT_TRUE(result.hasField("md5"));
        ASSERT_TRUE(result.hasField("_id"));
    }

    TEST_F(GridFSTest, FindFile) {
        // Store the data
        string data = "this is the data";
        BSONObj result;
        GridFS* gfs = getGrid();
        result = gfs->storeFile(data.c_str(), data.length(), "data.txt", "text");

        // Get it back
        GridFile gf = gfs->findFile("data.txt");
        ASSERT_TRUE(gf.exists());
        ASSERT_TRUE(gf.getMetadata().isEmpty());
        ASSERT_EQUALS(gf.getContentType(), "text");
        ASSERT_EQUALS(gf.getFilename(), "data.txt");
        ASSERT_EQUALS(gf.getUploadDate(), result["uploadDate"].date());
        ASSERT_EQUALS(gf.getChunkSize(), result["chunkSize"].numberInt());
        ASSERT_EQUALS(gf.getMD5(), result["md5"].String());
        ASSERT_EQUALS(gf.getContentLength(), data.size());
        ASSERT_EQUALS(gf.getNumChunks(), 1);
    }

    TEST_F(GridFSTest, FindFileOrder) {
        // Store the data
        string data = "this is the data";
        string other = "this is ther other data";
        BSONObj result;
        GridFS* gfs = getGrid();
        result = gfs->storeFile(data.c_str(), data.length(), "data.txt");
        // Oh god... we need to sleep here because the uploadDate only has ms resolution
        sleepmillis(100);
        result = gfs->storeFile(other.c_str(), other.length(), "data.txt");

        // Ensure most recently inserted file with "data.txt" name comes back
        GridFile gf = gfs->findFile("data.txt");
        ASSERT_EQUALS(gf.getContentLength(), other.length());

        // Compare chunk data
        GridFSChunk chunk = gf.getChunk(0);
        int data_length = chunk.len();
        ASSERT_EQUALS(string(chunk.data(data_length)), other);
    }

    TEST_F(GridFSTest, WriteToFile) {
        // Store the data
        string data = "this is the data";
        BSONObj result;
        GridFS* gfs = getGrid();
        result = gfs->storeFile(data.c_str(), data.length(), "data.txt");

        // Get it back and write it to a file
        GridFile gf = gfs->findFile("data.txt");
        gf.write("/tmp/whatever");

        // check the written data is correct
        ifstream written_file;
        written_file.open("/tmp/whatever", ios::binary);
        stringstream written_data;
        written_data << written_file.rdbuf();
        ASSERT_EQUALS(written_data.str(), data);
    }

    TEST_F(GridFSTest, WriteToStream) {
        // Store the data
        string data = "this is the data";
        BSONObj result;
        GridFS* gfs = getGrid();
        result = gfs->storeFile(data.c_str(), data.length(), "data.txt");

        // Get it back and write some data to a string stream
        GridFile gf = gfs->findFile("data.txt");
        stringstream ss;
        gf.write(ss);

        // Check the written data is correct
        ASSERT_EQUALS(ss.str(), data);
    }

    TEST_F(GridFSTest, RemoveFile) {
        // Store the data
        string data = "this is the data";
        BSONObj result;
        GridFS* gfs = getGrid();
        result = gfs->storeFile(data.c_str(), data.length(), "data.txt");

        // Remove the file
        gfs->removeFile("data.txt");

        // Try to get the file
        GridFile gf = gfs->findFile("data.txt");
        ASSERT_FALSE(gf.exists());
    }

    TEST_F(GridFSTest, ListFiles) {
        // Store the data
        string data = "this is the data";
        BSONObj result;
        GridFS* gfs = getGrid();
        result = gfs->storeFile(data.c_str(), data.length(), "data.txt");
        result = gfs->storeFile(data.c_str(), data.length(), "data2.txt");

        // Peek at the first file, should be in insertion order
        auto_ptr<DBClientCursor> files = gfs->list();
        BSONObj file = files->peekFirst();
        ASSERT_EQUALS(file["filename"].String(), "data.txt");

        // Assert some things about the cursor and data set
        ASSERT_EQUALS(files->itcount(), 2);
        ASSERT_EQUALS(files->getns(), "gridfs-test.fs.files");
    }

    TEST_F(GridFSTest, ListFilesQuery) {
        // Store the data
        string data = "this is the data";
        BSONObj result;
        GridFS* gfs = getGrid();
        result = gfs->storeFile(data.c_str(), data.length(), "data.txt");

        // Query matches one file
        auto_ptr<DBClientCursor> files = gfs->list(
            BSON("filename" << "data.txt")
        );
        ASSERT_EQUALS(files->itcount(), 1);

        // Query matches no files
        auto_ptr<DBClientCursor> files_bad = gfs->list(
            BSON("filename" << "databad.txt")
        );
        ASSERT_EQUALS(files_bad->itcount(), 0);
    }
}
