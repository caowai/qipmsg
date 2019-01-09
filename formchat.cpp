#include "formchat.h"
#include "ui_formchat.h"
#include "qipmsg.h"
#include <QFileDialog>
#include <QProgressBar>

static uint32_t g_send_file_id = 100;

QString FormChat::FileSizeConvert(qint64 size)
{
    if(size > 1024*1024*1024)
    {
        return QString::number(size/(1024*1024*1024.0),10,1)+"GB";
    }
    else if(size > 1024*1024)
    {
        return QString::number(size/(1024*1024.0),10,1)+"MB";
    }
    else if(size > 1024)
    {
        return QString::number(size/(1024.0),10,1)+"KB";
    }
    else
        return QString::number(size)+"B";
}
FormChat::FormChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormChat)
{
    QAction *addfileAction = nullptr;
    QAction *delfileAction = nullptr;
    QAction *delAllAction = nullptr;
    QAction *acceptfileAction = nullptr;
    QAction *rejectfileAction = nullptr;
    QAction *acceptAllAction = nullptr;
    QAction *rejectAllAction = nullptr;
    mSendFileCmdMenu = new QMenu(this);
    mRecvFileCmdMenu = new QMenu(this);
    QStringList header;
    addfileAction = mSendFileCmdMenu->addAction(tr("Add File"));
    delfileAction = mSendFileCmdMenu->addAction(tr("Remove File"));
    delAllAction = mSendFileCmdMenu->addAction(tr("Remove All"));

    acceptfileAction = mRecvFileCmdMenu->addAction(tr("Accept"));
    rejectfileAction = mRecvFileCmdMenu->addAction(tr("Reject"));
    acceptAllAction = mRecvFileCmdMenu->addAction(tr("Accpet All"));
    rejectAllAction = mRecvFileCmdMenu->addAction(tr("Reject All"));

    //mSendFileCmdMenu->addAction("Add Dir");
    ui->setupUi(this);
    ui->labelChatClient->setText(tr("Client"));
    ui->labelChatHost->setText(tr("Host"));
    ui->labelChatGroup->setText(tr("Group"));
    ui->pushButtonChatSent->setText(tr("Send(&S)"));

    //ui->textEditChat->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidgetSendFileList->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidgetSendFileList->verticalHeader()->setVisible(false);

    ui->tableWidgetSendFileList->setFrameShape(QFrame::NoFrame);

    ui->tableWidgetRecvFileList->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidgetRecvFileList->verticalHeader()->setVisible(false);

    ui->tableWidgetRecvFileList->setFrameShape(QFrame::NoFrame);

    header<<tr("File Name")<<tr("File Size");
    ui->tableWidgetSendFileList->setColumnCount(2);
    ui->tableWidgetSendFileList->setHorizontalHeaderLabels(header);

    ui->tableWidgetRecvFileList->setColumnCount(2);
    ui->tableWidgetRecvFileList->setHorizontalHeaderLabels(header);

    connect(ui->pushButtonChatSent,SIGNAL(clicked()),this,SLOT(chatSend()));


    connect(addfileAction,SIGNAL(triggered()),this,SLOT(addShareFile()));
    connect(delfileAction,SIGNAL(triggered()),this,SLOT(delShareFile()));
    connect(delAllAction,SIGNAL(triggered()),this,SLOT(delAllShareFile()));
#if 0
    connect(acceptfileAction,SIGNAL(triggered()),this,SLOT(acceptShareFile()));
    connect(acceptAllAction,SIGNAL(triggered()),this,SLOT(acceptAllShareFile()));
    connect(rejectfileAction,SIGNAL(triggered()),this,SLOT(rejectShareFile()));
    connect(rejectAllAction,SIGNAL(triggered()),this,SLOT(rejectAllShareFile()));
#endif

    connect(ui->tableWidgetSendFileList,SIGNAL(customContextMenuRequested(const QPoint)),this,SLOT(on_tableWidgetSendFileList_customContextMenuRequested(const QPoint)));
    connect(ui->tableWidgetRecvFileList,SIGNAL(customContextMenuRequested(const QPoint)),this,SLOT(on_tableWidgetRecvFileList_customContextMenuRequested(const QPoint)));
    //connect(this,SIGNAL(close()),this,SLOT(deleteLater()));
}

