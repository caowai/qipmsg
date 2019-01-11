#ifndef IPMSGUDPSESSION_H
#define IPMSGUDPSESSION_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QHostAddress>
#include <QUdpSocket>
#include "ipmsguser.h"

#define IPMSG_VERSION   "1_QIPMSGV01"

#define IPMSG_DEFAULT_USERNAME  "QIPMSG.U"
#define IPMSG_DEFAULT_NICKNAME  "QIPMSG.N"
#define IPMSG_DEFAULT_GROUPNAME "QIPMSG"

#define IPMSG_RETRY_COUNT   4

#define IPMSG_PORT 0x979

/*  command  */
#define IPMSG_NOOPERATION 0x00000000UL

#define IPMSG_BR_ENTRY 0x00000001UL
#define IPMSG_BR_EXIT 0x00000002UL
#define IPMSG_ANSENTRY 0x00000003UL
#define IPMSG_BR_ABSENCE 0x00000004UL

#define IPMSG_BR_ISGETLIST 0x00000010UL
#define IPMSG_OKGETLIST 0x00000011UL
#define IPMSG_GETLIST 0x00000012UL
#define IPMSG_ANSLIST 0x00000013UL
#define IPMSG_BR_ISGETLIST2 0x00000018UL

#define IPMSG_SENDMSG 0x00000020UL
#define IPMSG_RECVMSG 0x00000021UL
#define IPMSG_READMSG 0x00000030UL
#define IPMSG_DELMSG 0x00000031UL
#define IPMSG_ANSREADMSG 0x00000032UL

#define IPMSG_GETINFO 0x00000040UL
#define IPMSG_SENDINFO 0x00000041UL

#define IPMSG_GETABSENCEINFO 0x00000050UL
#define IPMSG_SENDABSENCEINFO 0x00000051UL

#define IPMSG_GETFILEDATA 0x00000060UL
#define IPMSG_RELEASEFILES 0x00000061UL
#define IPMSG_GETDIRFILES 0x00000062UL

#define IPMSG_GETPUBKEY 0x00000072UL
#define IPMSG_ANSPUBKEY 0x00000073UL

/*  option for all command  */
#define IPMSG_ABSENCEOPT 0x00000100UL
#define IPMSG_SERVEROPT 0x00000200UL
#define IPMSG_DIALUPOPT 0x00010000UL
#define IPMSG_FILEATTACHOPT 0x00200000UL
#define IPMSG_ENCRYPTOPT 0x00400000UL
#define IPMSG_UTF8OPT 0x00800000UL

/*  option for send command  */
#define IPMSG_SENDCHECKOPT 0x00000100UL
#define IPMSG_SECRETOPT 0x00000200UL
#define IPMSG_BROADCASTOPT 0x00000400UL
#define IPMSG_MULTICASTOPT 0x00000800UL
#define IPMSG_NOPOPUPOPT 0x00001000UL
#define IPMSG_AUTORETOPT 0x00002000UL
#define IPMSG_RETRYOPT 0x00004000UL
#define IPMSG_PASSWORDOPT 0x00008000UL
#define IPMSG_NOLOGOPT 0x00020000UL
#define IPMSG_NEWMUTIOPT 0x00040000UL
#define IPMSG_NOADDLISTOPT 0x00080000UL
#define IPMSG_READCHECKOPT 0x00100000UL
#define IPMSG_SECRETEXOPT (IPMSG_READCHECKOPT | IPMSG_SECRETOPT)

#define IPMSG_NO_REPLY_OPTS (IPMSG_BROADCASTOPT | IPMSG_AUTORETOPT)

/* encryption flags for encrypt command */
#define IPMSG_RSA_512 0x00000001UL
#define IPMSG_RSA_1024 0x00000002UL
#define IPMSG_RSA_2048 0x00000004UL
#define IPMSG_RC2_40 0x00001000UL
#define IPMSG_RC2_128 0x00004000UL
#define IPMSG_RC2_256 0x00008000UL
#define IPMSG_BLOWFISH_128 0x00020000UL
#define IPMSG_BLOWFISH_256 0x00040000UL
#define IPMSG_AES_128 0x00100000UL
#define IPMSG_AES_192 0x00200000UL
#define IPMSG_AES_256 0x00400000UL
#define IPMSG_SIGN_STAMPOPT 0x01000000UL
#define IPMSG_SIGN_MD5 0x10000000UL
#define IPMSG_SIGN_SHA1 0x20000000UL

