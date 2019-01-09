#include "qipmsg.h"
#include "ui_qipmsg.h"
#include <unistd.h>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QTranslator>
#include <QThread>

QIPMSG::QIPMSG(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QIPMSG)
{
    qIpMsgLang = new QTranslator();
    qIpMsgLang->load(":/translations/translations_zh.qm");
    QApplication::instance()->installTranslator(qIpMsgLang);

    mSelf.userId=fromUnicode(QString(tr(IPMSG_DEFAULT_USERNAME)));
    mSelf.userVer=QString(IPMSG_VERSION).toUtf8();
    mSelf.userPktSeq = 0;
    mSelf.userHostName = QHostInfo::localHostName().toUtf8();
    mSelf.userNickName = fromUnicode(QString(tr(IPMSG_DEFAULT_NICKNAME)));
    mSelf.userGroupName = fromUnicode(QString(tr(IPMSG_DEFAULT_GROUPNAME)));

    mStartIp = 0;
    mEndIp = 0;

    ui->setupUi(this);

    fileServer = new IpMsgFileServer();

    qIpMsgStatus("");

    ui->menuUser->setTitle(tr("User"));
    ui->actionRefresh->setText(tr("Refresh"));
   // ui->actionSearch->setText(tr("Search"));
    ui->actionExit->setText(tr("Exit"));

    ui->menuSettings->setTitle(tr("Setting"));
    ui->actionConfigration->setText(tr("Configration"));

    //ui->menuAbout->setTitle(tr("About"));
   // ui->actionVersion->setText(tr("Version"));
    //ui->actionEmail->setText(tr("Email"));



    connect(ui->actionRefresh,SIGNAL(triggered()),this,SLOT(userListRefreshTrigger()));
    connect(&session,SIGNAL(ipMsgUdpSessionDataReady(QHostAddress, QByteArray)),this,SLOT(qIpMsgUdpSessionHandle(QHostAddress,QByteArray)));
    connect(fileServer,SIGNAL(ipMsgFileServerDataReady(quint32,quint16,QByteArray)),this,SLOT(qIpMsgFileServerHandle(quint32,quint16,QByteArray)));
    connect(this,SIGNAL(userListUpdate()),this,SLOT(qIpMsgUserListUpdate()));
    connect(ui->listWidgetUserList,SIGNAL(itemDoubleClicked(QListWidgetItem *)),this,SLOT(startChat(QListWidgetItem *)));
    connect(ui->actionConfigration,SIGNAL(triggered()),this,SLOT(qIpMsgConfigration()));
    connect(ui->actionEnglish,SIGNAL(triggered()),this,SLOT(qIpMsgLangtoEn()));
    connect(ui->actionChinese,SIGNAL(triggered()),this,SLOT(qIpMsgLangtoZh()));

    if(true == session.IpMsgUdpSessionValid())
    {
        session.ipMsgUserRefresh(&mSelf,mStartIp,mEndIp);
    }
    else
    {
        close();
    }

}


QIPMSG::~QIPMSG()
{
    qDebug()<<"QIPMSG will close";
    delete ui;
}

void QIPMSG::qIpMsgLangtoEn()
{
   if(qIpMsgLang != nullptr)
   {
       //Switch language to English.

       QApplication::instance()->removeTranslator(qIpMsgLang);
       ui->retranslateUi(this);
       //Notes: Please remember, do not delete qIpMsgLang every time.
       //delete(qIpMsgLang);
       //qIpMsgLang = nullptr;
       qIpMsgStatus(tr("Found")+" "+QString::number(mUsers.count(),10)+" "+tr("users"));
   }
}


void QIPMSG::qIpMsgLangtoZh()
{
    //Switch language to chinese.
    //Please remember, do not delete qIpMsgLang every time.
    if(qIpMsgLang == nullptr)
    {
        qIpMsgLang = new QTranslator();
    }
     qIpMsgLang->load(":/translations/translations_zh.qm");
     QApplication::instance()->installTranslator(qIpMsgLang);
     ui->retranslateUi(this);
     qIpMsgStatus(tr("Found")+" "+QString::number(mUsers.count(),10)+" "+tr("users"));
}
QString QIPMSG::toUnicode(QByteArray data)
{
    return QTextCodec::codecForName(mCodec.toStdString().c_str())->toUnicode(data);
}

