// gridfs.cpp

/*    Copyright 2009 10gen
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

#include "mongo/client/gridfs.h"

#include <algorithm>
#include <boost/smart_ptr.hpp>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <utility>

#if defined(_WIN32)
#include <io.h>
#endif

#include "mongo/client/dbclientcursor.h"

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif


namespace mongo {

using std::min;
using std::auto_ptr;
using std::cout;
using std::ios;
using std::ofstream;
using std::ostream;
using std::string;

const unsigned DEFAULT_CHUNK_SIZE = 255 * 1024;

GridFSChunk::GridFSChunk(BSONObj o) {
    _data = o;
}

GridFSChunk::GridFSChunk(BSONObj fileObject, int chunkNumber, const char* data, int len) {
    BSONObjBuilder b;
    b.appendAs(fileObject["_id"], "files_id");
    b.append("n", chunkNumber);
    b.appendBinData("data", len, BinDataGeneral, data);
    _data = b.obj();
}


GridFS::GridFS(DBClientBase& client, const string& dbName, const string& prefix)
    : _client(client), _dbName(dbName), _prefix(prefix) {
    _filesNS = dbName + "." + prefix + ".files";
    _chunksNS = dbName + "." + prefix + ".chunks";
    _chunkSize = DEFAULT_CHUNK_SIZE;

    client.createIndex(_filesNS, BSON("filename" << 1));
    client.createIndex(_chunksNS, IndexSpec().addKeys(BSON("files_id" << 1 << "n" << 1)).unique());
}

GridFS::~GridFS() {}

void GridFS::setChunkSize(unsigned int size) {
    massert(13296, "invalid chunk size is specified", (size != 0));
    _chunkSize = size;
}

unsigned int GridFS::getChunkSize() const {
    return _chunkSize;
}

BSONObj GridFS::storeFile(const char* data,
                          size_t length,
                          const string& remoteName,
                          const string& contentType) {
    char const* const end = data + length;

    OID id;
    id.init();
    BSONObj idObj = BSON("_id" << id);

    int chunkNumber = 0;
    while (data < end) {
        int chunkLen = MIN(_chunkSize, (unsigned)(end - data));
        GridFSChunk c(idObj, chunkNumber, data, chunkLen);
        _client.insert(_chunksNS.c_str(), c._data);

        chunkNumber++;
        data += chunkLen;
    }

    return insertFile(remoteName, id, length, contentType);
}


BSONObj GridFS::storeFile(const string& fileName,
                          const string& remoteName,
                          const string& contentType) {
    FILE* fd;
    if (fileName == "-")
        fd = stdin;
    else
        fd = fopen(fileName.c_str(), "rb");
    uassert(10013, "error opening file", fd);

    OID id;
    id.init();
    BSONObj idObj = BSON("_id" << id);

    int chunkNumber = 0;
    gridfs_offset length = 0;
    while (!feof(fd)) {
        // boost::scoped_array<char>buf (new char[_chunkSize+1]);
        char* buf = new char[_chunkSize + 1];
        char* bufPos = buf;         //.get();
        unsigned int chunkLen = 0;  // how much in the chunk now
        while (chunkLen != _chunkSize && !feof(fd)) {
            int readLen = fread(bufPos, 1, _chunkSize - chunkLen, fd);
            chunkLen += readLen;
            bufPos += readLen;

            verify(chunkLen <= _chunkSize);
        }

        GridFSChunk c(idObj, chunkNumber, buf, chunkLen);
        _client.insert(_chunksNS.c_str(), c._data);

        length += chunkLen;
        chunkNumber++;
        delete[] buf;
    }

    if (fd != stdin)
        fclose(fd);

    return insertFile((remoteName.empty() ? fileName : remoteName), id, length, contentType);
}

BSONObj GridFS::insertFile(const string& name,
                           const OID& id,
                           gridfs_offset length,
                           const string& contentType) {
    // Wait for any pending writebacks to finish
    BSONObj errObj = _client.getLastErrorDetailed();
    uassert(16428,
            str::stream() << "Error storing GridFS chunk for file: " << name
                          << ", error: " << errObj,
            DBClientWithCommands::getLastErrorString(errObj) == "");

    BSONObj res;
    if (!_client.runCommand(_dbName.c_str(), BSON("filemd5" << id << "root" << _prefix), res))
        throw UserException(9008, "filemd5 failed");

    BSONObjBuilder file;
    file << "_id" << id << "filename" << name << "chunkSize" << _chunkSize << "uploadDate"
         << DATENOW << "md5" << res["md5"];

    if (length < 1024 * 1024 * 1024) {  // 2^30
        file << "length" << (int)length;
    } else {
        file << "length" << (long long)length;
    }

    if (!contentType.empty())
        file << "contentType" << contentType;

    BSONObj ret = file.obj();
    _client.insert(_filesNS.c_str(), ret);

    return ret;
}

void GridFS::removeFile(const string& fileName) {
    auto_ptr<DBClientCursor> files = _client.query(_filesNS, BSON("filename" << fileName));
    if (files.get()) {
        while (files->more()) {
            BSONObj file = files->next();
            BSONElement id = file["_id"];
            _client.remove(_filesNS.c_str(), BSON("_id" << id));
            _client.remove(_chunksNS.c_str(), BSON("files_id" << id));
        }
    }
}

GridFile::GridFile(const GridFS* grid, BSONObj obj) {
    _grid = grid;
    _obj = obj;
}

GridFile GridFS::findFileByName(const string& fileName) const {
    return findFile(BSON("filename" << fileName));
};

GridFile GridFS::findFile(Query query) const {
    query.sort(BSON("uploadDate" << -1));
    return GridFile(this, _client.findOne(_filesNS.c_str(), query.obj));
}

auto_ptr<DBClientCursor> GridFS::list() const {
    return _client.query(_filesNS.c_str(), BSONObj());
}

auto_ptr<DBClientCursor> GridFS::list(BSONObj o) const {
    return _client.query(_filesNS.c_str(), o);
}

void GridFS::_insertChunk(const GridFSChunk& chunk) {
    _client.insert(_chunksNS.c_str(), chunk._data);
}

BSONObj GridFile::getMetadata() const {
    BSONElement meta_element = _obj["metadata"];
    if (meta_element.eoo()) {
        return BSONObj();
    }

    return meta_element.embeddedObject();
}

GridFSChunk GridFile::getChunk(int n) const {
    _exists();
    BSONObjBuilder b;
    b.appendAs(_obj["_id"], "files_id");
    b.append("n", n);

    BSONObj o = _grid->_client.findOne(_grid->_chunksNS.c_str(), b.obj());
    uassert(10014, "chunk is empty!", !o.isEmpty());
    return GridFSChunk(o);
}

gridfs_offset GridFile::write(ostream& out) const {
    _exists();

    const int num = getNumChunks();

    for (int i = 0; i < num; i++) {
        GridFSChunk c = getChunk(i);

        int len;
        const char* data = c.data(len);
        out.write(data, len);
    }

    return getContentLength();
}

gridfs_offset GridFile::write(const string& where) const {
    if (where == "-") {
        return write(cout);
    } else {
        ofstream out(where.c_str(), ios::out | ios::binary);
        uassert(13325, "couldn't open file: " + where, out.is_open());
        return write(out);
    }
}

void GridFile::_exists() const {
    uassert(10015, "doesn't exists", exists());
}

GridFileBuilder::GridFileBuilder(GridFS* const grid)
    : _grid(grid),
      _chunkSize(grid->getChunkSize()),
      _currentChunk(0),
      _pendingData(new char[_chunkSize]),
      _pendingDataSize(0),
      _fileLength(0) {
    _fileId.init();
    _fileIdObj = BSON("_id" << _fileId);
}

const char* GridFileBuilder::_appendChunk(const char* data,
                                          size_t length,
                                          bool forcePendingInsert) {
    const char* const end = data + length;
    while (data < end) {
        size_t chunkLen = min(_chunkSize, static_cast<size_t>(end - data));
        // the last chunk needs to be stored as pendingData, break while if
        // necessary
        if ((chunkLen < _chunkSize) && (!forcePendingInsert))
            break;
        GridFSChunk chunk(_fileIdObj, _currentChunk, data, chunkLen);
        _grid->_insertChunk(chunk);
        ++_currentChunk;
        data += chunkLen;
        _fileLength += chunkLen;
    }
    return data;
}

void GridFileBuilder::_appendPendingData() {
    if (_pendingDataSize > 0) {
        _appendChunk(_pendingData.get(), _pendingDataSize, true);
        _pendingDataSize = 0;
    }
}

void GridFileBuilder::appendChunk(const char* data, size_t length) {
    if (length == 0)
        return;

    // check if there is pending data
    if (_pendingDataSize > 0) {
        size_t totalSize = _pendingDataSize + length;
        size_t size = min(_chunkSize, totalSize) - _pendingDataSize;
        memcpy(_pendingData.get() + _pendingDataSize, data, size);
        _pendingDataSize += size;
        invariant(_pendingDataSize <= _chunkSize);
        if (_pendingDataSize == _chunkSize) {
            _appendPendingData();
            const char* const end = data + length;
            data = _appendChunk(data + size, length - size, false);
            if (data != end) {
                invariant(data < end);
                size_t nsize = static_cast<size_t>(end - data);
                memcpy(_pendingData.get() + _pendingDataSize, data, nsize);
                _pendingDataSize += nsize;
            }
        }
    } else {
        const char* const end = data + length;
        // split data in _chunkSize blocks, and store as pending the last block if
        // necessary
        data = _appendChunk(data, length, false);
        // process pending data if necessary
        if (data != end) {
            size_t size = static_cast<size_t>(end - data);
            memcpy(_pendingData.get() + _pendingDataSize, data, size);
            _pendingDataSize += size;
        }
    }
}

BSONObj GridFileBuilder::buildFile(const string& remoteName, const string& contentType) {
    _appendPendingData();
    BSONObj ret = _grid->insertFile(remoteName, _fileId, _fileLength, contentType);
    // resets the object to allow more data append for a GridFile
    _currentChunk = 0;
    _pendingDataSize = 0;
    _fileLength = 0;
    _fileId.init();
    _fileIdObj = BSON("_id" << _fileId);
    return ret;
}
}
