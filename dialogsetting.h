#ifndef DIALOGSETTING_H
#define DIALOGSETTING_H

#include <QDialog>
#include <QPushButton>

namespace Ui {
class DialogSetting;
}

class DialogSetting : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetting(QWidget *parent = nullptr);
    void setUserName(QByteArray userId);
    void setUserNickName(QByteArray userNickName);
    void setGroupName(QByteArray groupName);
    void setStartIp(unsigned int ip);
    void setEndIp(unsigned int ip);
    QString getUserName();
    QString getUserNickName();
    QString getGroupName();
    QString getStartIp();
    QString getEndIp();
    ~DialogSetting();
signals:
    void setDialogClose();
    void setDialogAccepted();
    void setDialogRejected();
private:
    Ui::DialogSetting *ui;
protected:
    void closeEvent(QCloseEvent *event);
    bool event(QEvent *event);
private slots:
    void on_lineEditUserName_textEdited(const QString &arg1);
    void on_lineEditNickName_textEdited(const QString &arg1);
    void on_lineEditGroupName_textEdited(const QString &arg1);
    void on_lineEditStartIp_textEdited(const QString &arg1);
    void on_lineEditEndIp_textEdited(const QString &arg1);
    void on_pushButtonAccept_clicked();
    void on_pushButtonCancel_clicked();
};

#endif // DIALOGSETTING_H
