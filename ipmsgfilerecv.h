#ifndef IPMSGFILERECV_H
#define IPMSGFILERECV_H

#include "ipmsgfileclient.h"
#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include "ipmsgcommon.h"

class IpMsgFileRecv : public QObject
{
    Q_OBJECT
public:
    explicit IpMsgFileRecv(IpMsgUser *user,fileEntryT *file,QObject *parent = nullptr);
    IpMsgFileClient *client = nullptr;
signals:
    void fileRecvCancel();

public slots:
    void ipMsgFileClientCancel(fileEntryT *file);

};

#endif // IPMSGFILERECV_H
