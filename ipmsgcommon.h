#ifndef IPMSGCOMMON_H
#define IPMSGCOMMON_H

#include <QCoreApplication>
#include <QDir>

#define IPMSG_VERSION   "1_QIPMSGV01"

#define IPMSG_DEFAULT_USERNAME  "QIPMSG.U"
#define IPMSG_DEFAULT_NICKNAME  "QIPMSG.N"
#define IPMSG_DEFAULT_GROUPNAME "QIPMSG"

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
#endif // IPMSGCOMMON_H
