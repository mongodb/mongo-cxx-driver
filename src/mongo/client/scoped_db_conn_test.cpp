/*    Copyright 2012 10gen Inc.
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

#include "mongo/base/init.h"
#include "mongo/client/connpool.h"
#include "mongo/client/dbclientcursor.h"
#include "mongo/db/dbmessage.h"
#include "mongo/platform/cstdint.h"
#include "mongo/util/fail_point_service.h"
#include "mongo/util/time_support.h"
#include "mongo/util/timer.h"
#include "mongo/unittest/unittest.h"

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/thread.hpp>

// WIN32 headers included by platform/basic.h
#if defined(_WIN32)
    // Initialize Winsock
    struct WinsockInit {
        WinsockInit() {

            WSADATA wsaData;
            int iResult;

            iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
            if (iResult != 0) {
                printf("WSAStartup failed: %d\n", iResult);
            }
        }

        ~WinsockInit() { WSACleanup(); }
    } winsock_init;
#else
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <fcntl.h>
    #include <sys/time.h>
#endif

/**
 * Tests for ScopedDbConnection, particularly in connection pool management.
 * The tests also indirectly tests DBClientConnection's failure detection
 * logic (hence the use of the dummy server as opposed to mocking the
 * connection).
 */

using boost::scoped_ptr;
using mongo::DBClientBase;
using mongo::FailPoint;
using mongo::ScopedDbConnection;
using std::string;
using std::vector;

namespace {
    const string TARGET_HOST = "localhost:27017";
    const int TARGET_PORT = 27017;
}

namespace mongo {

    struct SocketAddress {
        SocketAddress() {
            size = sizeof(address);
            memset(&address, 0, size);
        }

        SocketAddress(int port) {
            size = sizeof(address);
            memset(&address, 0, size);
            address.sin_family = AF_INET;
            address.sin_port = htons(port);
            address.sin_addr.s_addr = INADDR_ANY;
        }

        sockaddr* get_address() {
            return reinterpret_cast<sockaddr*>(&address);
        }

        socklen_t size;
        sockaddr_in address;
    };

    class TCPSocket {
        public:
            TCPSocket(SocketAddress sa, int fd) : _sa(sa), _fd(fd), _closed(false) { }
            TCPSocket(int port) : _sa(port), _closed(false) { init(); }
            ~TCPSocket() { close(); }

            void init() {
                _fd = socket(PF_INET, SOCK_STREAM, 0);

                if (_fd == -1) {
                    perror("Failed to get file descriptor");
                    std::abort();
                }
            }

            void set_opt(int optname, int optval=true) {
                char * p_opt = reinterpret_cast<char*>(&optval);
                int optset = ::setsockopt(_fd, SOL_SOCKET, optname, p_opt, sizeof(optval));
                if (optset < 0) {
                    perror("Failed to set socket opts");
                    close();
                    std::abort();
                }
            }

            void bind() {
                int bound = ::bind(_fd, _sa.get_address(), _sa.size);
                if (bound != 0) {
                    perror("Failed to bind");
                    close();
                    std::abort();
                }
            }

            void listen() {
                int listening = ::listen(_fd, 1);
                if (listening != 0) {
                    perror("Failed to listen");
                    close();
                    std::abort();
                }
            }

            int send(MsgData* toSend, int len) {
                int sent = ::send(_fd, (char*)toSend, len, 0);
                if (sent == -1) {
                    perror("send");
                    close();
                    std::abort();
                }
                return sent;
            }

            int recv(char* buffer, int len) {
                int received = ::recv(_fd, buffer, len, 0);
                if (received == -1) {
                    perror("recv");
                    close();
                    std::abort();
                }
                return received;
            }

            void close() {
                if (!_closed) {
#if defined(_WIN32)
                    ::shutdown(_fd, SD_BOTH);
                    ::closesocket(_fd);
#else
                    ::shutdown(_fd, SHUT_RDWR);
                    ::close(_fd);
#endif
                    _closed = true;
                    _fd = -1;
                }
            }

            int raw() {
                return _fd;
            }

            TCPSocket* accept() {
                SocketAddress client_sa;
                int client_fd;
                if ((client_fd = ::accept(_fd, client_sa.get_address(), &client_sa.size)) == -1) {
                    perror("accept");
                    std::abort();
                }
                return new TCPSocket(client_sa, client_fd);
            }

        private:
            SocketAddress _sa;
            int _fd;
            bool _closed;
    };

    class RequestHandler {
    public:
        RequestHandler(TCPSocket* socket) : _socket(socket) {}
        ~RequestHandler() {}
        void operator()() { _handle_request(); }

