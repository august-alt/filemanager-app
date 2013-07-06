/*
 * Copyright (C) 2012 Robin Burchell <robin+nemo@viroteck.net>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#include "iorequestworker.h"
#include "iorequest.h"

#include <QMutexLocker>
#include <QDateTime>
#include <QDebug>

/*!
  Lives on an IOWorkerThread.

  Responsible for running IORequest jobs on the thread instance, and
  disposing of their resources once they are done.
 */
IORequestWorker::IORequestWorker()
    : QThread()
    , mTimeToQuit(false)
{
}

void IORequestWorker::addRequest(IORequest *request)
{
#if DEBUG_EXT_FS_WATCHER
        qDebug() << "[exfsWatcher]" << QDateTime::currentDateTime().toString("hh:mm:ss.zzz")
                 << Q_FUNC_INFO;
#endif

    request->moveToThread(this);

    // TODO: queue requests so we run the most important one first
    QMutexLocker lock(&mMutex);
    mRequests.append(request);

    // wake run()
    mWaitCondition.wakeOne();
}

void IORequestWorker::run()
{
    forever {
        QMutexLocker lock(&mMutex);

        if (mTimeToQuit)
            return;

        if (mRequests.empty())
            mWaitCondition.wait(&mMutex);

        int counter = 0;
        while (!mRequests.isEmpty())
        {
           /* If there are more than one thread in the queue and there is at least one
            * of the type Auto Refresh, do:
            *  1. if all threads are type Auto Refresh (leave just one) it might not happen but we check it here
            *  2. if there is at least one type DirList, remove all type Auto Refresh, the user might have changed
            *      the current dir when the Auto Refresh was already in the queue
            */
            if ( (counter=mRequests.count()) > 1)
            {
               int autoRefreshCounter = 0;
               for(int aux=0; aux < counter; aux++)
               {
                   if (mRequests.at(aux)->type() == IORequest::DirListExternalFSChanges)
                   {
                       autoRefreshCounter++;
                   }
               }
               if (autoRefreshCounter)
               {
                   removeAutoRefreshThread( (autoRefreshCounter == counter)
                                            ? counter -1
                                            : autoRefreshCounter
                                              );
               }
            }
            IORequest *request = mRequests.takeFirst();            
            lock.unlock();
            request->run();
            request->deleteLater();
            lock.relock();
        }
    }
}


void IORequestWorker::exit()
{
#if DEBUG_MESSAGES
    qDebug() << Q_FUNC_INFO << "Quitting";
#endif
    QMutexLocker lock(&mMutex);
    mTimeToQuit = true;
    mWaitCondition.wakeOne();
}



void IORequestWorker::removeAutoRefreshThread(int toRemoveCounter)
{
    int counter = mRequests.count();
    while(counter-- && toRemoveCounter)
    {
       if (mRequests.at(counter)->type() == IORequest::DirListExternalFSChanges)
       {
           --toRemoveCounter;
           IORequest *request = mRequests.takeAt(counter);
           delete request;
       }
    }
}
