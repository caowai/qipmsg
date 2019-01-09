#ifndef DIALOGSENDFILEMENU_H
#define DIALOGSENDFILEMENU_H

#include <QDialog>

namespace Ui {
class DialogSendFileMenu;
}

class DialogSendFileMenu : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSendFileMenu(QWidget *parent = nullptr);
    ~DialogSendFileMenu();

private:
    Ui::DialogSendFileMenu *ui;
};

#endif // DIALOGSENDFILEMENU_H
