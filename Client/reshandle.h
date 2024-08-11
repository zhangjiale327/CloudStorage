#ifndef RESHANDLE_H
#define RESHANDLE_H

#include <QWidget>
#include"protocol.h"
#include<stdlib.h>
#include<QString>
class ResHandle : public QWidget
{
    Q_OBJECT
public:
    explicit ResHandle(QWidget *parent = nullptr);
    void res_regist(PDU*pdu);
    void res_login(PDU*pdu);
    void res_findfriend(PDU*pdu);
    void res_is_online(PDU*pdu);
    void res_addfriend(PDU*pdu);
    void res_addfriend_agree(PDU*pdu);
    void res_addfriend_agree_respend(PDU*pdu);
    void res_addfriend_unagree_respend(PDU*pdu);
    void res_flush_friend_online(PDU*pdu);
    void res_logoff(PDU*pdu);
    void res_del_Online_Friend(PDU*pdu);
    void chat(PDU*pdu);
    void mkdir(PDU*pdu);
    void flushFile(PDU*pdu);
    void deldir(PDU*pdu);
    void delfile(PDU*pdu);
    void rename(PDU*pdu);
    void mvfile(PDU*pdu);
    void uploadfile(PDU*pdu);
    void uploaddata(PDU*pdu);
    void sharefile(PDU*pdu);
    void sharefilerequest(PDU*pdu);
    void sharefileagree(PDU*pdu);
    void loadfile(PDU*pdu);
    void loadFilData(PDU*pdu);
signals:

};

#endif // RESHANDLE_H
