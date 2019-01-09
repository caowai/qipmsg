#ifndef IPMSGFILESERVER_H
#define IPMSGFILESERVER_H

#include <QObject>
#include <QHostAddress>
#include <QFileInfo>
#include <QTcpServer>
#include <QTcpSocket>
#include "ipmsgcommon.h"

class IpMsgFileServer : public QObject
{
    Q_OBJECT
public:
    explicit IpMsgFileServer(QObject *parent = nullptr);
    QTcpSocket* fileServerSockMatch(quint32 host, quint16 port);
    bool mFileServerListen = false;

signals:
    void ipMsgFileServerDataReady(quint32 ip,quint16 port,QByteArray data);
    void ipMsgFileServerSendFileFinished(quint32 fileId);

    void fileSeverFilerTransProgress(quint32 fileId,int val); // 进度信号，[0, 100],指示进度
    void fileServerFileTransFinished(quint32 fileId); // 结束信号
    void fileServerTransFinished();


public slots:
    void fileServerNewConnection();
    void fileServerReadyRead();
    void fileServerSockDisconnect();
    void fileServerSockError(QAbstractSocket::SocketError error);

private:
    QTcpServer    mServer;
    QList <fileEntryT *> fileSendQueue;
    QList <QTcpSocket *> mSocks;
    QThread *mSendThread  = nullptr;
};

#endif // IPMSGFILESERVER_H
