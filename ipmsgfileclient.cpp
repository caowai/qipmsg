#include "ipmsgfileclient.h"
#include "ipmsgudpsession.h"
#include <QTcpSocket>
#include <QDateTime>
extern quint32 g_pkg_seq;

IpMsgFileClient::IpMsgFileClient(IpMsgUser *user,QObject *parent) : QObject(parent)
{
    qDebug()<<__FUNCTION__<<"Thread"<<QThread::currentThreadId();
    me.userId = user->userId;
    me.userVer = user->userVer;
    me.userPktSeq = user->userPktSeq;
    me.userCfmSeq = user->userCfmSeq;
    me.userHostAddr = user->userHostAddr;
    me.userHostName = user->userHostName;
    me.userNickName = user->userNickName;
    me.userIcon = user->userIcon;
    me.userGroupName = user->userGroupName;
    mSize = 0;
}

void IpMsgFileClient::ipMsgFileClientStart()
{
    sock = new QTcpSocket();
    QHostAddress server;

    qDebug()<<__FUNCTION__<<"Thread"<<QThread::currentThreadId();

    server.setAddress(recvFileInfo.fileHost);
    connect(sock,SIGNAL(connected()),this,SLOT(ipMsgFileClientConnected()));
    connect(sock,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(ipMsgFileClientError(QAbstractSocket::SocketError)));
    connect(sock,SIGNAL(disconnected()),this,SLOT(ipMsgFileClientDisconnected()));
    connect(sock,SIGNAL(readyRead()),this,SLOT(ipMsgFileClientRecv()));
    sock->connectToHost(server,2425);

    mProgress = 0;
    if(false == sock->waitForConnected(5000))
    {
        emit ipMsgFileClientErrorSig(recvFileInfo.fileId,mProgress);
        sock->disconnectFromHost();
    }
}

void IpMsgFileClient::ipMsgFileClientClose()
{
    if(nullptr!=sock
            && sock->isOpen())
    {
        if(sock->state() != QAbstractSocket::UnconnectedState)
        {
            sock->disconnectFromHost();
            return;
        }
    }
}

void IpMsgFileClient::ipMsgFileClientConnected()
{
    QByteArray fileReq;
    qDebug()<<__FUNCTION__<<"Thread"<<QThread::currentThreadId();
    QTcpSocket* sock = qobject_cast<QTcpSocket*>(sender());
    qDebug()<<"Connected to"<<sock->peerAddress()<<sock->peerPort();

    fileReq.append(me.userVer+":");
    fileReq.append(QString::number(g_pkg_seq++,10)+":");
    //fileReq.append(QString::number(time(nullptr),10)+":");
    fileReq.append(me.userId+":");
    fileReq.append(me.userHostName+":");
    fileReq.append(QString::number(IPMSG_GETFILEDATA,10)+":");
    fileReq.append(QString::number(me.userPktSeq,16)+":");
    fileReq.append(QString::number(recvFileInfo.fileId,16)+":");
    fileReq.append("0:");
    fileReq.append((char)0);
    qDebug()<<"TCP send"<<fileReq;
    if(fileReq.length() == sock->write(fileReq)
            && sock->waitForBytesWritten() == true  )
    {
        mFile.setFileName(recvFileInfo.info.absoluteFilePath);
        if(false == mFile.open(QIODevice::WriteOnly))
        {
            sock->disconnectFromHost();
        }
    }
}

void IpMsgFileClient::ipMsgFileClientRecv()
{
    QByteArray data;
    qint64 len;
    QDateTime timestamp;
    data.clear();
    data.resize(8192);


    QTcpSocket* sock = qobject_cast<QTcpSocket*>(sender());

    len = sock->read(data.data(),8192);
    //data = sock->read(8192);

    if(len >0)
    {
        if(mFile.write(data.data(),len)<0)
        {
            mFile.close();
            sock->disconnectFromHost();;
        }
        mSize+=len;
    }
    else if(len <0)
    {
       mFile.close();
       sock->disconnectFromHost();
       return;
    }
    else
    {
       qDebug()<<recvFileInfo.info.fileName<<mSize<<" 0";
       return;
    }

    if(mProgress != (mSize*100)/recvFileInfo.info.size)
    {
        mProgress = (mSize*100)/recvFileInfo.info.size;
        emit ipMsgFileClientProgressSig(recvFileInfo.fileId,mProgress);
    }

    //qDebug()<<recvFileInfo.info.fileName<<mSize;
    if(mSize >= recvFileInfo.info.size)
    {
        qDebug()<<"Receive"<<recvFileInfo.info.fileName<<"finished";
        timestamp.setTime_t(recvFileInfo.info.metadataChangeTime);
        mFile.close();
        mFile.setFileTime(timestamp,QFileDevice::FileModificationTime);
        //qDebug() << timestamp.toString(Qt::SystemLocaleShortDate);
        sock->disconnectFromHost();
        return;
    }
}

void IpMsgFileClient::ipMsgFileClientDisconnected()
{
   qDebug()<<__FUNCTION__<<"Thread"<<QThread::currentThreadId();
   QTcpSocket* sock = qobject_cast<QTcpSocket*>(sender());
   if(mFile.isOpen())
       mFile.close();
   if(sock->isOpen())
       sock->close();

   if(mSize == recvFileInfo.info.size)
   {
       emit ipMsgFileClientFinishSig(recvFileInfo.fileId);
   }
   else if(mSize < recvFileInfo.info.size)
   {
       emit ipMsgFileClientErrorSig(recvFileInfo.fileId,mProgress);
   }
   else
   {
       emit ipMsgFileClientErrorSig(recvFileInfo.fileId,100);
   }
   QThread::currentThread()->quit();
}

void IpMsgFileClient::ipMsgFileClientError(QAbstractSocket::SocketError error)
{
    qDebug()<<__FUNCTION__<<"Thread"<<QThread::currentThreadId();
    qDebug()<<"Error:"<<error;
    QTcpSocket* sock = qobject_cast<QTcpSocket*>(sender());
    if(mFile.isOpen())
        mFile.close();
    if(sock->isOpen())
        sock->close();

    if(mSize == recvFileInfo.info.size)
    {
        emit ipMsgFileClientFinishSig(recvFileInfo.fileId);
    }
    else if(mSize < recvFileInfo.info.size)
    {
        emit ipMsgFileClientErrorSig(recvFileInfo.fileId,mProgress);
    }
    else
    {
        emit ipMsgFileClientErrorSig(recvFileInfo.fileId,100);
    }
    QThread::currentThread()->quit();
}

