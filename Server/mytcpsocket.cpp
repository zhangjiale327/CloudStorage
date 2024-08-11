#include "mytcpsocket.h"
#include "protocol.h"
#include "operatedb.h"
#include<QDebug>
MyTcpSocket::MyTcpSocket()
{
    //建立信号连接 当客户端有消息发送时 服务器会触发这个信号 然后触发recvMsg接受消息
    QObject::connect(this,&QTcpSocket::readyRead,this,&MyTcpSocket::recvMsg);
    QObject::connect(this,&QTcpSocket::disconnected,this,&MyTcpSocket::clientOffline);
    mh=new MsgHandle();
}

MyTcpSocket::~MyTcpSocket()
{
    delete mh;
}



//PDU *MyTcpSocket::readPDU()
//{
//    uint uiPDULen=0;//作为传出参数记得取地址 获取客户端发来消息的总长度 即PDU协议的第一个参数
//    this->read((char*)&uiPDULen,sizeof(uint));//第一个参数 如果是指针类型的话就不需要&了
//    qDebug()<<"协议总长度为:"<<uiPDULen;
//    //获取实际消息长度
//    uint uiMsgLen=uiPDULen-sizeof(PDU);

//    PDU*pdu=mkPDU(uiMsgLen);
//    //读取PDU中除了消息长度以外的其他内容 需要进行偏移 因为我们总协议长度这个变量已经读取过了
//    this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
//    qDebug()<<"接收到的客户端信息:"
//           <<"消息长度:"<<pdu->uiMsgLen
//          <<"消息类型:"<<pdu->uiMsgType
//         <<"消息内容1:"<<pdu->uiData
//        <<"消息内容2:"<<(pdu->uiData)+32;
//    return pdu;
//}

void MyTcpSocket::sendPDU(PDU *respdu)
{
    if(respdu==NULL){
        qDebug()<<"repdu=NULL";
        return;
    }
    qDebug()<<"协议总长度: "<<respdu->uiPDULen;
    this->write((char*)respdu,respdu->uiPDULen);//将处理后的结果发送给客户端
    qDebug()<<"给客户端发送 响应消息 成功!";
    free(respdu);
    respdu=NULL;
}

PDU *MyTcpSocket::PDUhandle(PDU *pdu)
{
    switch(pdu->uiMsgType){
    case ENUM_MSG_TYPE_REGIST_REQUEST://注册请求
        return mh->regist(pdu);
    case ENUM_MSG_TYPE_LOGOFF_REQUEST://注销请求
        return mh->logoff(pdu);
    case ENUM_MSG_TYPE_LOGIN_REQUEST://登录请求
        return mh->login(pdu,strLoginName);
    case ENUM_MSG_TYPE_FINDFRIEND_REQUEST://查找好友请求
        return mh->findfriend(pdu);
    case ENUM_MSG_TYPE_ONLINEUSER_REQUEST://在线好友
        return mh->is_online(pdu);
     case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST://添加好友请求
        return mh->addFriend(pdu);
     case ENUM_MSG_TYPE_ADD_AGREE_REQUEST://同意好友添加请求
        return mh->agreeFriend(pdu);
     case ENUM_MSG_TYPE_ADD_UNAGREE_REQUEST://不同意好友添加请求
        return mh->unagreeFriend(pdu);
      case ENUM_MSG_TYPE_FLUSH_FRIEND_ONLINE_REQUEST://刷新好友请求
        return mh->flushOnlineFriend(pdu);
      case ENUM_MSG_TYPE_DEL_ONLINE_FRIEND_REQUEST://删除在线好友请求
        return mh->del_online_friend(pdu);
        case ENUM_MSG_TYPE_CHAT_FRIEND_REQUEST://好友聊天
               mh->Chat(pdu);
                break;
        case ENUM_MSG_TYPE_MKDIR_REQUEST://创建文件请求
      return mh->mkdir(pdu);

    case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST://刷新文件请求
            return mh->flushFile(pdu);
    case ENUM_MSG_TYPE_DEL_DIR_REQUEST://删除文件夹请求
            return mh->del_dir(pdu);
    case ENUM_MSG_TYPE_DEL_FILE_REQUEST://删除文件请求
            return mh->del_file(pdu);
    case ENUM_MSG_TYPE_RENAME_REQUEST://重命名文件请求
            return mh->rename(pdu);
    case ENUM_MSG_TYPE_MVFILE_REQUEST://移动文件请求
            return mh->mvfile(pdu);
    case ENUM_MSG_TYPE_UPLOAD_REQUEST:
        return mh->upLoadfile(pdu);

    case ENUM_MSG_TYPE_UPLOAD_DATA_REQUEST:
        return mh->uploaddata(pdu);
    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:
        return mh->shareFile(pdu);
    case ENUM_MSG_TYPE_SHARE_FILE_AGREE_REQUEST:
        return mh->shareAgreeFile(pdu);
    case ENUM_MSG_TYPE_LOAD_FILE_REQUEST:
        return mh->loadFile(pdu);
    case ENUM_MSG_TYPE_LOAD_FILE_AGREE_REQUEST:
        return mh->loadFileData(this);
    default:
        break;
    }
    return NULL;
}
void MyTcpSocket::recvMsg()
{
    QByteArray data = this->readAll();
    Buffer.append(data);
    while(Buffer.size() >= (int)sizeof(PDU)){
        PDU* pdu= (PDU*)(Buffer.data());
        if(Buffer.size() < int(pdu->uiPDULen)){
            qDebug()<<"半包";
            break;
        }
    //对接受到的数据类型进行判断 然后根据数据类型执行相应的功能
    PDU*respdu=PDUhandle(pdu);
    sendPDU(respdu);
    Buffer.remove(0,pdu->uiPDULen);
}}

void MyTcpSocket::clientOffline()//与客户端断开连接后 服务器要做的事情
{
    OperateDB::getinstance().handleoffline(strLoginName.toStdString().c_str());//先执行数据库 将客户端状态设置为0
    emit offline(this);//发送信号 通知server类 让其将该socket从socket队列中删除
    return;
}
