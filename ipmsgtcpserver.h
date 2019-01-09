#ifndef IPMSGTCPSERVER_H
#define IPMSGTCPSERVER_H

#include <QObject>
#include <QTcpServer>

class IpMsgTcpServer : public QTcpServer
{
    Q_OBJECT

public:
    IpMsgTcpServer();
protected:
    //void incomingConnection(qintptr handle);


};

#endif // IPMSGTCPSERVER_H