    private:
        void _handle_request() {
            char msglen_bytes[sizeof(int32_t)];
            _socket->recv(msglen_bytes, sizeof(int32_t));
            int msglen;
            memcpy(&msglen, msglen_bytes, sizeof(int32_t));

            boost::scoped_array<char> buffer(new char[msglen]);
            memcpy(buffer.get(), &msglen, sizeof(int32_t));

            int32_t position = sizeof(int32_t);
            while (position < msglen) {
                int got = _socket->recv(buffer.get() + position, (msglen - position));
                position += got;
            }

            int32_t request_id = _extract_request_id(buffer.get());

            Message reply;
            replyToQuery(0, reply, _build_is_master());
            MsgData* toSend = reply.singleData();
            toSend->responseTo = request_id;
            _send(toSend);
        }

        void _send(MsgData* toSend) {
            int left = toSend->len;
            while (left > 0) {
                int sent = _socket->send(toSend + (toSend->len - left), left);
                left -= sent;
            }
        }

        int _extract_request_id(char* buffer) {
            int32_t request_id;
            memcpy(&request_id, buffer + sizeof(int32_t), sizeof(int32_t));
            return request_id;
        }

        BSONObj _build_is_master() {
            BSONObjBuilder isMaster;
            isMaster.append("ismaster", true);
            isMaster.append("ok", true);
            return isMaster.obj();
        }

        TCPSocket* _socket;
    };

    class TCPServer {
        public:
            TCPServer(int port) : _server_sock(port), _running(true) {
                _server_sock.set_opt(SO_REUSEADDR);
                _server_sock.bind();
                _server_sock.listen();
            }

            ~TCPServer() {
                close_clients();
                join_threads();
                _server_sock.close();
            }

            bool is_running() {
                boost::lock_guard<boost::mutex> lock(_mutex);
                return _running;
            }

            void start() {
                int server_fd = _server_sock.raw();
                const timeval delay = {0, 10000};
                fd_set server_fd_set;
                fd_set readable_fd_set;
                FD_ZERO(&server_fd_set);
                FD_SET(server_fd, &server_fd_set);
                readable_fd_set = server_fd_set;

                while (is_running()) {
                    timeval t = delay;
                    int selected = ::select(server_fd + 1, &readable_fd_set, NULL, NULL, &t);

                    if (selected == -1) {
                        perror("select");
                        std::abort();
                    }
                    else if (selected > 0 && FD_ISSET(server_fd, &readable_fd_set)) {
                        TCPSocket* p_new_sock = _server_sock.accept();
                        _client_socks.push_back(p_new_sock);
                        _threads.push_back(new boost::thread(RequestHandler(p_new_sock)));
                    }

                    // reset readable_fd_set and t
                    readable_fd_set = server_fd_set;
                }
            }

            void stop() {
                boost::lock_guard<boost::mutex> lock(_mutex);
                _running = false;
            }

            void join_threads() {
                vector<boost::thread*>::iterator it;
                for (it = _threads.begin(); it != _threads.end(); ++it) {
                    (*it)->join();
                    delete *it;
                }
            }

            void close_clients() {
                vector<TCPSocket*>::iterator it;
                for (it = _client_socks.begin(); it != _client_socks.end(); ++it)
                    delete *it;
            }

            void operator()() {
                start();
            }

        private:
            vector<TCPSocket*> _client_socks;
            vector<boost::thread*> _threads;
            TCPSocket _server_sock;
            boost::mutex _mutex;
            bool _running;
    };

    class DummyServerFixture : public ::testing::Test {
        public:
            DummyServerFixture() {
                _server = new TCPServer(TARGET_PORT);
                _thread = new boost::thread(boost::ref(*_server));
                _maxPoolSizePerHost = mongo::pool.getMaxPoolSize();
            }

            ~DummyServerFixture() {
                mongo::pool.setMaxPoolSize(_maxPoolSizePerHost);
                mongo::ScopedDbConnection::clearPool();

                _server->stop();
                _thread->join();

                delete _thread;
                delete _server; // refered to server must outlive thread
            }

        protected:
            static void assertGreaterThan(uint64_t a, uint64_t b) {
                ASSERT_GREATER_THAN(a, b);
            }

            static void assertNotEqual(uint64_t a, uint64_t b) {
                ASSERT_NOT_EQUALS(a, b);
            }

            /**
            * Tries to grab a series of connections from the pool, perform checks on
            * them, then put them back into the pool. After that, it checks these
            * connections can be retrieved again from the pool.
            *
            * @param checkFunc method for comparing new connections and arg2.
            * @param arg2 the value to pass as the 2nd parameter of checkFunc.
            * @param newConnsToCreate the number of new connections to make.
            */
            void checkNewConns(void (*checkFunc)(uint64_t, uint64_t), uint64_t arg2,
                               size_t newConnsToCreate) {

                vector<ScopedDbConnection*> newConnList;

                for (size_t x = 0; x < newConnsToCreate; x++) {
                    ScopedDbConnection* newConn = new ScopedDbConnection(TARGET_HOST);
                    checkFunc(newConn->get()->getSockCreationMicroSec(), arg2);
                    newConnList.push_back(newConn);
                }

                const uint64_t oldCreationTime = mongo::curTimeMicros64();

                for (vector<ScopedDbConnection*>::iterator iter = newConnList.begin();
                        iter != newConnList.end(); ++iter) {
                    (*iter)->done();
                    delete *iter;
                }
                newConnList.clear();

                // Check that connections created after the purge was put back to the pool.
                for (size_t x = 0; x < newConnsToCreate; x++) {
                    ScopedDbConnection* newConn = new ScopedDbConnection(TARGET_HOST);
                    ASSERT_LESS_THAN(newConn->get()->getSockCreationMicroSec(), oldCreationTime);
                    newConnList.push_back(newConn);
                }

                for (vector<ScopedDbConnection*>::iterator iter = newConnList.begin();
                        iter != newConnList.end(); ++iter) {
                    (*iter)->done();
                    delete *iter;
                }
            }

