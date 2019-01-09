#ifndef IPMSGUSER_H
#define IPMSGUSER_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QHostAddress>
#include <QHostInfo>
#include "formchat.h"
#include "ipmsgcommon.h"


class IpMsgUser : public QObject
{
    Q_OBJECT
public:
    explicit IpMsgUser(QObject *parent = nullptr);
    ~IpMsgUser();
    void dumpUserInfo();
    void appendChatHistory(QString data);
    void updateChatFormHistory();
    QList <fileEntryT *> fileQueue;
    int fileSendFlag;
    QByteArray userVer;
    QByteArray userId;
    QByteArray userNickName;
    QByteArray userGroupName;
    QByteArray userHostName;
    QByteArray userEmail;
    QByteArray userIcon;
    QHostAddress userHostAddr;
    uint32_t userPktSeq;
    uint32_t userCfmSeq;
    FormChat *chatForm = nullptr;
    QString chatHistory;
    QByteArray chatBuffer;
    int offlineTimer = 0;
    int blinkTimer = 0;
    int blinkFlag = 0;

signals:
    void sendFileUpdate();

private slots:
    void addSendFile(QString file);
    void delSendFile(int index);
    void delAllSendFile();

};

#endif // IPMSGUSER_H
