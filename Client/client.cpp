#include "client.h"
#include "ui_client.h"
#include "protocol.h"
#include "onlineuser.h"
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QIODevice>
#include <QObject>
#include <QHostAddress>
Client::Client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Client)
{
    ui->setupUi(this);
    getConfig();//获取配置文件内容
    //关联信号槽 信号客户端与服务器连接成功 槽函数输出连接成功标识
    QObject::connect(&m_tcpSocket,&QTcpSocket::connected,this,&Client::showConnect);
    m_tcpSocket.connectToHost(QHostAddress(strIP),usPort);//利用socket去连接客户端
    QObject::connect(&m_tcpSocket,&QTcpSocket::readyRead,this,&Client::readPdu);
}

void Client::getConfig()
{
    QFile file(":/client.config");//获取文件路径
    if(file.open(QIODevice::ReadOnly)){//以只读方式打开文件
        QByteArray baData=file.readAll();//读取文件全部内容
        QString strData=QString(baData);//将文件内容转换为字符串
//        qDebug()<<strData;
        QStringList strList=strData.split("\r\n");//将字符串按照分隔符拆分
        strIP=strList.at((0));
        usPort=strList.at(1).toUShort();//将端口号转换为short类型
        RootPath=strList.at(2);
        qDebug()<<"IP:"<<strIP<<"Port:"<<usPort<<"RootPath:"<<RootPath;//打印配置文件中的ip和端口号
    }else{
        QMessageBox::critical(this,"open config","open config falied");//如果打开配置文件失败 则报错
    }
}

void Client::showConnect()
{
    qDebug()<<"连接服务器成功!";
}

Client &Client::getinstance()
{
    static Client instance;
    //将单例对象创建为静态目的是为了 防止重复创建该实例 以及多线程中多个
    //线程会同时调用这个函数 创建多个实例对象 但是在c++11后 静态对象是为线程安全的
    return instance;
}

void Client::readPdu()
{
//    uint uiPDULen=0;//作为传出参数记得取地址 获取客户端发来消息的总长度 即PDU协议的第一个参数
//    m_tcpSocket.read((char*)&uiPDULen,sizeof(uint));//第一个参数 如果是指针类型的话就不需要&了
//    qDebug()<<"协议总长度为:"<<uiPDULen;
//    //获取实际消息长度
//    uint uiMsgLen=uiPDULen-sizeof(PDU);

//    PDU*pdu=mkPDU(uiMsgLen);
//    //读取PDU中除了消息长度以外的其他内容 需要进行偏移 因为我们总协议长度这个变量已经读取过了
//    m_tcpSocket.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
//    qDebug()<<"接收到的服务器信息:"
//           <<"消息长度:"<<pdu->uiMsgLen
//          <<"消息类型:"<<pdu->uiMsgType
//         <<"消息内容1:"<<pdu->uiData
//        <<"消息内容2:"<<pdu->uiData+32;
    QByteArray data = m_tcpSocket.readAll();
    Buffer.append(data);
    while(Buffer.size() >= (int)sizeof(PDU)){
        PDU* pdu= (PDU*)(Buffer.data());
        if(Buffer.size() < int(pdu->uiPDULen)){
            break;
        }
    //对接受到的数据类型进行判断 然后根据数据类型执行相应的功能
        PDUhandle(pdu);
    Buffer.remove(0,pdu->uiPDULen);
}
}

