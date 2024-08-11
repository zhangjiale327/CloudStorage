#ifndef MSGHANDLE_H
#define MSGHANDLE_H
#include"protocol.h"

#include <QFile>
#include <QString>
class MyTcpSocket;
class MsgHandle
{
public:
    MsgHandle();
    ~MsgHandle();
    PDU*regist(PDU *pdu);
    PDU*login(PDU *pdu,QString& name);
    PDU*findfriend(PDU *pdu);
    PDU*is_online(PDU*pdu);
    PDU*addFriend(PDU*pdu);
    PDU*agreeFriend(PDU*pdu);
    PDU*unagreeFriend(PDU*pdu);
    PDU*flushOnlineFriend(PDU*pdu);
    PDU*logoff(PDU*pdu);
    PDU*del_online_friend(PDU*pdu);
    void Chat(PDU*pdu);
    PDU* mkdir(PDU*pdu);
    PDU* flushFile(PDU*pdu);
    PDU* del_dir(PDU*pdu);
    PDU* del_file(PDU*pdu);
    PDU* rename(PDU*pdu);
    PDU* mvfile(PDU* pdu);
    PDU* upLoadfile(PDU* pdu);
    PDU* uploaddata(PDU*pdu);
    PDU* shareFile(PDU*pdu);
    PDU* shareAgreeFile(PDU*pdu);
    PDU* loadFile(PDU*pdu);
    PDU* loadFileData(MyTcpSocket *socket);
    QFile m_upLoadFile;
    qint64 m_upLoadTotal;
    qint64 m_upLoadreserve;
    bool upLoad;

    QFile m_LoadFile;
    bool LoadFile;
    //MyTcpSocket* mt;
};
#endif // MSGHANDLE_H
