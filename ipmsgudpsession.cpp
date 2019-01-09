#include "ipmsgudpsession.h"
#include "ipmsguser.h"
#include <unistd.h>
#include <QTextCodec>
#include <QTimerEvent>
#include <QNetworkInterface>
#include <QDateTime>

quint32 g_pkg_seq = 0;
IpMsgUdpSession::IpMsgUdpSession(QObject *parent) : QObject(parent)
{
       if(false == ipMsgSock.bind(QHostAddress::AnyIPv4, IPMSG_PORT, QUdpSocket::ShareAddress))
       {
           mUdpBind = false;
       }
       else {
           mUdpBind = true;
       }
       connect(&ipMsgSock, SIGNAL(readyRead()),this, SLOT(ipMsgUdpDataHandle()));
}


bool IpMsgUdpSession::IpMsgUdpSessionValid()
{
    return ipMsgSock.isValid();
}

void IpMsgUdpSession::ipMsgPktSend(QHostAddress address, uint16_t port,QByteArray data)
{
    //qDebug()<<"OUT:"<<address.toString()<<"Data:"<<data;
    ipMsgSock.writeDatagram(data,address,port);
}

void IpMsgUdpSession::ipMsgUserExit(IpMsgUser *user)
{
    QHostAddress dest;
    QList<QNetworkInterface> interfaceList;
    QList<QNetworkAddressEntry> entryList;
    QByteArray command;
    command.append(user->userVer+":");
    command.append(QString::number(g_pkg_seq++)+":");
    //command.append(QString::number(time(nullptr),10)+":");
    command.append(user->userId+":");
    command.append(user->userHostName+":");
    command.append(QString::number(IPMSG_BR_EXIT|IPMSG_ABSENCEOPT,10)+":");
    command.append((char)0);
    if(user->userGroupName.length()>0)
    {
        command.append(user->userGroupName);
    }
    command.append((char)0);

    interfaceList = QNetworkInterface::allInterfaces();

    for(int j = 0; j< interfaceList.size();j++)
    {
        entryList = interfaceList.at(j).addressEntries();
        for (int i = 0; i < entryList.size(); ++i)
        {
            if (entryList.at(i).ip().toIPv4Address())
            {
                //qDebug()<<"Interface:"<<interfaceList.at(j).name()<<"IPAdress:"<<entryList.at(i).ip().toString()<<"Broadcast:"<<entryList.at(i).broadcast().toString();
                if(entryList.at(i).ip() != QHostAddress::LocalHost)
                {
                    dest.setAddress(entryList.at(i).broadcast().toIPv4Address());
                    ipMsgPktSend(dest,IPMSG_PORT,command);
                    usleep(10000);
                }
            }
         }
    }
}


void IpMsgUdpSession::ipMsgUserRefresh(IpMsgUser *user,unsigned int startip, unsigned int endip)
{
    QHostAddress dest;
    QHostAddress mask;
    QList<QNetworkInterface> interfaceList;
    QList<QNetworkAddressEntry> entryList;
    QByteArray command;
    if(ipMsgHostDetectTimer !=0)
    {
        killTimer(ipMsgHostDetectTimer);
        ipMsgHostDetectTimer  = 0;
    }
    mUser = user;
    ipMsgHostEnd = endip;
    ipMsgHostStart = startip;
    ipMsgHostCurrent = startip;
    command.append(mUser->userVer+":");
    command.append(QString::number(g_pkg_seq++)+":");
    //command.append(QString::number(time(nullptr),10)+":");
    command.append(mUser->userId+":");
    command.append(mUser->userHostName+":");
    command.append(QString::number(IPMSG_BR_ENTRY|IPMSG_ABSENCEOPT,10)+":");
    command.append(mUser->userNickName);
    command.append((char)0);
    command.append(mUser->userGroupName);
    command.append((char)0);

    if(startip!=endip)
    {
        ipMsgHostDetectTimer=startTimer(10);
        return;
    }
    else if(startip!=0)
    {
        dest.setAddress(startip);
        ipMsgPktSend(dest,IPMSG_PORT,command);
        return;
    }
    interfaceList = QNetworkInterface::allInterfaces();

    for(int j = 0; j< interfaceList.size();j++)
    {
        entryList = interfaceList.at(j).addressEntries();
        for (int i = 0; i < entryList.size(); ++i)
        {
            if (entryList.at(i).ip().toIPv4Address())
            {
                //qDebug()<<"Interface:"<<interfaceList.at(j).name()<<"IPAdress:"<<entryList.at(i).ip().toString()<<"Broadcast:"<<entryList.at(i).broadcast().toString();
                if(entryList.at(i).ip() != QHostAddress::LocalHost)
                {
                    dest.setAddress(entryList.at(i).broadcast().toIPv4Address());
                    ipMsgPktSend(dest,IPMSG_PORT,command);
                    usleep(10000);
                }
            }
         }
    }
    return;
}
void IpMsgUdpSession::timerEvent(QTimerEvent *event)
{
    QByteArray command;
    QHostAddress dest;

    if(event->timerId() == ipMsgHostDetectTimer
            && mUser != nullptr
            && ipMsgHostCurrent!=0
            && ipMsgHostCurrent <= ipMsgHostEnd)
    {
        command.append(mUser->userVer+":");
        command.append(QString::number(g_pkg_seq++)+":");
        //command.append(QString::number(time(nullptr),10)+":");
        command.append(mUser->userId+":");
        command.append(mUser->userHostName+":");
        command.append(QString::number(IPMSG_BR_ENTRY|IPMSG_ABSENCEOPT,10)+":");
        command.append(mUser->userNickName);
        command.append((char)0);
        command.append(mUser->userGroupName);
        command.append((char)0);
        dest.setAddress(ipMsgHostCurrent);
        ipMsgPktSend(dest,IPMSG_PORT,command);
        ipMsgHostCurrent++;
        if(ipMsgHostCurrent > ipMsgHostEnd)
        {
            ipMsgHostCurrent = 0;
            killTimer(ipMsgHostDetectTimer);
            ipMsgHostDetectTimer = 0;
        }
    }

}