QByteArray QIPMSG::fromUnicode(QString data)
{
    return QTextCodec::codecForName(mCodec.toStdString().c_str())->fromUnicode(data);
}

void QIPMSG::closeEvent(QCloseEvent *event)
{
    qDebug()<<"Close all chat windows";
    for(int i=0;i<mUsers.count();i++)
    {
        if(mUsers.at(i)->chatForm != nullptr)
        {
            mUsers.at(i)->chatForm->close();
            //delete(mUsers.at(i)->chatForm);
        }
    }
    if(true == session.IpMsgUdpSessionValid())
    {
        session.ipMsgUserExit(&mSelf);
    }
}

void QIPMSG::qIpMsgConfigration()
{
    if(mSetPanel == nullptr)
    {
        mSetPanel = new DialogSetting();
    }
    mSetPanel->setWindowTitle(tr("Configration"));
    mSetPanel->setUserName(toUnicode(mSelf.userId).toUtf8());
    mSetPanel->setUserNickName(toUnicode(mSelf.userNickName).toUtf8());
    mSetPanel->setGroupName(toUnicode(mSelf.userGroupName).toUtf8());
    mSetPanel->setStartIp(mStartIp);
    mSetPanel->setEndIp(mEndIp);
    connect(mSetPanel,SIGNAL(setDialogClose()),this,SLOT(qIpMsgSettingsRejected()));
    connect(mSetPanel,SIGNAL(setDialogAccepted()),this,SLOT(qIpMsgSettingsAccepted()));
    connect(mSetPanel,SIGNAL(setDialogRejected()),this,SLOT(qIpMsgSettingsRejected()));
    mSetPanel->show();
}

void QIPMSG::qIpMsgSettingsAccepted()
{
    mSelf.userId = fromUnicode(mSetPanel->getUserName());
    mSelf.userNickName = fromUnicode(mSetPanel->getUserNickName());
    mSelf.userGroupName = fromUnicode(mSetPanel->getGroupName());
    mStartIp = QHostAddress(mSetPanel->getStartIp()).toIPv4Address();
    mEndIp = QHostAddress(mSetPanel->getEndIp()).toIPv4Address();
    qDebug()<<"New configratuion:";
    qDebug()<<"Username:"<<mSelf.userId<<"NickName:"<<mSelf.userNickName<<"GroupName:"<<mSelf.userGroupName;
    qDebug()<<"StartIP:"<<QHostAddress(mStartIp).toString()<<"EndIP:"<<QHostAddress(mEndIp).toString();
    session.ipMsgUserRefresh(&mSelf,mStartIp,mEndIp);
    mSetPanel->close();
    delete mSetPanel;
    mSetPanel = nullptr;
}
void QIPMSG::qIpMsgSettingsRejected()
{
    if(nullptr != mSetPanel)
    {
        delete mSetPanel;
        mSetPanel = nullptr;
    }
}
void QIPMSG::userListRefreshTrigger()
{
    //Udp session is valid
    if(true == session.IpMsgUdpSessionValid())
    {
        for(int i=0;i<mUsers.count();i++)
        {
            //Chat form is null or hidden.
            //Need delete the chat form and delete user if no message unread.
            if((mUsers.at(i)->chatForm == nullptr
                    || mUsers.at(i)->chatForm->isHidden()))
             {
                 if(mUsers.at(i)->chatForm != nullptr)
                 {
                    //Close the chat form window
                    mUsers.at(i)->chatForm->close();
                    //Delete the window
                    delete(mUsers.at(i)->chatForm);
                    qDebug()<<__FUNCTION__<<__LINE__;
                    mUsers.at(i)->chatForm = nullptr;
                 }
                 //No unread message for this user, delete user
                 if(mUsers.at(i)->blinkTimer==0)
                 {
                    delete(ui->listWidgetUserList->takeItem(i));
                    qDebug()<<__FUNCTION__<<__LINE__;
                    delete(mUsers.takeAt(i));
                    qDebug()<<__FUNCTION__<<__LINE__;
                 }
            }
            else
            {
                //No new message but chat form is actived, then just hidden user from list
                //but not delete it.
                if(mUsers.at(i)->blinkTimer==0)
                {
                    ui->listWidgetUserList->item(i)->setHidden(true);
                }
            }
        }
        session.ipMsgUserRefresh(&mSelf,mStartIp,mEndIp);
    }
}

