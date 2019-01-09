#ifndef IPMSGFILECLIENT_H
#define IPMSGFILECLIENT_H

#include <QObject>
#include <QHostAddress>
#include <QFileInfo>

#include "filetransworker.h"


#define FILE_TRANS_STATUS_IDLE 0
#define FILE_TRANS_STATUS_QUEUE 1
#define FILE_TRANS_STATUS_PRERUN 2
#define FILE_TRANS_STATUS_RUN 3
#define FILE_TRANS_STATUS_FINISHED 4

typedef struct fileEntryInfoT
{
    qint64 size;
    quint32 permission;
    QString fileName;
    QString absoluteFilePath;
    quint32 fileType;
    quint32 metadataChangeTime;
}fileEntryInfoT;

//QFileInfo a;

typedef struct fileEntryT
{
    bool        fileOut;
    quint32     fileId;
    fileEntryInfoT    info;
    quint64     fileOffset;
    //If recv file, fileHost is source
    //If send file, fileHost is dest
    quint32      fileHost;
    quint16      filePort;
    //IDLE,SENDING,FINISHED. -- send file
    //IDLE,RECEIVING,FINISHED -- recv file
    quint32     fileTranStatus;
    bool         fileTranStopFlag;
}fileEntryT;

class IpMsgFileClient : public QObject
{
    Q_OBJECT
public:
    explicit IpMsgFileClient(QObject *parent = nullptr);
    fileEntryT fileInfo;

signals:

public slots:

private:
    //fileEntryT recvFileInfo;
};

#endif // IPMSGFILECLIENT_H
