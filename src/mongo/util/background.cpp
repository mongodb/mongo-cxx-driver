// @file background.cpp

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

#define MONGO_LOG_DEFAULT_COMPONENT ::mongo::logger::LogComponent::kCommand

#include "mongo/platform/basic.h"

#include "mongo/util/background.h"

#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/once.hpp>
#include <boost/thread/thread.hpp>

#include "mongo/stdx/functional.h"
#include "mongo/util/debug_util.h"
#include "mongo/util/log.h"
#include "mongo/util/mongoutils/str.h"
#include "mongo/util/net/ssl_manager.h"
#include "mongo/util/time_support.h"
#include "mongo/util/timer.h"

using namespace std;
namespace mongo {

// both the BackgroundJob and the internal thread point to JobStatus
struct BackgroundJob::JobStatus {
    JobStatus() : mutex(), state(NotStarted) {}

    boost::mutex mutex;
    boost::condition_variable done;
    State state;
};

BackgroundJob::BackgroundJob(bool selfDelete) : _selfDelete(selfDelete), _status(new JobStatus) {}

BackgroundJob::~BackgroundJob() {}

void BackgroundJob::jobBody() {
    LOG(1) << "BackgroundJob starting" << endl;

    try {
        run();
    } catch (const std::exception& e) {
        error() << "backgroundjob " << e.what();
        throw;
    }

    // We must cache this value so that we can use it after we leave the following scope.
    const bool selfDelete = _selfDelete;

#ifdef MONGO_SSL
    // TODO(sverch): Allow people who use the BackgroundJob to also specify cleanup tasks.
    // Currently the networking code depends on this class and this class depends on the
    // networking code because of this ad hoc cleanup.
    SSLManagerInterface* manager = getSSLManager();
    if (manager)
        manager->cleanupThreadLocals();
#endif

    {
        // It is illegal to access any state owned by this BackgroundJob after leaving this
        // scope, with the exception of the call to 'delete this' below.
        boost::lock_guard<boost::mutex> l(_status->mutex);
        _status->state = Done;
        _status->done.notify_all();
    }

    if (selfDelete)
        delete this;
}

void BackgroundJob::go() {
    boost::lock_guard<boost::mutex> l(_status->mutex);
    massert(17234,
            mongoutils::str::stream() << "backgroundJob already running: " << name(),
            _status->state != Running);

    // If the job is already 'done', for instance because it was cancelled or already
    // finished, ignore additional requests to run the job.
    if (_status->state == NotStarted) {
        boost::thread t(stdx::bind(&BackgroundJob::jobBody, this));
        _status->state = Running;
        t.detach();
    }
}

Status BackgroundJob::cancel() {
    boost::lock_guard<boost::mutex> l(_status->mutex);

    if (_status->state == Running)
        return Status(ErrorCodes::IllegalOperation, "Cannot cancel a running BackgroundJob");

    if (_status->state == NotStarted) {
        _status->state = Done;
        _status->done.notify_all();
    }

    return Status::OK();
}

namespace {
inline boost::xtime incxtimemillis(long long s) {
    boost::xtime xt;
    boost::xtime_get(&xt, MONGO_BOOST_TIME_UTC);
    xt.sec += (int)(s / 1000);
    xt.nsec += (int)((s % 1000) * 1000000);
    if (xt.nsec >= 1000000000) {
        xt.nsec -= 1000000000;
        xt.sec++;
    }
    return xt;
}
}  // namespace

bool BackgroundJob::wait(unsigned msTimeOut) {
    verify(!_selfDelete);  // you cannot call wait on a self-deleting job
    boost::unique_lock<boost::mutex> l(_status->mutex);
    while (_status->state != Done) {
        if (msTimeOut) {
            boost::xtime deadline = incxtimemillis(msTimeOut);
            if (!_status->done.timed_wait(l, deadline))
                return false;
        } else {
            _status->done.wait(l);
        }
    }
    return true;
}

BackgroundJob::State BackgroundJob::getState() const {
    boost::lock_guard<boost::mutex> l(_status->mutex);
    return _status->state;
}

bool BackgroundJob::running() const {
    boost::lock_guard<boost::mutex> l(_status->mutex);
    return _status->state == Running;
}

}  // namespace mongo
