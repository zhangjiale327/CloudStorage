#include "mytcpserver.h"
//实现MyTcpServer单例模式
MyTcpServer &MyTcpServer::getinstance()
{
    static MyTcpServer instance;//将单例对象创建为静态目的是为了 防止重复创建该实例 以及多线程中多个
    //线程会同时调用这个函数 创建多个实例对象 但是在c++11后 静态对象是为线程安全的
    return instance;
}

void MyTcpServer::incomingConnection(qintptr handle)
{
    qDebug()<<"客户端已经连接";
    //初始化mytcpsocket 并将其加入到客户端socket列表中
    MyTcpSocket *pTcpSocket=new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(handle);


     //将连接后的客户端加入到列表中
    m_TcpSocketList.append(pTcpSocket);

    QObject::connect(pTcpSocket,&MyTcpSocket::offline,this,&MyTcpServer::deleteSocket);
    //与socket类中的信号建立连接 执行槽函数 将该socket从socket队列中删除
}

void MyTcpServer::resend(char *strTarName, PDU *pdu)
{
    qDebug()<<"resend"<<pdu->uiMsgType;
    for(int i=0;i<MyTcpServer::getinstance().m_TcpSocketList.size();i++){
        if(m_TcpSocketList.at(i)->strLoginName==strTarName){
            m_TcpSocketList.at(i)->write((char*)pdu,pdu->uiPDULen);
        }
    }
}

void MyTcpServer::deleteSocket(MyTcpSocket*socket)//从队列中删除断开的客户端socket
{
    if(socket==NULL) {
        return;
    }
    m_TcpSocketList.removeOne(socket);
    qDebug()<<"当前连接的客户端个数:"
        <<m_TcpSocketList.size();
    for(int i=0;i<m_TcpSocketList.size();i++){
        qDebug()<<m_TcpSocketList.at(i)->strLoginName;
    }
    socket->deleteLater();//延时删除该socket 可能我们断开后 该socket还有点用
    socket=NULL;
}



MyTcpServer::MyTcpServer()
{

}


