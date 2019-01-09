#include "dialogsetting.h"
#include "ui_dialogsetting.h"
#include <QHostAddress>
#include <QDialogButtonBox>
#include <QCloseEvent>
#include <QEvent>

#include <unistd.h>

DialogSetting::DialogSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetting)
{
    ui->setupUi(this);
}

DialogSetting::~DialogSetting()
{
    delete ui;
}

bool DialogSetting::event(QEvent *event)
{
    //Language changed.
    if(event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    //Must call QWidget:event(event) or the process to be broken
    return QWidget::event(event);
}

void DialogSetting::closeEvent(QCloseEvent *event)
{
    emit setDialogClose();
}


void DialogSetting::setUserName(QByteArray userId)
{
    ui->lineEditUserName->setText(userId);
}
void DialogSetting::setUserNickName(QByteArray userNickName)
{
    ui->lineEditNickName->setText(userNickName);
}
void DialogSetting::setGroupName(QByteArray groupName)
{
    ui->lineEditGroupName->setText(groupName);
}

void DialogSetting::setStartIp(unsigned int ip)
{
    QHostAddress add;
    add.setAddress(ip);
    ui->lineEditStartIp->setText(add.toString());
}
void DialogSetting::setEndIp(unsigned int ip)
{
    QHostAddress add;
    add.setAddress(ip);
    ui->lineEditEndIp->setText(add.toString());
}



QString DialogSetting::getUserName()
{
    return ui->lineEditUserName->text();
}

QString DialogSetting::getUserNickName()
{
    return ui->lineEditNickName->text();
}


QString DialogSetting::getGroupName()
{
    return ui->lineEditGroupName->text();
}
QString DialogSetting::getStartIp()
{
    return ui->lineEditStartIp->text();
}
QString DialogSetting::getEndIp()
{
    return ui->lineEditEndIp->text();
}

void DialogSetting::on_lineEditUserName_textEdited(const QString &arg1)
{
    if(arg1.length()>32  || arg1.length() == 0)
    {
        ui->labelWarning->setText(tr("User name length is from 1-32"));
        return;
    }
    else
    {
        ui->labelWarning->setText("Valid user name");
    }
    return;
}

void DialogSetting::on_lineEditNickName_textEdited(const QString &arg1)
{
    if(arg1.length()>32  || arg1.length() == 0)
    {
        ui->labelWarning->setText(tr("Nick name length is from 1-32"));
        return;
    }
    else
    {
        ui->labelWarning->setText("Valid nick name");
    }
    return;
}

void DialogSetting::on_lineEditGroupName_textEdited(const QString &arg1)
{
    if(arg1.length()>32  || arg1.length() == 0)
    {
        ui->labelWarning->setText(tr("Group name length is from 1-32"));
        return;
    }
    else
    {
        ui->labelWarning->setText("Valid group name");
    }
    return;
}

void DialogSetting::on_lineEditStartIp_textEdited(const QString &arg1)
{
    QHostAddress tmp;
    if(false == tmp.setAddress(arg1))
    {
        ui->labelWarning->setText(tr("Invalid Start IP Address"));
    }
    else
    {
        ui->labelWarning->setText(tr("Valid Start IP Address"));
    }
    return;
}

void DialogSetting::on_lineEditEndIp_textEdited(const QString &arg1)
{
    QHostAddress tmp;
    if(false == tmp.setAddress(arg1))
    {
        ui->labelWarning->setText(tr("Invalid End IP Address"));
    }
    else
    {
        ui->labelWarning->setText(tr("Valid End IP Address"));
    }
    return;
}

void DialogSetting::on_pushButtonAccept_clicked()
{
    QHostAddress start;
    QHostAddress end;
    start.setAddress(ui->lineEditStartIp->text());
    end.setAddress(ui->lineEditEndIp->text());

    if(start.toIPv4Address() > end.toIPv4Address())
    {
        ui->labelWarning->setText(tr("End IP should >= Start IP"));
        return;
    }
    else
    {
        emit setDialogAccepted();
    }
}

void DialogSetting::on_pushButtonCancel_clicked()
{
    emit  setDialogRejected();
}