void QIPMSG::qIpMsgUserListUpdate()
{
    int count = 0;
    ui->listWidgetUserList->clear();

    //IpMsgUserSort(mUsers);
    count = mUsers.count();
    qIpMsgStatus(tr("Found")+" "+QString::number(mUsers.count(),10)+" "+tr("users"));
    //qIpMsgStatus(toUnicode((tr("Found").toUtf8()+" "+QString::number(mUsers.count(),10).toUtf8()+" "+tr("users").toUtf8())));
    //qIpMsgStatus(fromUnicode(QString(tr("Found")+" "+QString::number(mUsers.count(),10)+" "+tr("users"))));

    for(int i=0;i<count;i++)
    {
        ui->listWidgetUserList->addItem(new QListWidgetItem(QIcon(":/userIcon/chat.png"),toUnicode(mUsers.at(i)->userNickName)
                                                          +"("
                                                          +toUnicode(mUsers.at(i)->userHostName)
                                                          +")"
                                                          +"\n"
                                                          +mUsers.at(i)->userHostAddr.toString()
                                                          ));
    }

}

void QIPMSG::qIpMsgStatus(QString data)
{
    ui->labelStatus->setText(data);
}
void QIPMSG::startChat(QListWidgetItem *item)
{
    int currentRow=ui->listWidgetUserList->currentIndex().row();

    item->setSelected(false);

    //Stop new message blink first for this chat form.
    if(mUsers.at(currentRow)->blinkTimer != 0)
    {
        killTimer(mUsers.at(currentRow)->blinkTimer);
        mUsers.at(currentRow)->blinkTimer = 0;
        mUsers.at(currentRow)->blinkFlag = 0;
    }
    //Update widget information
    ui->listWidgetUserList->item(currentRow)->setIcon(QIcon(":/userIcon/chat.png"));
    ui->listWidgetUserList->item(currentRow)->setTextColor(QColor("black"));

    //Create a new chat form window
    if(nullptr == mUsers.at(currentRow)->chatForm)
    {
        mUsers.at(currentRow)->chatForm = new FormChat();
        connect(mUsers.at(currentRow)->chatForm,SIGNAL(sent(QString,QHostAddress)),this,SLOT(qIpMsgChatSent(QString,QHostAddress)));
        //connect(mUsers.at(currentRow)->chatForm,SIGNAL(addSendFile(QString)),mUsers.at(currentRow),SLOT(addSendFile(QString)));
        //connect(mUsers.at(currentRow)->chatForm,SIGNAL(delSendFile(int)),mUsers.at(currentRow),SLOT(delSendFile(int)));
        //connect(mUsers.at(currentRow)->chatForm,SIGNAL(delAllSendFile()),mUsers.at(currentRow),SLOT(delAllSendFile()));
       // connect(mUsers.at(currentRow)->chatForm,SIGNAL(close()),this,SLOT(chatClean()));
    }

    //Update chat form information
    mUsers.at(currentRow)->chatForm->setWindowTitle(toUnicode(mUsers.at(currentRow)->userNickName));

    mUsers.at(currentRow)->updateChatFormHistory();
    mUsers.at(currentRow)->chatForm->setClient(mUsers.at(currentRow)->userVer);
    mUsers.at(currentRow)->chatForm->setUserName(toUnicode(mUsers.at(currentRow)->userId));
    mUsers.at(currentRow)->chatForm->setHost(mUsers.at(currentRow)->userHostAddr.toString());
    mUsers.at(currentRow)->chatForm->setGroupName(toUnicode(mUsers.at(currentRow)->userGroupName));
    //Shwo chat form.
    mUsers.at(currentRow)->chatForm->show();
}

