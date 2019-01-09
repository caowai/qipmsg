#include "filetransworker.h"
#include <QFile>
#include <QThread>
#include <QHostAddress>
#include <QFileInfo>
FileTransWorker::FileTransWorker(QObject *parent) : QObject(parent)
{
}
void FileTransWorker::fileTransStart()
{
    QByteArray data;
    qint64 ret = 0;
    int wr_len = 0;
    QFile file;
    qint64 fileSend = 0;
    qint64 fileSize = 0;

    QTcpSocket *mSock = new QTcpSocket();

    mSock->setSocketDescriptor(mHandle);

    qDebug()<<"will trans File"<<mFilename<<"Sock"<<mHandle<<mSock->peerAddress().toString()<<mSock->peerPort();

    if(nullptr != mSock
            && mSock->isValid()
            && false == mFileTranStopFlag
            && true == QFileInfo(mFilename).isFile()
            )
    {
            //qDebug()<<"File"<<mFilename<<"Sock"<<mSock->peerAddress().toString()<<mSock->peerPort();

            file.setFileName(mFilename);
            fileSize = file.size();
            if(file.open(QFile::ReadOnly))
            {
                while(false == mFileTranStopFlag
                      && false == file.atEnd()
                      && mSock->isValid())
                {
                    data.clear();
                    data.resize(0);
                    data = file.read(8192);
                    if(data.length()>0)
                    {
                        wr_len = 0;
                        while(wr_len!=data.length())
                        {
                            ret = mSock->write(data.right(data.length()-wr_len),data.length()-wr_len);
                            if( -1 != ret)
                            {
                                wr_len+=ret;
                                fileSend+=ret;
                                if((fileSend*100/fileSize)!=mProgress)
                                {
                                    mProgress = (fileSend*100/fileSize);
                                    emit fileTransProgress(mFileId,mProgress);
                                }
                                mSock->waitForBytesWritten();
                                continue;
                            }
                            else
                            {
                                break;
                            }
                        }
                        if( -1 == ret)
                        {
                            emit fileTransError(mFileId,mProgress);
                            break;
                        }
                    }
                    else
                    {
                        emit fileTransError(mFileId,mProgress);
                        break;
                    }
                }
                file.close();
                emit fileTransFinished(mFileId);
            }
    }
    else
    {
        emit fileTransError(mFileId,mProgress);
    }
    if(mSock->state() == QTcpSocket::ConnectedState)
    {
        mSock->disconnectFromHost();
    }

    if(mSock->state() != QTcpSocket::UnconnectedState)
    {
        mSock->waitForDisconnected();
    }
    mSock->close();
    delete mSock;
    return;
}


