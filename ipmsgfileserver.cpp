#include "ipmsgfileserver.h"
#include <QThread>
#include <unistd.h>
#include "filetransworker.h"

IpMsgFileServer::IpMsgFileServer(QObject *parent) : QObject(parent)
{

    if(false == mServer.listen(QHostAddress::Any,2425))
    {
        qDebug()<<"Listen to port 2425 filead";
        mFileServerListen = false;
        return;
    }
    else {
        mFileServerListen = true;
    }
    connect(&mServer,SIGNAL(newConnection()),this,SLOT(fileServerNewConnection()));
}

void IpMsgFileServer::fileServerNewConnection()
{
    QTcpSocket *sock = mServer.nextPendingConnection();

    qDebug()<<"new connection:"<<__FUNCTION__<<QThread::currentThreadId();
    if(nullptr == sock)
        return;
    qDebug()<<"Client"<<sock->peerAddress().toString()<<"Port"<<sock->peerPort()<<"connected";
    connect(sock,SIGNAL(readyRead()),this,SLOT(fileServerReadyRead()));
    //connect(sock,SIGNAL(disconnected()),this,SLOT(fileServerSockDisconnect()));
    //connect(sock,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(fileServerSockError(QAbstractSocket::SocketError)));
    mSocks.append(sock);
}

void IpMsgFileServer::fileServerReadyRead()
{
    QByteArray buffer;
    QTcpSocket* readSocket = qobject_cast<QTcpSocket*>(sender());
    qDebug()<<"Client"<<readSocket->peerAddress()<<"Port"<<readSocket->peerPort()<<"send data";
    buffer = readSocket->readAll();
    qDebug()<<buffer;
    //readSocket->close();
    emit ipMsgFileServerDataReady(readSocket->peerAddress().toIPv4Address(),readSocket->peerPort(),buffer);
}

void IpMsgFileServer::fileServerSockDisconnect()
{
    QTcpSocket* srcSocket = qobject_cast<QTcpSocket*>(sender());
    qDebug()<<"Client"<<srcSocket->peerAddress().toString()<<"Port"<<srcSocket->peerPort()<<"disconnected";
    qDebug()<<"Tell send thread, stop send data to this client and delete sock and connect from queue";
    return;
}

void IpMsgFileServer::fileServerSockError(QAbstractSocket::SocketError error)
{
    QTcpSocket* srcSocket = qobject_cast<QTcpSocket*>(sender());
    qDebug()<<"Client"<<srcSocket->peerAddress().toString()<<"Port"<<srcSocket->peerPort()<<"error";
    qDebug()<<"Tell send thread, stop send data to this client and delete sock and connect from queue";
    return;
}

QTcpSocket* IpMsgFileServer::fileServerSockMatch(quint32 host, quint16 port)
{
    for(int i =0; i< mSocks.count();i++)
    {
        if(mSocks.at(i)->peerAddress().toIPv4Address() == host
                && mSocks.at(i)->peerPort() == port)
            return mSocks.at(i);
    }
    return nullptr;
}