FormChat::~FormChat()
{
    if(mSendFileCmdMenu)
    {
        delete  mSendFileCmdMenu;
        mSendFileCmdMenu = nullptr;
    }
    ui->tableWidgetSendFileList->clear();
    ui->tableWidgetSendFileList->setRowCount(0);
    ui->tableWidgetRecvFileList->clear();
    ui->tableWidgetRecvFileList->setRowCount(0);
    delete ui;
}


void FormChat::addShareFile()
{
    fileEntryT *fileNode =  nullptr;
    QString file = QFileDialog::getOpenFileName(nullptr,nullptr,nullptr,"");
    QProgressBar *sizeBar = nullptr;
    QFileInfo fileinfo;
    int current_row = ui->tableWidgetSendFileList->rowCount();

    if(file.length() == 0)
    {
        qDebug("no file selected");
        return;
    }

    fileinfo.setFile(file);
    if(fileinfo.isFile() == false)
    {
        qDebug("invalid file type");
        return;
    }
    //emit addSendFile(file);
    ui->tableWidgetSendFileList->insertRow(current_row);
    ui->tableWidgetSendFileList->setItem(current_row,0,new QTableWidgetItem(fileinfo.fileName()));
    sizeBar = new QProgressBar();
    sizeBar->setFormat(FileSizeConvert(fileinfo.size()));
    sizeBar->setTextVisible(true);
    sizeBar->setValue(0);
    ui->tableWidgetSendFileList->setCellWidget(current_row,1,sizeBar);

    fileNode = new fileEntryT();
    fileNode->info.fileName = fileinfo.fileName();
    fileNode->info.size = fileinfo.size();
    fileNode->info.permission = fileinfo.permissions();
    fileNode->info.absoluteFilePath = fileinfo.absoluteFilePath();
    fileNode->info.metadataChangeTime = fileinfo.metadataChangeTime().toTime_t();

    fileNode->fileId = g_send_file_id++;
    fileNode->fileHost = QHostAddress(ui->labelChatHostValue->text()).toIPv4Address();
    fileNode->fileOffset = 0;
    fileNode->fileTranStatus = FILE_TRANS_STATUS_IDLE;
    fileList.append(fileNode);
    //qDebug()<<ui->tableWidgetSendFileList->rowCount()<<QFileInfo(file).fileName()+"|"+QString::number(fileinfo.size(),10);
}

void FormChat::delFixedShareFile(int index)
{
    if(index >=0 && index < ui->tableWidgetSendFileList->columnCount())
    {
        //emit delSendFile(ui->tableWidgetSendFileList->currentRow());
        fileList.removeAt(index);
        ui->tableWidgetSendFileList->removeRow(index);
    }
}
void FormChat::delShareFile()
{
    int index = ui->tableWidgetSendFileList->currentRow();
    delFixedShareFile(index);
}

void FormChat::delAllShareFile()
{
    //emit delAllSendFile();
    fileList.clear();
    ui->tableWidgetSendFileList->clearContents();
    ui->tableWidgetSendFileList->setRowCount(0);
}


void FormChat::addRemoteShareFile(fileEntryT *newfile)
{
    QProgressBar *sizeBar = nullptr;
    int current_row = ui->tableWidgetRecvFileList->rowCount();
#if 0
    fileEntryT *fileNode =  nullptr;
    fileNode = new fileEntryT();
    fileNode->info.fileName = newfile->info.fileName;
    fileNode->info.size = newfile->info.size;
    fileNode->info.permission = newfile->info.permission;
    fileNode->info.absoluteFilePath = newfile->info.absoluteFilePath;
    fileNode->info.metadataChangeTime = newfile->info.metadataChangeTime;
    fileNode->fileId = newfile->fileId;
    fileNode->fileHost = newfile->fileHost;
    fileNode->filePort = newfile->filePort;
    fileNode->fileOffset = newfile->fileOffset;
    fileNode->fileTranStatus = FILE_TRANS_STATUS_IDLE;
    fileList.append(fileNode);
#else
    fileList.append(newfile);
#endif
    //emit addSendFile(file);
    ui->tableWidgetRecvFileList->insertRow(current_row);
    ui->tableWidgetRecvFileList->setItem(current_row,0,new QTableWidgetItem(newfile->info.fileName));
    sizeBar = new QProgressBar();
    sizeBar->setFormat(FileSizeConvert(newfile->info.size));
    sizeBar->setTextVisible(true);
    sizeBar->setValue(0);
    ui->tableWidgetRecvFileList->setCellWidget(current_row,1,sizeBar);
}

