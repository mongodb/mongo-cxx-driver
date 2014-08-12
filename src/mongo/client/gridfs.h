/** @file gridfs.h */

/*    Copyright 2009 10gen Inc.
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

#pragma once

#include "boost/scoped_array.hpp"

#include "mongo/bson/bsonelement.h"
#include "mongo/bson/bsonobj.h"
#include "mongo/client/dbclientinterface.h"
#include "mongo/client/export_macros.h"

namespace mongo {

    typedef unsigned long long gridfs_offset;

    class GridFS;
    class GridFile;
    class GridFileBuilder;

    class MONGO_CLIENT_API GridFSChunk {
    public:
        GridFSChunk( BSONObj data );
        GridFSChunk( BSONObj fileId , int chunkNumber , const char * data , int len );

        int len() const {
            int len;
            _data["data"].binDataClean( len );
            return len;
        }

        const char * data( int & len ) const {
            return _data["data"].binDataClean( len );
        }

    private:
        BSONObj _data;
        friend class GridFS;
    };


    /**
      GridFS is for storing large file-style objects in MongoDB.
      @see http://dochub.mongodb.org/core/gridfsspec
     */
    class MONGO_CLIENT_API GridFS {
    public:
        /**
         * @param client - db connection
         * @param dbName - root database name
         * @param prefix - if you want your data somewhere besides <dbname>.fs
         */
        GridFS( DBClientBase& client , const std::string& dbName , const std::string& prefix="fs" );
        ~GridFS();

        /**
         * @param
         */
        void setChunkSize(unsigned int size);

        unsigned int getChunkSize() const;

        /**
         * puts the file reference by fileName into the db
         * @param fileName local filename relative to process
         * @param remoteName optional filename to use for file stored in GridFS
         *                   (default is to use fileName parameter)
         * @param contentType optional MIME type for this object.
         *                    (default is to omit)
         * @return the file object
         */
        BSONObj storeFile( const std::string& fileName , const std::string& remoteName="" , const std::string& contentType="");

        /**
         * puts the file represented by data into the db
         * @param data pointer to buffer to store in GridFS
         * @param length length of buffer
         * @param remoteName filename to use for file stored in GridFS
         * @param contentType optional MIME type for this object.
         *                    (default is to omit)
         * @return the file object
         */
        BSONObj storeFile( const char* data , size_t length , const std::string& remoteName , const std::string& contentType="");

        /**
         * removes file referenced by fileName from the db
         * @param fileName filename (in GridFS) of the file to remove
         * @return the file object
         */
        void removeFile( const std::string& fileName );

        /**
         * returns a file object matching the query
         */
        GridFile findFile( Query query ) const;

        /**
         * equiv to findFile( { filename : filename } )
         */
        GridFile findFileByName( const std::string& fileName ) const;

        /**
         * convenience method to get all the files
         */
        std::auto_ptr<DBClientCursor> list() const;

        /**
         * convenience method to get all the files with a filter
         */
        std::auto_ptr<DBClientCursor> list( BSONObj query ) const;

    private:
        DBClientBase& _client;
        std::string _dbName;
        std::string _prefix;
        std::string _filesNS;
        std::string _chunksNS;
        unsigned int _chunkSize;

        // insert fileobject. All chunks must be in DB.
        BSONObj insertFile(const std::string& name, const OID& id, gridfs_offset length, const std::string& contentType);

        // Insert a chunk into DB, this method is intended to be used by
        // GridFileBuilder to incrementally insert chunks
        void _insertChunk(const GridFSChunk& chunk);

        friend class GridFile;
        friend class GridFileBuilder;
    };

    /**
       wrapper for a file stored in the Mongo database
     */
    class MONGO_CLIENT_API GridFile {
    public:
        /**
         * @return whether or not this file exists
         * findFile will always return a GriFile, so need to check this
         */
        bool exists() const {
            return ! _obj.isEmpty();
        }

        std::string getFilename() const {
            return _obj["filename"].str();
        }

        int getChunkSize() const {
            return (int)(_obj["chunkSize"].number());
        }

        gridfs_offset getContentLength() const {
            return (gridfs_offset)(_obj["length"].number());
        }

        std::string getContentType() const {
            return _obj["contentType"].valuestr();
        }

        Date_t getUploadDate() const {
            return _obj["uploadDate"].date();
        }

        std::string getMD5() const {
            return _obj["md5"].str();
        }

        BSONElement getFileField( const std::string& name ) const {
            return _obj[name];
        }

        BSONObj getMetadata() const;

        int getNumChunks() const {
            return (int) ceil( (double)getContentLength() / (double)getChunkSize() );
        }

        GridFSChunk getChunk( int n ) const;

        /**
           write the file to the output stream
         */
        gridfs_offset write( std::ostream & out ) const;

        /**
           write the file to this filename
         */
        gridfs_offset write( const std::string& where ) const;

    private:
        GridFile(const GridFS * grid , BSONObj obj );

        void _exists() const;

        const GridFS * _grid;
        BSONObj        _obj;

        friend class GridFS;
    };
    
    /**
     * class which allow to build GridFiles in a stream fashion way
     */
    class GridFileBuilder {
    public:
        /**
         * @param grid - gridfs instance
         */
        GridFileBuilder( GridFS* const grid );
        
        /**
         * Appends a chunk of data. Data will be split as many times as
         * necessary in chunkSize blocks. Sizes not multiple of chunkSize will
         * copy the reamining bytes to a pendingData pointer. In this way,
         * it is possible to add data in a stream fashion way.
         * @param data - C string with data
         * @param length - size of the string
         */
        void appendChunk( const char* data, size_t length );

        /**
         * Inserts the description of the file in GridFS collection. Note that
         * the stream will be reinitialized after the build call, so it will be
         * possible to continue appending data to build another file.
         * @param remoteName filename to use for file stored in GridFS
         * @param contentType optional MIME type for this object.
         *                    (default is to omit)
         * @return the file object
         */
        mongo::BSONObj buildFile( const std::string& remoteName,
                                  const std::string& contentType="" );
        
    private:
        GridFS* const _grid;
        const size_t _chunkSize; // taken from GridFS in the constructor
        unsigned int _currentChunk;
        OID _fileId;
        BSONObj _fileIdObj;
        boost::scoped_array<char> _pendingData; // pointer with _chunkSize space
        size_t _pendingDataSize;
        gridfs_offset _fileLength;

        const char* _appendChunk( const char* data, size_t length,
                                  bool forcePendingInsert );

        void _appendPendingData();
    };


}