void QIPMSG::qIpMsgChatSent(QString data,QHostAddress dest)
{
    QList <fileEntryT *>files;
    for(int i = 0; i<mUsers.count();i++)
    {
        if(dest == mUsers.at(i)->userHostAddr)
        {
            if(data.length() > 0)
            {
                mUsers.at(i)->userPktSeq =(uint32_t)(time(nullptr));
                //Send chat content out
                session.ipMsgSendData(&mSelf,dest,fromUnicode(data));
                //Update Chat history in Chat form.
                mUsers.at(i)->appendChatHistory(toUnicode(mSelf.userNickName+" "+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toUtf8()+"\n"));
                mUsers.at(i)->appendChatHistory(data);
                mUsers.at(i)->appendChatHistory("\n");
                mUsers.at(i)->updateChatFormHistory();
                mUsers.at(i)->chatForm->getChat()->clear();
                mUsers.at(i)->offlineTimer=startTimer(200);
            }
            if(mUsers.at(i)->chatForm->fileList.size()>0)
            {
                for(int j=0;j<mUsers.at(i)->chatForm->fileList.size();j++)
                {
                    if(mUsers.at(i)->chatForm->fileList.at(j)->fileTranStatus == FILE_TRANS_STATUS_IDLE)
                    {
                        files.append(mUsers.at(i)->chatForm->fileList.at(j));
                        mUsers.at(i)->chatForm->fileList.at(j)->fileTranStatus = FILE_TRANS_STATUS_QUEUE;
                        mSelf.fileQueue.append(mUsers.at(i)->chatForm->fileList.at(j));
                        qDebug()<<"add new file to queue:"<<mUsers.at(i)->chatForm->fileList.at(j)->info.fileName;
                    }
                }
                if(files.count()>0)
                    session.ipMsgSendFiles(&mSelf,mUsers.at(i)->userHostAddr,files);
            }
            break;
        }
    }
}