void Client::sendPdu(PDU *pdu)
{
    m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

QString& Client::get_strLoginName()
{
    return strLoginName;
}

QString &Client::get_strVer()
{
    return strVer;
}

void Client::PDUhandle(PDU*pdu)
{

    //对接受到的数据类型进行判断 然后根据数据类型执行相应的功能
    switch(pdu->uiMsgType){
    case ENUM_MSG_TYPE_REGIST_RESPEND:{//注册响应
        rh->res_regist(pdu);
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPEND:{//登录响应
        rh->res_login(pdu);
        break;
    }
    case ENUM_MSG_TYPE_FINDFRIEND_RESPEND:{//查找好友响应
        rh->res_findfriend(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ONLINEUSER_RESPEND:{//查找在线用户响应
        rh->res_is_online(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPEND:{
        rh->res_addfriend(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
        rh->res_addfriend_agree(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_AGREE_RESPEND:{
        rh->res_addfriend_agree_respend(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_UNAGREE_RESPEND:{
        rh->res_addfriend_unagree_respend(pdu);
        break;
    }
    case ENUM_MSG_TYPE_FLUST_FRIEND_ONLINE_RESPEND:{
        rh->res_flush_friend_online(pdu);
        break;
    }
    case ENUM_MSG_TYPE_LOGOFF_RESPEND:{
        rh->res_logoff(pdu);
        break;
    }
    case NUM_MSG_TYPE_DEL_ONLINE_FRIEND_RESPEND:{
        rh->res_del_Online_Friend(pdu);
        break;
    }
    case ENUM_MSG_TYPE_CHAT_FRIEND_REQUEST:
        rh->chat(pdu);
        break;
    case NUM_MSG_TYPE_MKDIR_RESPEND://创建文件夹响应
        rh->mkdir(pdu);
        break;
    case ENUM_MSG_TYPE_FLUSH_FILE_RESPEND://刷新文件响应
        rh->flushFile(pdu);
        break;
    case ENUM_MSG_TYPE_DEL_DIR_RESPEND://删除文件夹响应
        rh->deldir(pdu);
        break;
    case ENUM_MSG_TYPE_DEL_FILE_RESPEND:
        rh->delfile(pdu);
        break;
    case ENUM_MSG_TYPE_RENAME_RESPEND:
        rh->rename(pdu);
        break;
    case ENUM_MSG_TYPE_MVFILE_RESPEND://移动文件响应
        rh->mvfile(pdu);
        break;
    case ENUM_MSG_TYPE_UPLOAD_RESPEND://上传文件响应
        rh->uploadfile(pdu);
        break;
    case ENUM_MSG_TYPE_UPLOAD_DATA_RESPEND://上传文件数据响应
        rh->uploaddata(pdu);
        break;
    case ENUM_MSG_TYPE_SHARE_FILE_RESPEND://上传文件数据响应
        rh->sharefile(pdu);
        break;
    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST://上传文件数据响应
        rh->sharefilerequest(pdu);
        break;
    case ENUM_MSG_TYPE_SHARE_FILE_AGREE_RESPEND://上传文件数据响应
        rh->sharefileagree(pdu);
        break;
    case ENUM_MSG_TYPE_LOAD_FILE_RESPEND://下载文件响应
        rh->loadfile(pdu);
        break;
    case ENUM_MSG_TYPE_LOAD_FILE_AGREE_RESPEND://下载文件数据响应
        rh->loadFilData(pdu);
        break;
    default:
        break;
    }
//    free(pdu);
//    pdu=NULL;
    return;
}

QTcpSocket &Client::get_QTcpSocket()
{
    return m_tcpSocket;
}

Client::~Client()
{
    delete ui;
}


void Client::on_Regist_clicked()
{
        QString Name = ui->Name_Line->text();
        QString Pwd  = ui->Pwd_Line->text();
        if(Name.isEmpty() || Pwd.isEmpty() || Name.toStdString().size()>32 || Name.toStdString().size()>32){
            QMessageBox::information(this,"注册","用户名或密码不合法!");
            return;
        }
        PDU* pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_REQUEST;
        memcpy(pdu->uiData,Name.toStdString().c_str(),32);
        memcpy(pdu->uiData+32,Pwd.toStdString().c_str(),32);
        Client::getinstance().sendPdu(pdu);
        return;
}

void Client::on_Login_clicked()
{
        QString Name = ui->Name_Line->text();
        QString Pwd  = ui->Pwd_Line->text();
        if(Name.isEmpty() || Pwd.isEmpty() || Name.toStdString().size()>32 || Name.toStdString().size()>32){
            QMessageBox::information(this,"登录","用户名或密码不合法!");
            return;
        }
        PDU* pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_REQUEST;
        memcpy(pdu->uiData,Name.toStdString().c_str(),32);
        memcpy(pdu->uiData+32,Pwd.toStdString().c_str(),32);
        Client::getinstance().sendPdu(pdu);
        return;
}

void Client::on_logoff_clicked()//注销按钮
{
        QString Name = ui->Name_Line->text();
        QString Pwd  = ui->Pwd_Line->text();


        if(Name.isEmpty() || Pwd.isEmpty() || Name.toStdString().size()>32 || Name.toStdString().size()>32){
            QMessageBox::information(this,"注销","用户名或密码不合法!");
            return;
        }
        PDU* pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_LOGOFF_REQUEST;
        memcpy(pdu->uiData,Name.toStdString().c_str(),32);
        memcpy(pdu->uiData+32,Pwd.toStdString().c_str(),32);
        Client::getinstance().sendPdu(pdu);
        return;
    }

QString Client::get_RootPath()
{
    return RootPath;
}



