#include "dialogsendfilemenu.h"
#include "ui_dialogsendfilemenu.h"

DialogSendFileMenu::DialogSendFileMenu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSendFileMenu)
{
    ui->setupUi(this);
}

DialogSendFileMenu::~DialogSendFileMenu()
{
    delete ui;
}
