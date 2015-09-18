// @file background.h

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

#include <boost/scoped_ptr.hpp>
#include <boost/utility.hpp>
#include <string>
#include <vector>

#include "mongo/base/status.h"

namespace mongo {

/**
 *  Background thread dispatching.
 *  subclass and define run()
 *
 *  It is not possible to run the job more than once. An attempt to call 'go' while the
 *  task is running will fail. Calling 'go' after the task has finished are ignored and
 *  will not start the job again.
 *
 *  Thread safety: Note that when the job destructs, the thread is not terminated if still
 *  running. Generally, if the thread could still be running, allocate the job dynamically
 *  and set deleteSelf to true.
 *
 *  The overridden run() method will be executed on the background thread, so the
 *  BackgroundJob object must exist for as long the background thread is running.
 */

class BackgroundJob : boost::noncopyable {
protected:
    /**
     * sub-class must instantiate the BackgroundJob
     *
     * @param selfDelete if set to true, object will destruct itself after the run() finished
     * @note selfDelete instances cannot be wait()-ed upon
     */
    explicit BackgroundJob(bool selfDelete = false);

    virtual std::string name() const = 0;

    /**
     * define this to do your work.
     * after this returns, state is set to done.
     * after this returns, deleted if deleteSelf true.
     *
     * NOTE:
     *   if run() throws, the exception will be caught within 'this' object and will ultimately lead
     *   to the BackgroundJob's thread being finished, as if run() returned.
     *
     */
    virtual void run() = 0;

public:
    enum State { NotStarted, Running, Done };

    virtual ~BackgroundJob();

    /**
     * starts job.
     * returns immediately after dispatching.
     *
     * @note the BackgroundJob object must live for as long the thread is still running, ie
     * until getState() returns Done.
     */
    void go();


    /**
     * If the job has not yet started, transitions the job to the 'done' state immediately,
     * such that subsequent calls to 'go' are ignored, and notifies any waiters waiting in
     * 'wait'. If the job has already been started, this method returns a not-ok status: it
     * does not cancel running jobs. For this reason, you must still call 'wait' on a
     * BackgroundJob even after calling 'cancel'.
     */
    Status cancel();

    /**
     * wait for completion.
     *
     * @param msTimeOut maximum amount of time to wait in milliseconds
     * @return true if did not time out. false otherwise.
     *
     * @note you can call wait() more than once if the first call times out.
     * but you cannot call wait on a self-deleting job.
     */
    bool wait(unsigned msTimeOut = 0);

    // accessors. Note that while the access to the internal state is synchronized within
    // these methods, there is no guarantee that the BackgroundJob is still in the
    // indicated state after returning.
    State getState() const;
    bool running() const;

private:
    const bool _selfDelete;

    struct JobStatus;
    const boost::scoped_ptr<JobStatus> _status;

    void jobBody();
};

}  // namespace mongo
