#ifndef IPMSGTCPSESSION_H
#define IPMSGTCPSESSION_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QTcpServer>
#include <QDataStream>

class IpMsgTcpSession : public QObject
{
    Q_OBJECT
public:
    explicit IpMsgTcpSession(QObject *parent = nullptr);

signals:

public slots:

private:
    //Incoming. Outgoing
    qint32       tcpDir;
    QHostAddress destHost;
    uint16_t     destPort;
    QByteArray fileName;
    QByteArray destFileName;
    //Connecting, Connected,CloseByRemote,Error,Disconnected
    qint32   sessionStatus;
    //Wait,Receiving,Sending
    qint32   dataStatus;
    //0 - 100
    qint32   filePersent;
    QTcpServer tcpServer;

};

#endif // IPMSGTCPSESSION_H
