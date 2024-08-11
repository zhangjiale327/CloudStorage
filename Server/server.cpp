#include "server.h"
#include "ui_server.h"
#include "mytcpserver.h"
#include <QTcpSocket>
Server::Server(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);
    loadconfig();
    MyTcpServer::getinstance().listen(QHostAddress(m_strIp),m_Port);//进行监听客户端方面的连接
    //如果监听到客户端有连接 自动调用一个函数incomingConnetion函数
    //但是这个函数什么都没实现 所以需要我们继承tcpserver类 去实现它
}

Server::~Server()
{
    delete ui;

}

void Server::loadconfig()
{
    //创建Qfile类对象 用于读取配置文件内容
    QFile file(":/Server.config");
    if(file.open(QIODevice::ReadOnly)){//只读打开文件
        QByteArray baData=file.readAll();//读取配置文件中的全部内容
        QString strData(baData);//将配置文件内容转换为字符串
        QStringList strList=strData.split("\r\n");//将字符串按照"\r\n"分离 便于打印ip和端口号
        //qDebug()<<strList;//打印日志文件 防止错误
        m_strIp=strList.at(0);//将ip转换为字符串
        m_Port=strList[1].toUShort();//将端口号转换为short16位
        m_RootPath=strList[2];//读取文件地址
        qDebug()<<"IP:"<<m_strIp<<"Port:"<<m_Port<<"RootPath:"<<m_RootPath;//打印配置文件中的ip和端口号
        file.close();//关闭文件
    }else{
        qDebug()<<"打开配置文件失败";
    }
}

Server &Server::getinstance()
{
    static Server instance;
    return instance;
}

QString &Server::get_RootPath()
{
    return m_RootPath;
}

