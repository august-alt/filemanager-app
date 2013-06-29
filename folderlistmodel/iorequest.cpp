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

#include "iorequest.h"

#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>

IORequest::IORequest() : QObject(), m_type(DirList)
{
}

IORequest::RequestType IORequest::type() const
{
    return m_type;
}


DirListWorker::DirListWorker(const QString &pathName, QDir::Filter filter, const bool isRecursive)
    : mPathName(pathName)
    , mFilter(filter)
    , mIsRecursive(isRecursive)
{

}


void DirListWorker::run()
{
#if DEBUG_MESSAGES
    qDebug() << Q_FUNC_INFO << "Running on: " << QThread::currentThreadId();
#endif

    QVector<QFileInfo> directoryContents = getContents();

    // last batch
    emit itemsAdded(directoryContents);
    emit workerFinished();
}


QVector<QFileInfo> DirListWorker::getContents()
{
    QVector<QFileInfo> directoryContents;
    directoryContents = add(mPathName, mFilter, mIsRecursive, directoryContents);
    return directoryContents;
}


QVector<QFileInfo> DirListWorker::add(const QString &pathName,
                                      QDir::Filter filter,
                                      const bool isRecursive,
                                      QVector<QFileInfo> directoryContents)
{
    QDir tmpDir = QDir(pathName, QString(), QDir::NoSort, filter);
    QDirIterator it(tmpDir);
    while (it.hasNext()) {
        it.next();

        if(it.fileInfo().isDir() && isRecursive) {
            directoryContents = add(it.fileInfo().filePath(), filter, isRecursive, directoryContents);
        } else {
            directoryContents.append(it.fileInfo());
        }

        if (directoryContents.count() >= 50) {
            emit itemsAdded(directoryContents);

            // clear() would force a deallocation, micro-optimization
            directoryContents.erase(directoryContents.begin(), directoryContents.end());
        }
    }

    return directoryContents;
}


//---------------------------------------------------------------------------------------------------------
ExternalFileSystemChangesWorker::ExternalFileSystemChangesWorker(const QVector<QFileInfo>& content,
                                                   const QString &pathName,
                                                   QDir::Filter filter,
                                                   const bool isRecursive)
    : DirListWorker(pathName, filter, isRecursive)

{
    m_type        = DirListExternalFSChanges;
    m_curContent  = content;
}


void ExternalFileSystemChangesWorker::run()
{
    QVector<QFileInfo> directoryContents = getContents();
}
