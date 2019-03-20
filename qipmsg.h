#ifndef QIPMSG_H
#define QIPMSG_H

#include <QMainWindow>
#include <QTextCodec>
#include <QUdpSocket>
#include <QWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QDateTime>
#include <QPushButton>
#include <QTranslator>
#include "ipmsgcommon.h"
#include "dialogsetting.h"
#include "ipmsgfileserver.h"
#include "ipmsgudpsession.h"
#include "ipmsgfilerecv.h"

namespace Ui {
class QIPMSG;
}

class QIPMSG : public QMainWindow
{
    Q_OBJECT

public:
    explicit QIPMSG(QWidget *parent = nullptr);
    ~QIPMSG();
    QTranslator *qIpMsgLang = nullptr;
    IpMsgUser mSelf;
    QString toUnicode(QByteArray data);


public slots:
   void qIpMsgUdpSessionHandle(QHostAddress src, QByteArray data);
   void qIpMsgFileServerHandle(quint32 ip, quint16 port, QByteArray data);
   void qIpMsgUserListUpdate();
   void userListRefreshTrigger();
   void qIpMsgConfigration();
   void startChat(QListWidgetItem *item);
   void qIpMsgChatSent(QString data,QHostAddress dest);
   void qIpMsgSettingsAccepted();
   void qIpMsgSettingsRejected();
   QByteArray fromUnicode(QString data);
   void qIpMsgStatus(QString data);

private slots:
   void qIpMsgLangtoEn();
   void qIpMsgLangtoZh();
   void qIpMsgFileTransFinished(quint32 fileId);
   void qIpMsgFileTransProgress(quint32 fileId,int progress);
   void qIpMsgFileTransError(quint32 fileId,int progress);
   void qIpMsgAcceptFile(fileEntryT *file);
   void qIpMsgRejectFile(fileEntryT *file);
   void qIpMsgRecvFileFinish(quint32 fileId);
   void qIpMsgRecvFileError(quint32 fileId);

signals:
   void userListUpdate();
   void changeLang(int lang);


protected:
    void timerEvent(QTimerEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    Ui::QIPMSG *ui;
    QList<IpMsgUser*> mUsers;
    DialogSetting *mSetPanel = nullptr;
    IpMsgUdpSession session;
    IpMsgFileServer *fileServer;
    QString mCodec="GBK";
    unsigned int mStartIp=0;
    unsigned int mEndIp=0;
};

#endif // QIPMSG_H