void IpMsgUdpSession::ipMsgRespOK(IpMsgUser *user, IpMsgUser *dest)
{
    QByteArray command;
    {

        command.append(user->userVer+":");
        command.append(QString::number(g_pkg_seq++)+":");
        //command.append(QString::number(time(nullptr),10)+":");
        command.append(user->userId+":");
        command.append(user->userHostName+":");
        command.append(QString::number(IPMSG_RECVMSG|IPMSG_ABSENCEOPT,10)+":");
        command.append(QString::number(user->userPktSeq,10));
        command.append((char)0);
        command.append((char)0);
        ipMsgPktSend(dest->userHostAddr,IPMSG_PORT,command);
    }
}



void IpMsgUdpSession::ipMsgSendData(IpMsgUser *user, QHostAddress dest,QByteArray data)
{
    QByteArray command;
    {

        command.append(user->userVer+":");
        command.append(QString::number(user->userPktSeq,10)+":");
        command.append(user->userId+":");
        command.append(user->userHostName+":");
        command.append(QString::number(IPMSG_SENDMSG|IPMSG_ABSENCEOPT,10)+":");
        command.append(data);
        command.append((char)0);
        command.append((char)0);
        ipMsgPktSend(dest,IPMSG_PORT,command);
    }
}


void IpMsgUdpSession::ipMsgSendFiles(IpMsgUser *user,QHostAddress dest, QList <fileEntryT *>files)
{
     QByteArray command;
     command.clear();
     command.append(user->userVer+":");
     command.append(QString::number(user->userPktSeq,10)+":");
     command.append(user->userId+":");
     command.append(user->userHostName+":");
     command.append(QString::number(IPMSG_SENDMSG|IPMSG_ABSENCEOPT|IPMSG_FILEATTACHOPT,10)+":");
     command.append((char)0);
     for(int i =0;i<files.count();)
     {
         command.append(QString::number(files.at(i)->fileId,10)+":");
         command.append(files.at(i)->info.fileName+":");
         command.append("0"+QString::number(files.at(i)->info.size,16)+":");
         command.append(QString::number(files.at(i)->info.metadataChangeTime,16)+":");
         command.append("1:");

         i++;
         command.append('\a');

         if(i == files.count())
         {
            command.append((char)0);
         }
     }
     ipMsgPktSend(dest,IPMSG_PORT,command);
}
void IpMsgUdpSession::ipMsgRespBr(IpMsgUser*user, IpMsgUser *dest)
{
    QByteArray command;
    command.clear();
    {
        command.append(user->userVer+":");
        command.append(QString::number(g_pkg_seq++)+":");
        //command.append(QString::number(time(nullptr),10)+":");
        command.append(user->userId+":");
        command.append(user->userHostName+":");
        command.append(QString::number(IPMSG_ANSENTRY,10)+":");
        command.append(user->userNickName);
        command.append((char)0);
        command.append(user->userGroupName);
        command.append((char)0);
        ipMsgPktSend(dest->userHostAddr,IPMSG_PORT,command);
    }
}

void IpMsgUdpSession::ipMsgUdpDataHandle()
{
    QByteArray data;
    QHostAddress srcAddress;
    quint16 srcPort;
    data.resize(ipMsgSock.pendingDatagramSize());
    ipMsgSock.readDatagram(data.data(),data.size(),&srcAddress,&srcPort);
   // qDebug()<<"Received from"<<srcAddress.toString()<<":"<<QString::number(srcPort,10)<<data;

    emit ipMsgUdpSessionDataReady(srcAddress,data);
    return;
}
