#ifndef FILETRANSWORKER_H
#define FILETRANSWORKER_H

#include <QObject>
#include <QTcpSocket>

class FileTransWorker : public QObject
{
    Q_OBJECT
public:
    explicit FileTransWorker(QObject *parent = nullptr);
    qintptr mHandle;
    QString mFilename;
    uint32_t mFileId;
    bool mFileTranStopFlag = false;
    int mProgress = 0;

signals:
    void fileTransProgress(quint32 id,int val);
    void fileTransFinished(quint32 id);
    void fileTransError(quint32 id,int val);
public slots:
    void fileTransStart();

};

#endif // FILETRANSWORKER_H
