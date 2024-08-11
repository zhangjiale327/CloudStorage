#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QWidget>
#include "index.h"
#include"reshandle.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QWidget
{
    Q_OBJECT

public:
    void getConfig();//获取配置文件内容
    void showConnect();//显示客户端与服务器连接结果
    void readPdu();//接受服务器的响应消息
    QTcpSocket& get_QTcpSocket();//获取套接字
    static Client& getinstance();//全局访问节点 获取单例对象
    void PDUhandle(PDU*pdu);
    void sendPdu(PDU*pdu);//发送消息
    QString& get_strLoginName();
    QString& get_strVer();
    QString get_RootPath();
    QByteArray Buffer;//存储当前读到的所有消息
    ~Client();
    ResHandle*rh;
private slots:
    void on_Regist_clicked();//用于注册的槽函数
    void on_Login_clicked();//用于登录的槽函数

    void on_logoff_clicked();
private:
    Ui::Client *ui;
    QString strIP;//客户端ip地址
    quint16 usPort;//客户端端口号
    QString strLoginName;//获取当前登录的用户名
    QString RootPath;//获取配置文件中的路径
    QString strVer;
    QTcpSocket m_tcpSocket;//用于连接并通信的客户端socket
    //为了实现单例模式 将默认构造函数私有化 拷贝构造删除 拷贝复制运算符删除
    Client(QWidget *parent = nullptr);
    Client(const Client&instance)=delete;
    Client& operator=(const Client&instance)=delete;
};
#endif // CLIENT_H
