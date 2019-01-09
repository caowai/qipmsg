#ifndef IPMSGTRANSTHREAD_H
#define IPMSGTRANSTHREAD_H

#include <QObject>
#include <QThread>

class IpMsgTransThread : public QThread
{
    Q_OBJECT
public:
    IpMsgTransThread();    
};

#endif // IPMSGTRANSTHREAD_H