void QIPMSG::timerEvent(QTimerEvent *event)
{
    int current_timer = event->timerId();
    for(int i =0; i<mUsers.count();i++)
    {
        //No data response, indicate user offline
        if(mUsers.at(i)->offlineTimer !=0
                && mUsers.at(i)->offlineTimer == current_timer)
        {
            killTimer(mUsers.at(i)->offlineTimer);
            mUsers.at(i)->offlineTimer = 0;
            if(mUsers.at(i)->userCfmSeq != mUsers.at(i)->userPktSeq)
            {
                mUsers.at(i)->appendChatHistory(toUnicode(mUsers.at(i)->userNickName+tr(" May be offline").toUtf8()+"\n"));
                mUsers.at(i)->updateChatFormHistory();
            }
            break;
        }
        //New message arrived, blink user icon
        else if(mUsers.at(i)->blinkTimer !=0
                && mUsers.at(i)->blinkTimer == current_timer)
        {
            if(mUsers.at(i)->blinkFlag%2)
            {
                ui->listWidgetUserList->item(i)->setIcon(QIcon(":/userIcon/ipmsg.png"));
                ui->listWidgetUserList->item(i)->setTextColor(QColor("red"));
            }
            else {
                ui->listWidgetUserList->item(i)->setIcon(QIcon(":/userIcon/chat.png"));
                ui->listWidgetUserList->item(i)->setTextColor(QColor("black"));
            }
            mUsers.at(i)->blinkFlag++;
            break;
        }
    }
}
 void QIPMSG::qIpMsgUdpSessionHandle(QHostAddress src, QByteArray data)
 {
     IpMsgUser *user = nullptr;
     uint32_t pktCommand;
     uint8_t command;
     uint32_t commandOpt;
     QByteArrayList values;
     QByteArrayList others;
     QByteArray chatData;
     QByteArrayList files;

     int i =0;

     values = data.split(':');

     if(values.count() < 5)
     {
          qDebug()<<"Invalid pkt, ignore it";
          return;
     }

     for(i=5;i< values.count();i++)
     {
         chatData.append(values[i]);
         chatData.append(':');
     }


     pktCommand = values.at(4).toUInt();

     if(QString::number(pktCommand,10) != values.at(4))
     {
          qDebug()<<"Invalid command, ignore it";
          return;
     }

     command = pktCommand&0x000000FF;
     commandOpt = pktCommand&0xffffff00;
     //qDebug()<<"Command:"<<"0x"+QString::number(command,16)<<"Opt:"<<"0x"+QString::number(commandOpt,16);

     if(IPMSG_BR_ENTRY == command
             || IPMSG_ANSENTRY == command)
     {
         for(i=0;i<mUsers.count();i++)
         {
             if(mUsers.at(i)->userHostAddr == src)
             {
                 break;
             }
         }

         if(i!=mUsers.count())
         {
             user= mUsers.at(i);
         }
         else {
             user = new IpMsgUser();
         }//User information update

         user->userVer = values.at(0);
         mSelf.userPktSeq = values.at(1).toUInt();

         user->userId = values.at(2);

         user->userHostName = values.at(3);

         user->userHostAddr = src;


         if(chatData.length()>0)
         {
             others = chatData.split(0);
             if(others.count()>=1)
             {
                 for(int i = 0;i< others.count();i++)
                 {
                     if(i == 0)
                     {
                         user->userNickName = others.at(0);
                     }
                     else if(i == 1)
                     {
                         user->userGroupName = others.at(1);
                     }
                 }
             }
         }
         if(i==mUsers.count())
         {
             mUsers.append(user);
             ui->listWidgetUserList->addItem(new QListWidgetItem(QIcon(":/userIcon/chat.png"), toUnicode(mUsers.at(i)->userNickName)+"("+toUnicode(mUsers.at(i)->userHostName)+")"+"\n"+mUsers.at(i)->userHostAddr.toString()));
             qIpMsgStatus(tr("Found")+" "+QString::number(mUsers.count(),10)+" "+tr("users"));
             //mUsers.at(i)->dumpUserInfo();
             //qDebug()<<"Pkt Seq:"<<mSelf.userPktSeq;
         }
         else
         {
             ui->listWidgetUserList->item(i)->setText(toUnicode(mUsers.at(i)->userNickName)+"("+toUnicode(mUsers.at(i)->userHostName)+")"+"\n"+mUsers.at(i)->userHostAddr.toString());
         }
         ui->listWidgetUserList->item(i)->setTextColor(QColor("black"));
         ui->listWidgetUserList->item(i)->setHidden(false);

         if(IPMSG_BR_ENTRY == command)
         {
             session.ipMsgRespBr(&mSelf,mUsers.at(i));
         }
     }
     else if(IPMSG_SENDMSG == command)
     {
         for(i=0;i<mUsers.count();i++)
         {
             if(mUsers.at(i)->userHostAddr == src)
             {
                 qDebug()<<"Found"<<mUsers.at(i)->userHostAddr.toString();
                 mSelf.userPktSeq = values.at(1).toUInt();
                 if(mUsers.at(i)->chatForm == nullptr
                         || mUsers.at(i)->chatForm->isHidden())
                 {
                     mUsers.at(i)->blinkTimer=startTimer(500);
                 }
                 break;
             }
         }

         if(IPMSG_FILEATTACHOPT & commandOpt)
         {
             files = chatData.split(0);
             qDebug()<<"File Msg from"<<"User:"<<QString::number(i,10)<<values.at(3)<<files;
             //Handle File attched requirment.
            if(i != mUsers.count())
            {
             for(int k =0; k<files.count();k++)
             {
                 QByteArrayList fileList = files.at(k).split('\a');
                 for(int b= 0; b<fileList.count();b++)
                 {
                     QByteArrayList tmp = fileList.at(b).split(':');
                     if(tmp.count()>5)
                     {
                        fileEntryT *newFile = new fileEntryT();
                        newFile->fileId = tmp.at(0).toUInt();
                        newFile->info.fileName = tmp.at(1);
                        newFile->info.size = tmp.at(2).toUInt(nullptr,16);
                        newFile->info.permission = tmp.at(3).toUInt(nullptr,16);
                        mUsers.at(i)->chatForm->addRemoteShareFile(newFile);
                        qDebug()<<"Add"<<tmp.at(1);
                     }
                 }


             }
            }
         }
         else
         {
             qDebug()<<"Receive Msg from"<<"User:"<<QString::number(i,10)<<values.at(3)<<":"+chatData.split(0).at(0);
         }

         if(commandOpt&IPMSG_SENDCHECKOPT)
         {
                if(i != mUsers.count())
                {
                    //qDebug("Need resp");
                    session.ipMsgRespOK(&mSelf,mUsers.at(i));
                    mUsers.at(i)->appendChatHistory(toUnicode(mUsers.at(i)->userNickName+" "+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toUtf8()+"\n"));
                    mUsers.at(i)->appendChatHistory(toUnicode(chatData.split(0).at(0)+"\n"));
                    if(nullptr != mUsers.at(i)->chatForm
                            && mUsers.at(i)->chatForm->isEnabled())
                    {
                        mUsers.at(i)->chatForm->setWindowTitle(toUnicode(mUsers.at(i)->userNickName));
                        mUsers.at(i)->updateChatFormHistory();
                        mUsers.at(i)->chatForm->setClient(mUsers.at(i)->userVer);
                        mUsers.at(i)->chatForm->setUserName(toUnicode(mUsers.at(i)->userId));
                        mUsers.at(i)->chatForm->setHost(mUsers.at(i)->userHostAddr.toString());
                        mUsers.at(i)->chatForm->setGroupName(toUnicode(mUsers.at(i)->userGroupName));
                    }
                }
         }
     }
     else if(IPMSG_RECVMSG == command)
     {
         mSelf.userPktSeq = values.at(1).toUInt();
         for(i=0;i<mUsers.count();i++)
         {
             if(mUsers.at(i)->userHostAddr == src)
             {
                 if(mUsers.at(i)->offlineTimer !=0)
                 {
                     killTimer(mUsers.at(i)->offlineTimer);
                     mUsers.at(i)->offlineTimer = 0;
                 }
                 mUsers.at(i)->userCfmSeq = chatData.split(0).at(0).toUInt();
                 //qDebug()<<"Got cfm "<<mUsers.at(i)->userCfmSeq<<"from"<<src.toString();
                 break;
             }
         }
     }
     else if(IPMSG_BR_EXIT == command)
     {
           mSelf.userPktSeq = values.at(1).toUInt();
           for(i=0;i<mUsers.count();i++)
           {
               if(mUsers.at(i)->userHostAddr == src)
               {
                   if(mUsers.at(i)->blinkTimer==0)
                   {
                        qDebug()<<"will hidden"<<src.toString();
                        ui->listWidgetUserList->item(i)->setHidden(true);
                   }
                   else
                   {
                        qDebug()<<"Now message did not display, can not hidden it";
                   }
                   break;
               }
           }
           qIpMsgStatus(tr("Found")+" "+QString::number(mUsers.count(),10)+" "+tr("users"));
     }
 }


