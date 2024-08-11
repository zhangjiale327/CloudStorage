#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H
#include"protocol.h"
#include"msghandle.h"
#include <QObject>
#include <QTcpSocket>
class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    ~MyTcpSocket();
    QString strLoginName;//记录登录成功的用户名
    void sendPDU(PDU*respdu);
    PDU* PDUhandle(PDU*pdu);
    QByteArray Buffer;//存储当前读到的所有消息
    MsgHandle* mh;

public slots:
    void recvMsg(); //定义接受客户端消息的槽函数
    void clientOffline(); //与客户端断开连接
signals:
    void offline(MyTcpSocket*socket);//信号 发送给tcpserver类 使tcpserver类从socket队列中 删除该socket
};

#endif // MYTCPSOCKET_H
