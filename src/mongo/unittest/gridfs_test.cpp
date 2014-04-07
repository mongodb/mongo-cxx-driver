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

#include <fstream>
#include <sstream>
#include <string>

#include "mongo/unittest/integration_test.h"
#include "mongo/client/dbclient.h"

using boost::scoped_ptr;
using std::auto_ptr;
using std::ios;
using std::ifstream;
using std::string;
using std::stringstream;

using namespace mongo;
using mongo::unittest::IntegrationTestParameters;
extern IntegrationTestParameters params;

namespace {
    const unsigned int UDEFAULT_CHUNK_SIZE = 256 * 1024;
    const int DEFAULT_CHUNK_SIZE = UDEFAULT_CHUNK_SIZE;
    const string TEST_DB = "gridfs-test";
    const char DATA_NAME[] = "data.txt";
    const char OTHER_NAME[] = "other.txt";
    const char DATA[] = "this is the data";
    const char OTHER[] = "this is other data";
    const unsigned int UDATA_LEN = 16;
    const unsigned int UOTHER_LEN = 18;
    const int DATA_LEN = UDATA_LEN;
    const int OTHER_LEN = UOTHER_LEN;

    class GridFSTest : public unittest::Test {
    public:
        GridFSTest() : _conn(new DBClientConnection()) {
            _conn->connect("localhost:" + params.port);
            _conn->dropDatabase(TEST_DB);
            _gfs.reset(new GridFS(*_conn, TEST_DB));
        }

    protected:
        scoped_ptr<DBClientConnection> _conn;
        scoped_ptr<GridFS> _gfs;
    };
}

TEST_F(GridFSTest, DefaultChunkSize) {
    ASSERT_EQUALS(_gfs->getChunkSize(), UDEFAULT_CHUNK_SIZE);
}

TEST_F(GridFSTest, ModifyChunkSize) {
    _gfs->setChunkSize(1234U);
    ASSERT_EQUALS(_gfs->getChunkSize(), 1234U);
}

TEST_F(GridFSTest, StoreFile) {
    BSONObj result;
    result = _gfs->storeFile(DATA, DATA_LEN, DATA_NAME);

    ASSERT_EQUALS(result["filename"].str(), DATA_NAME);
    ASSERT_EQUALS(result["length"].numberInt(), DATA_LEN);
    ASSERT_EQUALS(result["chunkSize"].numberInt(), DEFAULT_CHUNK_SIZE);
    ASSERT_TRUE(result.hasField("uploadDate"));
    ASSERT_TRUE(result.hasField("md5"));
    ASSERT_TRUE(result.hasField("_id"));
}

TEST_F(GridFSTest, StoreFileFromFile) {
    BSONObj result;
    result = _gfs->storeFile("./src/mongo/unittest/data.txt", DATA_NAME);

    ASSERT_EQUALS(result["filename"].str(), DATA_NAME);
    ASSERT_EQUALS(result["length"].numberInt(), 20);
    ASSERT_EQUALS(result["chunkSize"].numberInt(), DEFAULT_CHUNK_SIZE);
    ASSERT_TRUE(result.hasField("uploadDate"));
    ASSERT_TRUE(result.hasField("md5"));
    ASSERT_TRUE(result.hasField("_id"));
}

TEST_F(GridFSTest, StoreFileMultipleChunks) {
    _gfs->setChunkSize(1);
    _gfs->storeFile(DATA, DATA_LEN, DATA_NAME);

    GridFile gf = _gfs->findFile(DATA_NAME);
    ASSERT_EQUALS(gf.getNumChunks(), DATA_LEN);
}

