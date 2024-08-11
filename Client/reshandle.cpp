#include "index.h"
#include "reshandle.h"
#include<QDebug>
#include<QMessageBox>
#include"client.h"
#include "friend.h"
ResHandle::ResHandle(QWidget *parent) : QWidget(parent)
{
}

void ResHandle::res_regist(PDU *pdu)
{
    bool ret;
    memcpy(&ret,pdu->uiData,sizeof(bool));
    //qDebug()<<"接受的服务器响应消息:";
    //qDebug()<<"ret="<<ret;
    if(ret){
        QMessageBox::information(&Client::getinstance(),"注册","注册成功!");
     }else{
        QMessageBox::information(&Client::getinstance(),"注册","账号已存在!");
    }
    return;
}



void ResHandle::res_login (PDU* pdu)
{
    bool ret;
    char Name[32] = {'\0'};     //用于接受服务器端响应登录成功的用户名 然后再给到成员变量strLoginName
    memcpy(&ret,pdu->uiData,sizeof(bool));
    memcpy(Name,pdu->uiData+32,32);
    Client::getinstance().get_strLoginName() = QString(Name);
    qDebug()<<"get_strLoginName() = "<<Client::getinstance().get_strLoginName();

    qDebug()<<"ret = "<<ret;
    if (ret){
            qDebug()<<"登录成功的用户名:"<<Name;
            Index::getinstance().setWindowTitle(Client::getinstance().get_strLoginName());
            Index::getinstance().show();
            Index::getinstance().get_friend()->flushFriendOline(Client::getinstance().get_strLoginName());
            Client::getinstance().hide();
        }else{
        QMessageBox::information(this,"登录","账号或密码不合法!");
    }
    return;
}

void ResHandle::res_findfriend(PDU *pdu)
{
    int ret;
    char Name[32]={'\0'};
    memcpy(&ret,pdu->uiData,sizeof(int));
    memcpy(Name,pdu->uiData+32,32);
    qDebug()<<"ret="<<ret;
    if(ret==-1){
        QMessageBox::information(&Client::getinstance(),"查找",QString("'%1'不存在").arg(Name));
     }else if(ret==0){
        QMessageBox::information(&Client::getinstance(),"查找",QString("'%1'不在线").arg(Name));
     }else if(ret==1){
        int ret=QMessageBox::information(&Client::getinstance(),"查找",QString("'%1'在线").arg(Name),"添加好友","取消");
        if(ret!=0){
            return;
        }
      PDU*pdu=mkPDU(0);
      pdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
      memcpy(pdu->uiData,Client::getinstance().get_strLoginName().toStdString().c_str(),32);
      memcpy(pdu->uiData+32,Name,32);
      Client::getinstance().sendPdu(pdu);
    }
    return;
}

void ResHandle::res_is_online(PDU *pdu)
{
    char temp[32]={'\0'};
    QStringList onelineUser;
    int userCount=pdu->uiMsgLen/32;
    for(int i=0;i<userCount;i++){
        memcpy(temp,pdu->caMsg+i*32,32);
        if(temp!=Client::getinstance().get_strLoginName()){
            onelineUser.append(QString(temp));
        }
        memset(temp,'\0',32);
    }
    Index::getinstance().get_friend()->getOnlineUser()->updateOnlineWidgetList(onelineUser);
    return;
}

void ResHandle::res_addfriend(PDU *pdu)
{
    int ret;
    char strTarName[32]={'\0'};
    memcpy(&ret,pdu->uiData,sizeof(int));
    qDebug()<<"接受到的服务器 ret="<<ret;
    memcpy(strTarName,pdu->uiData+32,32);
    if(ret==2){
        QMessageBox::information(&Index::getinstance(),"添加好友",QString("'%1'已经是好友关系").arg(strTarName));
    }else if(ret==-1){
        QMessageBox::information(&Index::getinstance(),"添加好友","服务器内部错误");
    }else if(ret==0){
        QMessageBox::information(&Index::getinstance(),"添加好友",QString("'%1'不在线").arg(strTarName));
    }
    return;
}

void ResHandle::res_addfriend_agree(PDU *pdu)
{
    char strCurName[32]={'\0'};
    memcpy(strCurName,pdu->uiData,32);
    qDebug()<<"申请添加好友的名字为:"<<strCurName;
    int ret=QMessageBox::question(&Index::getinstance(),"申请添加好友",QString("'%1'申请添加好友").arg(strCurName));
    PDU*respdu=mkPDU(0);
    if(ret!=QMessageBox::Yes){
        respdu->uiMsgType=ENUM_MSG_TYPE_ADD_UNAGREE_REQUEST;
        memcpy(respdu->uiData,pdu->uiData,64);
        Client::getinstance().sendPdu(respdu);
        return;
    }
    respdu->uiMsgType=ENUM_MSG_TYPE_ADD_AGREE_REQUEST;
    memcpy(respdu->uiData,pdu->uiData,64);
    Client::getinstance().sendPdu(respdu);
    return;
}