/* compatibility for Win beta version */
#define IPMSG_RC2_40OLD 0x00000010UL        // for beta1-4 only
#define IPMSG_RC2_128OLD 0x00000040UL       // for beta1-4 only
#define IPMSG_BLOWFISH_128OLD 0x00000400UL  // for beta1-4 only
#define IPMSG_RC2_40ALL (IPMSG_RC2_40 | IPMSG_RC2_40OLD)
#define IPMSG_RC2_128ALL (IPMSG_RC2_128 | IPMSG_RC2_128OLD)
#define IPMSG_BLOWFISH_128ALL (IPMSG_BLOWFISH_128 | IPMSG_BLOWFISH_128OLD)

/* file types for fileattach command */
#define IPMSG_FILE_REGULAR 0x00000001UL
#define IPMSG_FILE_DIR 0x00000002UL
#define IPMSG_FILE_RETPARENT 0x00000003UL  // return parent directory
#define IPMSG_FILE_SYMLINK 0x00000004UL
#define IPMSG_FILE_CDEV 0x00000005UL     // for UNIX
#define IPMSG_FILE_BDEV 0x00000006UL     // for UNIX
#define IPMSG_FILE_FIFO 0x00000007UL     // for UNIX
#define IPMSG_FILE_RESFORK 0x00000010UL  // for Mac

/* file attribute options for fileattach command */
#define IPMSG_FILE_RONLYOPT 0x00000100UL
#define IPMSG_FILE_HIDDENOPT 0x00001000UL
#define IPMSG_FILE_EXHIDDENOPT 0x00002000UL  // for MacOS X
#define IPMSG_FILE_ARCHIVEOPT 0x00004000UL
#define IPMSG_FILE_SYSTEMOPT 0x00008000UL

/* extend attribute types for fileattach command */
#define IPMSG_FILE_UID 0x00000001UL
#define IPMSG_FILE_USERNAME 0x00000002UL  // uid by string
#define IPMSG_FILE_GID 0x00000003UL
#define IPMSG_FILE_GROUPNAME 0x00000004UL  // gid by string
#define IPMSG_FILE_PERM 0x00000010UL       // for UNIX
#define IPMSG_FILE_MAJORNO 0x00000011UL    // for UNIX devfile
#define IPMSG_FILE_MINORNO 0x00000012UL    // for UNIX devfile
#define IPMSG_FILE_CTIME 0x00000013UL      // for UNIX
#define IPMSG_FILE_MTIME 0x00000014UL
#define IPMSG_FILE_ATIME 0x00000015UL
#define IPMSG_FILE_CREATETIME 0x00000016UL
#define IPMSG_FILE_CREATOR 0x00000020UL     // for Mac
#define IPMSG_FILE_FILETYPE 0x00000021UL    // for Mac
#define IPMSG_FILE_FINDERINFO 0x00000022UL  // for Mac
#define IPMSG_FILE_ACL 0x00000030UL
#define IPMSG_FILE_ALIASFNAME 0x00000040UL    // alias fname
#define IPMSG_FILE_UNICODEFNAME 0x00000041UL  // UNICODE fname

class IpMsgUdpSession : public QObject
{
    Q_OBJECT
public:
    explicit IpMsgUdpSession(QObject *parent = nullptr);
    bool IpMsgUdpSessionValid();
    void ipMsgUserRefresh(IpMsgUser *user,unsigned int startip, unsigned int endip);
    void ipMsgRespOK(IpMsgUser *user, IpMsgUser *dest);
    void ipMsgSendData(IpMsgUser *user, QHostAddress dest,QByteArray data);
    void ipMsgSendFiles(IpMsgUser *user,QHostAddress dest, QList <fileEntryT *>files);
    void ipMsgRespBr(IpMsgUser *user, IpMsgUser *dest);
    void ipMsgUserExit(IpMsgUser *user);
    bool mUdpBind = false;
signals:
    void ipMsgUdpSessionDataReady(QHostAddress src, QByteArray data);

public slots:


private slots:
    void ipMsgUdpDataHandle();
protected:
    void timerEvent(QTimerEvent *event);

private:
    QUdpSocket ipMsgSock;
    void ipMsgPktSend(QHostAddress address, uint16_t port,QByteArray data);
    unsigned int ipMsgHostCurrent;
    unsigned int ipMsgHostStart;
    unsigned int ipMsgHostEnd;
    IpMsgUser *mUser = nullptr;
    int32_t  ipMsgHostDetectTimer = 0;
};

#endif // IPMSGUDPSESSION_H
