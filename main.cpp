#include "qipmsg.h"
#include <QApplication>

#include <QTextCodec>
#include <QTranslator>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/userIcon/chat.png"));

    QIPMSG w;
    w.show();
    return a.exec();
}
