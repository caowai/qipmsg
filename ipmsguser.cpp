#include "ipmsguser.h"


IpMsgUser::IpMsgUser(QObject *parent) : QObject(parent)
{
    userVer.clear();
    userId.clear();
    userIcon.clear();
    userEmail.clear();
    userHostAddr.clear();
    userHostName.clear();
    userNickName.clear();
    userGroupName.clear();
    userPktSeq = 0;
    userCfmSeq = 0;
    chatForm = new FormChat();
}

IpMsgUser::~IpMsgUser()
{
    qDebug()<<"Do nothing for"<<this->userId;
}

void IpMsgUser::addSendFile(QString file)
{
#if 0
    IpMsgFileClient client;
    client.fileInfo.info = QFileInfo(file);
    client.fileInfo.fileId = g_send_file_id++;
    fileClients.append(&client);
    qDebug()<<"IpMsgUser"<<__FUNCTION__<<__LINE__<<"clients"<<fileClients.size();
#endif
}

void IpMsgUser::delSendFile(int index)
{
#if 0
    fileClients.takeAt(index);
    qDebug()<<"IpMsgUser"<<__FUNCTION__<<__LINE__<<"clients"<<fileClients.size();
#endif
}

void IpMsgUser::delAllSendFile()
{
#if 0
    fileClients.clear();
    qDebug()<<"IpMsgUser"<<__FUNCTION__<<__LINE__<<"clients"<<fileClients.size();
#endif
}


void IpMsgUser::dumpUserInfo()
{
    qDebug()<<"IpAddr:"<<userHostAddr.toString();
    qDebug()<<"Version:"<<userVer;
    qDebug()<<"UserId:"<<userId;
    qDebug()<<"NickName:"<<userNickName;
    qDebug()<<"Group:"<<userGroupName;
    qDebug()<<"Hostname:"<<userHostName;
    qDebug()<<"PacketSeq:"<<userPktSeq;
    return;

}

void IpMsgUser::appendChatHistory(QString data)
{
    chatHistory.append(data);
    return;
}

void IpMsgUser::updateChatFormHistory()
{
    if(chatForm != nullptr)
    {
        chatForm->updateChatHistory(chatHistory.toUtf8());
    }
}