void QIPMSG::qIpMsgFileServerHandle(quint32 ip, quint16 port, QByteArray data)
{
    uint32_t reqFileId = 0;
    uint32_t command;

    QHostAddress addr;

    QByteArray dataSample= "1_lbt80_0#160#FC4DD4D98024#0#0#0#4000#9:1546091332:caoweigang:CAOWEIGANG:96:5:2710:0:";
    QByteArrayList value = data.split(':');

    addr.setAddress(ip);
    qDebug()<<"Handle data"<<data<<"From"<<addr.toString()<<"Port:"<<port;
    if(value.count() > 8)
    {
        command = value.at(4).toUInt(nullptr,10) & 0xff;
        reqFileId = value.at(6).toUInt(nullptr,16);

        qDebug()<<"TCP Command:"<<command<<"File Id:"<<reqFileId;


        if(command != IPMSG_GETFILEDATA)
        {
            return;
        }
    }
    else
    {
        qDebug()<<"Error"<<__FUNCTION__<<__LINE__;
        return;
    }


    for(int i=0;i<mSelf.fileQueue.size();i++)
    {
        if(reqFileId == mSelf.fileQueue.at(i)->fileId
                && ip == mSelf.fileQueue.at(i)->fileHost
                && mSelf.fileQueue.at(i)->fileTranStatus == FILE_TRANS_STATUS_QUEUE)
        {
            mSelf.fileQueue.at(i)->filePort = port;
            mSelf.fileQueue.at(i)->fileTranStatus = FILE_TRANS_STATUS_RUN;
            {
                FileTransWorker *trans = new FileTransWorker();
                trans->mFilename = mSelf.fileQueue.at(i)->info.absoluteFilePath;
                trans->mFileId = mSelf.fileQueue.at(i)->fileId;
                trans->mHandle = (fileServer->fileServerSockMatch(mSelf.fileQueue.at(i)->fileHost,mSelf.fileQueue.at(i)->filePort))->socketDescriptor();

               // qDebug()<<"Got File"<<trans->mFilename<<"Sock"<<trans->mSock->peerAddress().toString()<<trans->mSock->peerPort();

                connect(trans,SIGNAL(fileTransProgress(quint32,int)),this,SLOT(qIpMsgFileTransProgress(quint32,int)));
                connect(trans,SIGNAL(fileTransFinished(quint32)),this,SLOT(qIpMsgFileTransFinished(quint32)));
                connect(trans,SIGNAL(fileTransError(quint32,int)),this,SLOT(qIpMsgFileTransError(quint32,int)));
                QThread *thread = new QThread();
                trans->moveToThread(thread);
                connect(thread,SIGNAL(started()),trans,SLOT(fileTransStart()));
                //connect(trans, SIGNAL(fileTransFinished()), thread, SLOT(quit()));
                connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
                connect(thread, SIGNAL(finished()), trans, SLOT(deleteLater()));
                thread->start();
            }
            mSelf.fileQueue.removeAt(i);
            break;
        }
    }
}