void FormChat::acceptShareFile()
{
    qDebug()<<__FUNCTION__;
}

void FormChat::rejectShareFile()
{
    qDebug()<<__FUNCTION__;
}

void FormChat::acceptAllShareFile()
{
    qDebug()<<__FUNCTION__;
}

void FormChat::rejectAllShareFile()
{
    qDebug()<<__FUNCTION__;
}
bool FormChat::event(QEvent *event)
{
    //Language change event from QIPMSG
    if(event->type() == QEvent::LanguageChange)
    {
        //qDebug()<<"will change language";
        ui->retranslateUi(this);
        setHost(mHost);
        setClient(mClient);
        setUserName(mUser);
        setGroupName(mGroup);
    }
    //Must call QWidget:event(event) or the process to be broken

    return QWidget::event(event);
}

QTextEdit * FormChat::getChatHistory()
{
    return ui->textEditHistory;
}

QTextEdit * FormChat::getChat()
{
    return ui->textEditChat;
}

void FormChat::updateChatHistory(QString data)
{
    ui->textEditHistory->setText(data);
    ui->textEditHistory->moveCursor(QTextCursor::End);
    return;
}
void FormChat::chatSend()
{
    emit sent(ui->textEditChat->toPlainText(),QHostAddress(ui->labelChatHostValue->text()));
}

void FormChat::setUserName(QString value)
{

    ui->labelChatNameValue->setText(value);
    mUser = value;
}

void FormChat::setHost(QString value)
{
    ui->labelChatHostValue->setText(value);
    mHost = value;
}

void FormChat::setGroupName(QString value)
{
    ui->labelChatGroupValue->setText(value);
    mGroup = value;
}

//Update IPMSG Client information in chat form.
void FormChat::setClient(QString value)
{
    if(value.contains("iptux"))
    {
        ui->labelChatClientValue->setText("IPTUX");
    }
    else if(value.left(5) == "1_lbt")
    {
        ui->labelChatClientValue->setText("FEIQIU");
    }
    else if(value == "1")
    {
        ui->labelChatClientValue->setText("IPMSG");
    }
    else if(value.left(strlen("1_QIPMSG")) == "1_QIPMSG")
    {
        ui->labelChatClientValue->setText("QIPMSG");
    }
    else
    {
        ui->labelChatClientValue->setText(value);
    }
    mClient = ui->labelChatClientValue->text();
}

void FormChat::on_tableWidgetSendFileList_customContextMenuRequested(const QPoint &pos)
{
    mSendFileCmdMenu->exec(QCursor::pos());
}

void FormChat::on_tableWidgetRecvFileList_customContextMenuRequested(const QPoint &pos)
{
    mRecvFileCmdMenu->exec(QCursor::pos());
}

void FormChat::updateFileProgress(quint32 fileId,int progress)
{

    QProgressBar *sizeBar = new QProgressBar();
    sizeBar->setFormat(FileSizeConvert(fileList.at(fileId)->info.size));
    sizeBar->setTextVisible(true);
    sizeBar->setValue(progress);
    ui->tableWidgetSendFileList->setCellWidget(fileId,1,sizeBar);
}

void FormChat::updateFileError(quint32 fileId,int progress)
{

    QProgressBar *sizeBar = new QProgressBar();
    sizeBar->setFormat(tr("Error"));
    sizeBar->setTextVisible(true);
    sizeBar->setValue(progress);
    ui->tableWidgetSendFileList->setCellWidget(fileId,1,sizeBar);
}
