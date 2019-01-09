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

public slots:

};

#endif // IPMSGFILERECV_H