        private:
            TCPServer* _server;
            boost::thread* _thread;
            uint32_t _maxPoolSizePerHost;
    };

    TEST_F(DummyServerFixture, BasicScopedDbConnection) {
        ScopedDbConnection conn1(TARGET_HOST);
        ScopedDbConnection conn2(TARGET_HOST);

        DBClientBase* conn1Ptr = conn1.get();
        conn1.done();

        ScopedDbConnection conn3(TARGET_HOST);
        ASSERT_EQUALS(conn1Ptr, conn3.get());

        conn2.done();
        conn3.done();
    }

    TEST_F(DummyServerFixture, InvalidateBadConnInPool) {
        ScopedDbConnection conn1(TARGET_HOST);
        ScopedDbConnection conn2(TARGET_HOST);
        ScopedDbConnection conn3(TARGET_HOST);

        conn1.done();
        conn3.done();

        const uint64_t badCreationTime = mongo::curTimeMicros64();

        mongo::getGlobalFailPointRegistry()->getFailPoint("throwSockExcep")->
                setMode(FailPoint::alwaysOn);

        try {
            conn2->query("test.user", mongo::Query());
        }
        catch (const mongo::SocketException&) {
        }

        mongo::getGlobalFailPointRegistry()->getFailPoint("throwSockExcep")->
                setMode(FailPoint::off);
        conn2.done();

        checkNewConns(assertGreaterThan, badCreationTime, 10);
    }

    TEST_F(DummyServerFixture, DontReturnKnownBadConnToPool) {
        ScopedDbConnection conn1(TARGET_HOST);
        ScopedDbConnection conn2(TARGET_HOST);
        ScopedDbConnection conn3(TARGET_HOST);

        conn1.done();

        mongo::getGlobalFailPointRegistry()->getFailPoint("throwSockExcep")->
                setMode(FailPoint::alwaysOn);

        try {
            conn3->query("test.user", mongo::Query());
        }
        catch (const mongo::SocketException&) {
        }

        mongo::getGlobalFailPointRegistry()->getFailPoint("throwSockExcep")->
                setMode(FailPoint::off);

        const uint64_t badCreationTime = conn3->getSockCreationMicroSec();
        conn3.done();
        // attempting to put a 'bad' connection back to the pool
        conn2.done();

        checkNewConns(assertGreaterThan, badCreationTime, 10);
    }

    TEST_F(DummyServerFixture, InvalidateBadConnEvenWhenPoolIsFull) {
        mongo::pool.setMaxPoolSize(2);

        ScopedDbConnection conn1(TARGET_HOST);
        ScopedDbConnection conn2(TARGET_HOST);
        ScopedDbConnection conn3(TARGET_HOST);

        conn1.done();
        conn3.done();

        const uint64_t badCreationTime = mongo::curTimeMicros64();

        mongo::getGlobalFailPointRegistry()->getFailPoint("throwSockExcep")->
                setMode(FailPoint::alwaysOn);

        try {
            conn2->query("test.user", mongo::Query());
        }
        catch (const mongo::SocketException&) {
        }

        mongo::getGlobalFailPointRegistry()->getFailPoint("throwSockExcep")->
                setMode(FailPoint::off);
        conn2.done();

        checkNewConns(assertGreaterThan, badCreationTime, 2);
    }

    TEST_F(DummyServerFixture, DontReturnConnGoneBadToPool) {
        ScopedDbConnection conn1(TARGET_HOST);

        const uint64_t conn1CreationTime = conn1->getSockCreationMicroSec();

        uint64_t conn2CreationTime = 0;

        {
            ScopedDbConnection conn2(TARGET_HOST);
            conn2CreationTime = conn2->getSockCreationMicroSec();

            conn1.done();
            // conn2 gets out of scope without calling done()
        }

        // conn2 should not have been put back into the pool but it should
        // also not invalidate older connections since it didn't encounter
        // a socket exception.

        ScopedDbConnection conn1Again(TARGET_HOST);
        ASSERT_EQUALS(conn1CreationTime, conn1Again->getSockCreationMicroSec());

        checkNewConns(assertNotEqual, conn2CreationTime, 10);

        conn1Again.done();
    }
}
