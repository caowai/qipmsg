#include "ipmsgfilerecv.h"


IpMsgFileRecv::IpMsgFileRecv(IpMsgUser *user,fileEntryT *file, QObject *parent) : QObject(parent)
{
    QThread *thread = new QThread();
    client = new IpMsgFileClient(user);
    client->recvFileInfo.info.fileName = file->info.fileName;
    client->recvFileInfo.info.size = file->info.size;
    client->recvFileInfo.info.size = file->info.size;
    client->recvFileInfo.info.permission = file->info.permission;
    client->recvFileInfo.info.absoluteFilePath = file->info.absoluteFilePath;
    client->recvFileInfo.fileHost = file->fileHost;
    client->recvFileInfo.fileId = file->fileId;
    client->recvFileInfo.fileOut = file->fileOut;
    client->recvFileInfo.filePort = file->filePort;
    client->recvFileInfo.fileTranStatus = FILE_TRANS_STATUS_RUN;
    client->recvFileInfo.fileOffset = 0;
    client->moveToThread(thread);
    connect(thread,SIGNAL(started()),client,SLOT(ipMsgFileClientStart()));
    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
    connect(thread,SIGNAL(finished()),client,SLOT(deleteLater()));
    connect(thread,SIGNAL(finished()),this,SLOT(deleteLater()));
    thread->start();
}