TEST_F(GridFSTest, FindFile) {
    const char content_type[] = "text";
    BSONObj result;
    result = _gfs->storeFile(DATA, DATA_LEN, DATA_NAME, content_type);

    GridFile gf = _gfs->findFile(DATA_NAME);
    ASSERT_TRUE(gf.exists());
    ASSERT_TRUE(gf.getMetadata().isEmpty());
    ASSERT_EQUALS(gf.getContentType(), content_type);
    ASSERT_EQUALS(gf.getFilename(), DATA_NAME);
    ASSERT_EQUALS(gf.getUploadDate(), result["uploadDate"].date());
    ASSERT_EQUALS(gf.getChunkSize(), result["chunkSize"].numberInt());
    ASSERT_EQUALS(gf.getMD5(), result["md5"].String());
    ASSERT_EQUALS(gf.getContentLength(), UDATA_LEN);
    ASSERT_EQUALS(gf.getNumChunks(), 1);
}

TEST_F(GridFSTest, FindFileOrder) {
    _gfs->storeFile(DATA, DATA_LEN, DATA_NAME);
    sleepmillis(100);   // must sleep here to avoid identical uploadDate
    _gfs->storeFile(OTHER, OTHER_LEN, DATA_NAME);

    GridFile gf = _gfs->findFile(DATA_NAME);
    ASSERT_EQUALS(gf.getContentLength(), UOTHER_LEN);

    GridFSChunk chunk = gf.getChunk(0);
    int data_length = chunk.len();
    ASSERT_EQUALS(string(chunk.data(data_length), data_length), OTHER);
}

TEST_F(GridFSTest, WriteToFile) {
    _gfs->storeFile(DATA, DATA_LEN, DATA_NAME);

    GridFile gf = _gfs->findFile(DATA_NAME);
    char tmp_name[12];
    tmpnam(tmp_name);
    gf.write(tmp_name);

    ifstream written_file;
    written_file.open(tmp_name, ios::binary);
    stringstream written_data;
    written_data << written_file.rdbuf();
    ASSERT_EQUALS(written_data.str(), DATA);
}

TEST_F(GridFSTest, WriteToStream) {
    _gfs->storeFile(DATA, DATA_LEN, DATA_NAME);

    GridFile gf = _gfs->findFile(DATA_NAME);
    stringstream ss;
    gf.write(ss);

    ASSERT_EQUALS(ss.str(), DATA);
}

TEST_F(GridFSTest, RemoveFile) {
    _gfs->storeFile(DATA, DATA_LEN, DATA_NAME);
    _gfs->storeFile(OTHER, DATA_LEN, OTHER_NAME);

    GridFile gfd = _gfs->findFile(DATA_NAME);
    ASSERT_TRUE(gfd.exists());
    GridFile gfo = _gfs->findFile(OTHER_NAME);
    ASSERT_TRUE(gfo.exists());

    _gfs->removeFile(DATA_NAME);

    gfd = _gfs->findFile(DATA_NAME);
    ASSERT_FALSE(gfd.exists());
    gfo = _gfs->findFile(OTHER_NAME);
    ASSERT_TRUE(gfo.exists());
}

TEST_F(GridFSTest, ListFiles) {
    _gfs->storeFile(DATA, DATA_LEN, DATA_NAME);
    _gfs->storeFile(DATA, DATA_LEN, OTHER_NAME);

    auto_ptr<DBClientCursor> files = _gfs->list();
    BSONObj file = files->peekFirst();
    ASSERT_EQUALS(file["filename"].String(), DATA_NAME);

    ASSERT_EQUALS(files->itcount(), 2);
    ASSERT_EQUALS(files->getns(), "gridfs-test.fs.files");
}

TEST_F(GridFSTest, ListFilesQuery) {
    _gfs->storeFile(DATA, DATA_LEN, DATA_NAME);

    auto_ptr<DBClientCursor> files = _gfs->list(
        BSON("filename" << DATA_NAME)
    );
    ASSERT_EQUALS(files->itcount(), 1);

    auto_ptr<DBClientCursor> files_bad = _gfs->list(
        BSON("filename" << OTHER_NAME)
    );
    ASSERT_EQUALS(files_bad->itcount(), 0);
}
