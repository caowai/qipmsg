#ifndef FORMCHAT_H
#define FORMCHAT_H

#include <QWidget>
#include <QHostAddress>
#include <QTextEdit>
#include <QMenu>
#include <QListWidgetItem>
#include "ipmsgcommon.h"

namespace Ui {
class FormChat;
}

class FormChat : public QWidget
{
    Q_OBJECT

public:
    explicit FormChat(QWidget *parent = nullptr);
    ~FormChat();
    QTextEdit *getChatHistory();
    QTextEdit *getChat();
    void updateChatHistory(QString data);
    void setUserName(QString value);
    void setHost(QString value);
    void setGroupName(QString value);
    void setClient(QString value);
    QString FileSizeConvert(qint64 size);
    QList <fileEntryT *>fileList;
    void updateFileProgress(quint32 fileId,int progress);
    void updateFileError(quint32 fileId,int progress);
    void delFixedShareFile(int index);
    void addRemoteShareFile(fileEntryT *newfile);
    void delFixedRemoteShareFile(int index);
    QWidget *main;
protected:
    bool event(QEvent *event);
signals:
    void sent(QString data,QHostAddress dest);
    void addSendFile(QString file);
    void delSendFile(int index);
    void delAllSendFile();
    void acceptFile(fileEntryT *file);
    void rejectFile(fileEntryT *file);
    void cancelFile(fileEntryT *file);
    void recvfinish(quint32 fileId);
    void recverror(quint32 fileId);
private:
    Ui::FormChat *ui;
    QString mUser;
    QString mHost;
    QString mGroup;
    QString mClient;
    QMenu *mSendFileCmdMenu;
    QMenu *mRecvFileCmdMenu;
    QObject *mChatUser;
private slots:
    void chatSend();
    void addShareFile();
    void delShareFile();
    void delAllShareFile();
    void acceptShareFile();
    void rejectShareFile();
    void acceptAllShareFile();
    void rejectAllShareFile();
    void on_tableWidgetSendFileList_customContextMenuRequested(const QPoint &pos);
    void on_tableWidgetRecvFileList_customContextMenuRequested(const QPoint &pos);
    void fileRecvFinished(quint32 fileId);
    void fileRecvProgress(quint32 fileId,int progress);
    void fileRecvError(quint32 fileId,int progress);

};

#endif // FORMCHAT_H