void QIPMSG::qIpMsgFileTransProgress(quint32 fileId,int progress)
{
   //qDebug()<<"File ID"<<fileId<<"Progress"<<progress;
   for(int i =0; i< mUsers.count();i++)
   {
       if(mUsers.at(i)->chatForm != nullptr)
       {
            for(int j = 0; j< mUsers.at(i)->chatForm->fileList.count();j++)
            {
                if(mUsers.at(i)->chatForm->fileList.at(j)->fileId == fileId)
                {
                    mUsers.at(i)->chatForm->updateFileProgress(j,progress);
                    return;
                }
            }
        }
   }
}


void QIPMSG::qIpMsgFileTransError(quint32 fileId,int progress)
{
   //qDebug()<<"File ID"<<fileId<<"Progress"<<progress;
   for(int i =0; i< mUsers.count();i++)
   {
       if(mUsers.at(i)->chatForm != nullptr)
       {
            for(int j = 0; j< mUsers.at(i)->chatForm->fileList.count();j++)
            {
                if(mUsers.at(i)->chatForm->fileList.at(j)->fileId == fileId)
                {
                    mUsers.at(i)->appendChatHistory(toUnicode(mSelf.userNickName+" "+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toUtf8()+"\n"));
                    mUsers.at(i)->appendChatHistory(tr("Send")+" \""+mUsers.at(i)->chatForm->fileList.at(j)->info.absoluteFilePath+"\" "+tr("to")+" "+mUsers.at(i)->userId+tr(" error"));
                    mUsers.at(i)->appendChatHistory("\n");
                    mUsers.at(i)->updateChatFormHistory();
                    mUsers.at(i)->chatForm->updateFileError(j,progress);
                    mUsers.at(i)->chatForm->delFixedShareFile(j);
                    return;
                }
            }
        }
   }
}

void QIPMSG::qIpMsgFileTransFinished(quint32 fileId)
{
   int i = 0;
   qDebug()<<"File ID"<<fileId<<"trans finished";
   for(i =0; i< mUsers.count();i++)
   {
       if(mUsers.at(i)->chatForm != nullptr)
       {
            for(int j = 0; j< mUsers.at(i)->chatForm->fileList.count();j++)
            {
                if(mUsers.at(i)->chatForm->fileList.at(j)->fileId == fileId)
                {
                    mUsers.at(i)->appendChatHistory(toUnicode(mSelf.userNickName+" "+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toUtf8()+"\n"));
                    mUsers.at(i)->appendChatHistory(tr("Send")+" \""+mUsers.at(i)->chatForm->fileList.at(j)->info.absoluteFilePath+"\" "+tr("to")+" "+mUsers.at(i)->userId+tr(" finished "));
                    mUsers.at(i)->appendChatHistory("\n");
                    mUsers.at(i)->updateChatFormHistory();
                    mUsers.at(i)->chatForm->fileList.at(j)->fileTranStatus = FILE_TRANS_STATUS_FINISHED;
                    mUsers.at(i)->chatForm->delFixedShareFile(j);
                    return;
                }
            }
        }
   }
}