void ResHandle::res_addfriend_agree_respend(PDU *pdu)
{
    char strCurName[32]={'\0'};
    char strTarName[32]={'\0'};
    memcpy(strCurName,pdu->uiData,32);
    memcpy(strTarName,pdu->uiData+32,32);
    QMessageBox::information(&Index::getinstance(),"添加好友",QString("添加好友'%1'成功").arg(strTarName));
    Index::getinstance().get_friend()->flushFriendOline(strCurName);
    return;
}

void ResHandle::res_addfriend_unagree_respend(PDU *pdu)
{
    char strTarName[32]={'\0'};
    memcpy(strTarName,pdu->uiData+32,32);
    QMessageBox::information(&Index::getinstance(),"添加好友",QString("'%1'拒绝添加好友").arg(strTarName));
    return;
}

void ResHandle::res_flush_friend_online(PDU *pdu)
{
    char temp[32]={'\0'};
    QStringList strFriend;
    int userCount=pdu->uiMsgLen/32;
    for(int i=0;i<userCount;i++){
        memcpy(temp,pdu->caMsg+i*32,32);
            strFriend.append(QString(temp));
        memset(temp,'\0',32);
    }
    Index::getinstance().get_friend()->updateFriendLw(strFriend);
    return;
}

void ResHandle::res_logoff(PDU *pdu)
{
    bool ret;
    char strCurName[32]={'\0'};
    memcpy(&ret,pdu->uiData,sizeof(bool));
    memcpy(strCurName,pdu->uiData+32,32);
    qDebug()<<"res logoff ret="<<ret;
    if(!ret){
        QMessageBox::information(&Index::getinstance(),"注销用户","该用户不存在 注销失败");
    }else{
        QMessageBox::information(&Index::getinstance(),"注销用户","注销成功");
    }
    return;
}

void ResHandle::res_del_Online_Friend(PDU *pdu)
{
    bool ret;
    char strDelFriend[32]={'\0'};
    memcpy(&ret,pdu->uiData,sizeof(bool));
    memcpy(strDelFriend,pdu->uiData+32,32);
    if(ret==false){
         QMessageBox::information(&Index::getinstance(),"删除用户","删除失败");
    }
    else{
        QMessageBox::information(&Index::getinstance(),"删除用户",QString("删除好友'%1'成功!").arg(strDelFriend));
    }
    Index::getinstance().get_friend()->flushFriendOline(Client::getinstance().get_strLoginName());
    return;
}

void ResHandle::chat(PDU *pdu)
{

    Chat*c = Index::getinstance().get_friend()->get_m_chat();
    if(c->isHidden()){
        c->show();
    }
    char strCharName[32]={'\0'};
    qDebug()<<"接受到的服务器好友chatName"<<strCharName;
    memcpy(strCharName,pdu->uiData,32);
    QString Msg = QString("%1: \n   %2\n").arg(strCharName).arg(pdu->caMsg);
    c->m_strCharName=strCharName;
    c->updateShow_LW(Msg);
    return;
}

void ResHandle::mkdir(PDU *pdu)
{
    bool flag;
    char dirName[32]={'\0'};
    memcpy(&flag,pdu->uiData,sizeof(bool));
    qDebug()<<"mkdir flag= "<<flag;
    memcpy(dirName,pdu->uiData+32,32);
    if(flag){
        QMessageBox::information(this,"创建文件夹",QString("创建文件夹 <<%1>> 成功!").arg(dirName));
        Index::getinstance().get_file()->flustFile();
    }else{
        QMessageBox::information(this,"创建文件夹",QString("创建文件夹 <<%1>> 失败!").arg(dirName));
    }
    return;
}

void ResHandle::flushFile(PDU *pdu)
{
    int Pcount = pdu->uiMsgLen/sizeof(FileInfo);
    QList<FileInfo *> PfILElIST;

    for(int i=0;i<Pcount;i++){
        FileInfo* file = new FileInfo;
        memcpy(file,pdu->caMsg+i*sizeof(FileInfo),sizeof(FileInfo));
        PfILElIST.append(file);
    }//利用服务器传过来的文件属性结构体 构建一个文件属性list
    Index::getinstance().get_file()->updateListWidget(PfILElIST);
    return;
}

