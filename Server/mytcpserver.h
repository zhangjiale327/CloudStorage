#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QList>
#include <QDebug>
#include  <QObject>
#include "mytcpsocket.h"
class MyTcpServer : public QTcpServer//建立自己的tcp 因为我们要重写tcpsocket中的comingconnect函数
{
    Q_OBJECT
public:
    static MyTcpServer& getinstance();//获取单例对象的全局节点
    void incomingConnection(qintptr handle);
    //重写tcpserver中的连接函数 当监听到有客户端连接时 服务器会自动调用这个函数
    //函数里面参数代表 用于通信的客户端的socket
    void resend(char* strTarName,PDU*pdu);
public slots:
    void deleteSocket(MyTcpSocket*socket);
private:
    //为了实现单例模式 将默认构造函数私有化 拷贝构造删除 拷贝复制运算符删除
    MyTcpServer();
    MyTcpServer(const MyTcpServer&instance)=delete;
    MyTcpServer& operator=(const MyTcpServer&instance)=delete;

    //建立存储客户端连接后信息的列表
    QList<MyTcpSocket*> m_TcpSocketList;
};
#endif // MYTCPSERVER_H