void ResHandle::deldir(PDU *pdu)
{
    bool ret;
    memcpy(&ret,pdu->uiData,sizeof(bool));

    qDebug()<<"deldir ret="<<ret;
    if(ret){
        QMessageBox::information(this,"删除文件夹","删除成功！");
        Index::getinstance().get_file()->flustFile();
    }else{
        QMessageBox::information(this,"删除文件夹","删除失败!");
    }
    return;
}
void ResHandle::delfile(PDU *pdu)
{
    bool ret;
    memcpy(&ret,pdu->uiData,sizeof(bool));

    qDebug()<<"delfile ret="<<ret;
    if(ret){
        QMessageBox::information(this,"删除文件","删除成功！");
        Index::getinstance().get_file()->flustFile();
    }else{
        QMessageBox::information(this,"删除文件","删除失败!");
    }
    return;
}

void ResHandle::rename(PDU *pdu)
{
    bool ret;
    memcpy(&ret,pdu->uiData,sizeof(bool));

    qDebug()<<"rename ret="<<ret;
    if(ret){
        QMessageBox::information(this,"重名名文件","修改名字成功！");
        Index::getinstance().get_file()->flustFile();
    }else{
        QMessageBox::information(this,"重命名文件","修改名字失败!");
    }
    return;
}

void ResHandle::mvfile(PDU *pdu)
{
    bool ret;
    memcpy(&ret,pdu->uiData,sizeof(bool));

    qDebug()<<"mvfile ret="<<ret;
    if(ret){
        QMessageBox::information(this,"移动文件","移动文件成功！");
        Index::getinstance().get_file()->flustFile();
    }else{
        QMessageBox::information(this,"移动文件","移动文件失败!");
    }
    return;
}

void ResHandle::uploadfile(PDU *pdu)
{
    int ret;
    memcpy(&ret,pdu->uiData,sizeof(int));
    qDebug()<<"uploadfile ret="<<ret;
    if(ret==0){
        QMessageBox::information(&Index::getinstance(),"上传文件","正在上传文件!");
    }else if(ret==-1){
        QMessageBox::information(&Index::getinstance(),"上传文件","打开文件失败");
    }else if(ret==1){
        Index::getinstance().get_file()->uploaddata();
    }
    return;
}

void ResHandle::uploaddata(PDU *pdu)
{
    bool ret;
    memcpy(&ret,pdu->uiData,sizeof(bool));
    qDebug()<<"uploaddata ret="<<ret;
    if(ret){
        QMessageBox::information(&Index::getinstance(),"上传文件","上传文件成功!");
        Index::getinstance().get_file()->flustFile();
    }else {
         QMessageBox::information(&Index::getinstance(),"上传文件","上传文件失败");
    }
    Index::getinstance().get_file()->upLoadaStat = false;
    return;
}

void ResHandle::sharefile(PDU *pdu)
{
    QMessageBox::information(Index::getinstance().get_file(),"分享文件","文件已分享!");
}

void ResHandle::sharefilerequest(PDU *pdu)
{
    QString strSharedPath = pdu->caMsg;
    int index = strSharedPath.lastIndexOf('/');
    QString strFileName = strSharedPath.right(strSharedPath.size()-index-1);
    int ret = QMessageBox::question(
                Index::getinstance().get_file()->sharedfile
                ,"分享文件",
                QString("%1 分享文件 %2 \n是否接受？").arg(pdu->uiData).arg(strFileName));
    if(ret!=QMessageBox::Yes){
        return;
    }
    PDU* respdu =mkPDU(pdu->uiMsgLen);
    respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_AGREE_REQUEST;

    memcpy(respdu->uiData,Client::getinstance().get_strLoginName().toStdString().c_str(),32);

    memcpy(respdu->caMsg,pdu->caMsg,pdu->uiMsgLen);
    Client::getinstance().sendPdu(respdu);
}

void ResHandle::sharefileagree(PDU *pdu)
{
    bool ret;
    memcpy(&ret,pdu->uiData,sizeof(bool));
    if(ret){
        QMessageBox::information(Index::getinstance().get_file(),"分享文件","分享文件成功!");
        Index::getinstance().get_file()->flustFile();
    }else{
        QMessageBox::information(Index::getinstance().get_file(),"分享文件","分享文件失败!");
    }
}

void ResHandle::loadfile(PDU *pdu)
{
    int ret;
    memcpy(&ret,pdu->uiData,sizeof(int));

    if(ret == 0){
        QMessageBox::information(&Client::getinstance(),"下载文件",
                                                        "已有文件正在下载");
        return;
    }
    if(ret ==-1){
        QMessageBox::information(&Client::getinstance(),"下载文件",
                                                        "打开文件失败!");
        return;
    }
    qint64 loadFileSize;
    memcpy(&loadFileSize,pdu->uiData+sizeof(int),sizeof(int));
    Index::getinstance().get_file()->rsloadFile(loadFileSize);
}

void ResHandle::loadFilData(PDU *pdu)
{
    Index::getinstance().get_file()->loadFileData(pdu->caMsg,pdu->uiMsgLen);
}





















