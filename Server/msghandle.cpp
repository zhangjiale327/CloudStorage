#include "msghandle.h"
#include "operatedb.h"
#include "server.h"
#include<stdlib.h>
#include<QDebug>
#include <qsqlquery.h>
#include <QDir>
#include "mytcpserver.h"
#include"mytcpsocket.h"
#include "operatedb.h"
MsgHandle::MsgHandle()
{
   // mt=new MyTcpSocket;
    upLoad = false;
    LoadFile = false;
}

MsgHandle::~MsgHandle()
{
    //delete mt;
}



PDU *MsgHandle::regist(PDU *pdu)
{
    char caName[32]={'\0'};
    char caPwd[32]={'\0'};
    memcpy(caName,pdu->uiData,32);
    memcpy(caPwd,pdu->uiData+32,32);//获取输入的用户名和密码
    qDebug()<<"caName："<<caName;
    qDebug()<<"caPwd："<<caPwd;//接受注册消息 用户名和密码
    bool ret=OperateDB::getinstance().handleRegist(caName,caPwd);//操作数据库 根据数据库返回值判断是否注册成功
    qDebug()<<"register ret="<<ret;
    if(ret){
        QDir file;
        QString filePath=QString("%1/%2").arg(Server::getinstance().get_RootPath()).arg(caName);
        file.mkdir(filePath);
        qDebug()<<"创建文件夹路径: "<<filePath;
        //注册成功后 在服务器bulid目录下filesys目录下创建一个 以用户名命名的文件
    }
    PDU*respdu=mkPDU(0);
    respdu->uiMsgType=ENUM_MSG_TYPE_REGIST_RESPEND;

    memcpy((char*)respdu->uiData,&ret,sizeof(bool));
    return respdu;
}



PDU*MsgHandle::login(PDU *pdu,QString &name)
{
    char caName[32]={'\0'};
    char caPwd[32]={'\0'};
    memcpy(caName,pdu->uiData,32);
    memcpy(caPwd,pdu->uiData+32,32);//获取输入的用户名和密码
    qDebug()<<"login caName："<<caName;
    qDebug()<<"login caPwd："<<caPwd;//接受登录消息 用户名和密码
    name=caName;
    bool ret=OperateDB::getinstance().handleLogin(caName,caPwd);//操作数据库 根据数据库返回值判断是否注册成功
    qDebug()<<"login ret="<<ret;
    qDebug()<<"strLoginName="<<name;

    PDU*respdu=mkPDU(0);
    respdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_RESPEND;

    memcpy(respdu->uiData,&ret,sizeof(bool));
    memcpy(respdu->uiData+32,name.toStdString().c_str(),32);//将登录成功的用户名返回给客户端
    return respdu;
}



PDU *MsgHandle::findfriend(PDU *pdu)
{
    char caName[32]={'\0'};
    memcpy(caName,pdu->uiData,32);
    qDebug()<<"findfriend caName："<<caName;
    int ret=OperateDB::getinstance().handleFindFriend(caName);//操作数据库 根据数据库返回值判断是否注册成功
    qDebug()<<"findfriend ret="<<ret;

    PDU*respdu=mkPDU(0);
    respdu->uiMsgType=ENUM_MSG_TYPE_FINDFRIEND_RESPEND;

    memcpy((char*)respdu->uiData,&ret,sizeof(int));
    memcpy((char*)respdu->uiData+32,caName,32);
    return respdu;
}



PDU *MsgHandle::is_online(PDU *pdu)
{
    QStringList onlineList=OperateDB::getinstance().handleOnlineUser();//操作数据库 根据数据库返回值判断是否注册成功
    uint userCount=onlineList.size();
    PDU*respdu=mkPDU(userCount*32);
    respdu->uiMsgType=ENUM_MSG_TYPE_ONLINEUSER_RESPEND;//查看在线用户响应
    for(uint i=0;i<userCount;i++){
        memcpy((char*)respdu->caMsg+i*32,onlineList.at(i).toStdString().c_str(),32);
        qDebug()<<"在线用户为:"<<onlineList.at(i);
    }
    return respdu;
}

PDU *MsgHandle::addFriend(PDU *pdu)
{
    char strCurName[32]={'\0'};
    char strTarName[32]={'\0'};
    memcpy(strCurName,pdu->uiData,32);
    memcpy(strTarName,pdu->uiData+32,32);
    qDebug()<<"查找好友请求: 当前用户名"<<strCurName
           <<"目标用户名:"<<strTarName;
    int ret=OperateDB::getinstance().handleAddFriend(strCurName,strTarName);
    if(ret==1){
        MyTcpServer::getinstance().resend(strTarName,pdu);
        qDebug()<<"当前好友 "<<strTarName<<"在线"
                <<"申请添加好友同意请求已发送";
    }
    qDebug()<<"ret="<<ret;
    PDU* respdu=mkPDU(0);
    respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPEND;
    memcpy((char*)respdu->uiData,&ret,sizeof(int));
    memcpy((char*)respdu->uiData+32,strTarName,32);
    return respdu;
}

PDU *MsgHandle::agreeFriend(PDU *pdu)
{
    char strCurName[32]={'\0'};
    char strTarName[32]={'\0'};
    memcpy(strCurName,pdu->uiData,32);
    memcpy(strTarName,pdu->uiData+32,32);
    OperateDB::getinstance().handleAgreeAddFriend(strCurName,strTarName);
    PDU* respdu=mkPDU(0);
    respdu->uiMsgType=ENUM_MSG_TYPE_ADD_AGREE_RESPEND;
    memcpy(respdu->uiData,pdu->uiData,64);
    MyTcpServer::getinstance().resend(strCurName,respdu);

    memcpy(respdu->uiData,strTarName,32);
    memcpy(respdu->uiData+32,strCurName,32);
    return respdu;
}

PDU *MsgHandle::unagreeFriend(PDU *pdu)
{
    char strCurName[32]={'\0'};
    char strTarName[32]={'\0'};
    memcpy(strCurName,pdu->uiData,32);
    memcpy(strTarName,pdu->uiData+32,32);
    PDU* respdu=mkPDU(0);
    respdu->uiMsgType=ENUM_MSG_TYPE_ADD_UNAGREE_RESPEND;
    memcpy(respdu->uiData,pdu->uiData,64);
    MyTcpServer::getinstance().resend(strCurName,respdu);
    return NULL;
}

PDU *MsgHandle::flushOnlineFriend(PDU *pdu)
{
    char curName[32]={'\0'};
    memcpy(curName,pdu->uiData,32);
    QStringList strList=OperateDB::getinstance().handleflushOnlineFriend(curName);
    qDebug()<<"flush friend "<<curName;
    uint userCount=strList.size();
    PDU*respdu=mkPDU(userCount*32);
    respdu->uiMsgType=ENUM_MSG_TYPE_FLUST_FRIEND_ONLINE_RESPEND;
    for(uint i=0;i<userCount;i++){
        memcpy((char*)respdu->caMsg+i*32,strList.at(i).toStdString().c_str(),32);
    }
    return respdu;
}

PDU *MsgHandle::logoff(PDU *pdu)
{
    char curName[32]= {'\0'};
    char curPwd[32] = {'\0'};
    memcpy(curName,pdu->uiData,32);
    memcpy(curPwd,pdu->uiData+32,32);
    bool ret=OperateDB::getinstance().handleLogoff(curName,curPwd);
    qDebug()<<"logoff ret="<<ret;
    PDU* respdu=mkPDU(0);
    respdu->uiMsgType=ENUM_MSG_TYPE_LOGOFF_RESPEND;
    memcpy((char*)respdu->uiData,&ret,sizeof(bool));
    memcpy((char*)respdu->uiData+32,curName,32);
    return respdu;
}

PDU *MsgHandle::del_online_friend(PDU *pdu)
{
    char strCurName[32]={'\0'};
    char delName[32]={'\0'};
    memcpy(strCurName,pdu->uiData,32);
    memcpy(delName,pdu->uiData+32,32);
    qDebug()<<"del_online_friend-> strCurName:"<<strCurName<<"delName"<<delName;
    bool ret=OperateDB::getinstance().handleDelOnlineFriend(strCurName,delName);
    qDebug()<<"del online friend ret="<<ret;
    PDU* respdu=mkPDU(0);
    respdu->uiMsgType=NUM_MSG_TYPE_DEL_ONLINE_FRIEND_RESPEND;
    memcpy((char*)respdu->uiData,&ret,sizeof(bool));
    memcpy((char*)respdu->uiData+32,delName,32);
    return respdu;
}


void MsgHandle::Chat(PDU *pdu)
{
    char strTarName[32]={'\0'};
    memcpy(strTarName,pdu->uiData+32,32);
    qDebug()<<"res Chat strTarName:"<<strTarName;
    MyTcpServer::getinstance().resend(strTarName,pdu);
}

PDU *MsgHandle::mkdir(PDU *pdu)
{
    char dirName[32] = {'\0'};
    memcpy(dirName,pdu->uiData,64);
    qDebug()<<"mkdir Name: "<<dirName;
    PDU* respdu = mkPDU(0);
    bool flag = false;
    respdu->uiMsgType = NUM_MSG_TYPE_MKDIR_RESPEND;
    memcpy(respdu->uiData+32,dirName,32);
    QString dirPath = pdu->caMsg;
    QDir dir;
    if(!dir.exists(dirPath)){
        memcpy(respdu->uiData,&flag,sizeof(bool));
        qDebug()<<"mkdir flag=： "<<flag;
        return respdu;
    }

    dirPath=QString("%1/%2").arg(dirPath).arg(dirName);

    if(dir.exists(dirPath) || !dir.mkdir(dirPath)){
        memcpy(respdu->uiData,&flag,sizeof(bool));
        qDebug()<<"mkdir flag=： "<<flag;
        return respdu;
    }
    flag=true;
    memcpy(respdu->uiData,&flag,sizeof(bool));
    qDebug()<<"mkdir flag=： "<<flag;
    return respdu;
}

PDU *MsgHandle::flushFile(PDU *pdu)//刷新文件
{
    QString dirPath = pdu->caMsg;//获取刷新文件的路径


    qDebug()<<"flush_path:"<<dirPath;

    QDir dir(dirPath);//用要刷新的文件路径初始化 操作文件的对象

    QFileInfoList fileInfoList = dir.entryInfoList();//获取当前路径下的所有文件和目录

    int fileConunt = fileInfoList.size()-2;//当前路径下除了 . ..两个文件以外其他的文件和目录总个数

    PDU* respdu = mkPDU(fileConunt*sizeof(FileInfo));//创建恢复pdu
    respdu->uiMsgType= ENUM_MSG_TYPE_FLUSH_FILE_RESPEND;
    FileInfo* pFileInfo = NULL;//文件属性指针
    QString strFileName;
    for(int i=0,j=0;i<fileInfoList.size();i++){
        strFileName = fileInfoList[i].fileName();
        if(strFileName == QString(".") || strFileName == QString("..")){
            continue;
        }
        pFileInfo = (FileInfo*)(respdu->caMsg)+j++;
        //将pdu指针转换为文件属性结构体类型 并且赋值给结构体对象 每次偏移是按照结构体类型大小进行偏移
        memcpy(pFileInfo->FileName,strFileName.toStdString().c_str(),32);
        //将文件或目录名 写入到pdu中
        if(fileInfoList[i].isDir()){
            pFileInfo->FileType=0;
        }else if(fileInfoList[i].isFile()){
            pFileInfo->FileType = 1;
        }//判断是文件还是目录 并且将其放入到pdu中
        qDebug()<<"flush strFileName "<< strFileName<<
                  "FileType "<<pFileInfo->FileType;
    }

    return respdu;
}

PDU *MsgHandle::del_dir(PDU *pdu)
{
    QFileInfo fileinfo(pdu->caMsg);
    qDebug()<<"deldir path="<<pdu->caMsg;
    bool ret = false;
    if(fileinfo.isDir()){
        QDir  dir(pdu->caMsg);
        ret = dir.removeRecursively();
    }
    PDU* resPdu = mkPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPEND;
    memcpy(resPdu->uiData,&ret,sizeof(bool));
    qDebug()<<"deldir ret="<<ret;
    return resPdu;
}

PDU *MsgHandle::del_file(PDU *pdu)
{
    QFileInfo fileinfo(pdu->caMsg);
    qDebug()<<"delfile path="<<pdu->caMsg;
    bool ret = false;
    if(fileinfo.isFile()){
        QFile  file(pdu->caMsg);
        ret = file.remove();
    }
    PDU* resPdu = mkPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_RESPEND;
    memcpy(resPdu->uiData,&ret,sizeof(bool));
    qDebug()<<"delfile ret="<<ret;
    return resPdu;
}

PDU *MsgHandle::rename(PDU *pdu)
{
    QString Path = pdu->caMsg;
    qDebug()<<"rename path="<<Path;
    char strOldName [32] = {'\0'};
    char strNewName [32] = {'\0'};
    memcpy(strOldName,pdu->uiData,32);
    memcpy(strNewName,pdu->uiData+32,32);

    qDebug()<<"rename strOldName="<<strOldName
           <<"strNewName="<<strNewName;
    QDir dir(Path);
    bool ret = false;
    PDU* resPdu = mkPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_RENAME_RESPEND;
    if(dir.exists(strNewName)){
         memcpy(resPdu->uiData,&ret,sizeof(bool));
         qDebug()<<"rename ret="<<ret;
         return resPdu;
    }

    ret = dir.rename(strOldName,strNewName);
    memcpy(resPdu->uiData,&ret,sizeof(bool));
    qDebug()<<"rename ret="<<ret;
    return resPdu;
}

PDU *MsgHandle::mvfile(PDU *pdu)
{
    int mv_len;
    int mv_tar_len;
    memcpy(&mv_len,pdu->uiData,sizeof(int));
    memcpy(&mv_tar_len,pdu->uiData+sizeof(int),sizeof(int));
    char* mv_path = new char[mv_len+1];
    char* mv_tar_path = new char[mv_tar_len+1];

    memset(mv_path,'\0',mv_len);
    memset(mv_tar_path,'\0',mv_tar_len);

    memcpy(mv_path,pdu->caMsg,mv_len);
    memcpy(mv_tar_path,pdu->caMsg+mv_len,mv_tar_len);

    qDebug()<<"mv_len"<<mv_len
           <<"mv_tar_len"<<mv_tar_len
          <<"mv_path"<<mv_path
         <<"mv_tar_path"<<mv_tar_path;
    bool ret = QFile::rename(mv_path,mv_tar_path);
    qDebug()<<"mvfile ret="<<ret;
    PDU* respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_MVFILE_RESPEND;
    memcpy(respdu->uiData,&ret,sizeof(bool));

    delete[] mv_path;
    delete[] mv_tar_path;
    mv_path = NULL;
    mv_tar_path = NULL;

    return respdu;
}

PDU *MsgHandle::upLoadfile(PDU *pdu)
{
    PDU* respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_RESPEND;
    int ret = 1;
    //ret=0 文件正在上传 -1代表文件打开失败 1成功
    if(upLoad){
        qDebug()<<"文件正在上传中!";
        ret = 0;
        memcpy(pdu->uiData,&ret,sizeof(qint64));
        return respdu;
    }

    char upLoadName[32] = {'\0'};
    QString upLoadPath = pdu->caMsg;
    qint64 fileSize;
    memcpy(upLoadName,pdu->uiData,32);
    memcpy(&fileSize,pdu->uiData+32,sizeof(qint64));

    upLoadPath = upLoadPath+"/"+upLoadName;
    m_upLoadFile.setFileName(upLoadPath);
    if(m_upLoadFile.open(QIODevice::WriteOnly)){
        qDebug()<<"open file succes";
        upLoad = true;
        m_upLoadTotal = fileSize;
        m_upLoadreserve = 0;
    }else{
        ret = -1;
        qDebug()<<"open file falied";
    }
    memcpy(respdu->uiData,&ret,sizeof(qint64));
    return respdu;
}

PDU *MsgHandle::uploaddata(PDU *pdu)
{
    m_upLoadFile.write(pdu->caMsg,pdu->uiMsgLen);
    m_upLoadreserve += pdu->uiMsgLen;
    if(m_upLoadreserve < m_upLoadTotal){
        return NULL;
    }

    m_upLoadFile.close();
    upLoad =false;
    PDU* respdu = mkPDU(0);
    respdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_DATA_RESPEND;
    bool ret = m_upLoadreserve==m_upLoadTotal;
    qDebug()<<"uploaddata ret= "<<ret;
    memcpy(respdu->uiData,&ret,sizeof(bool));
    return respdu;
}

PDU *MsgHandle::shareFile(PDU *pdu)
{
    char caCurName[32] = {'\0'};
    int friend_count;
    memcpy(caCurName,pdu->uiData,32);
    memcpy(&friend_count,pdu->uiData+32,sizeof(int));

    int size = friend_count*32;

    PDU* respdu = mkPDU(pdu->uiMsgLen - size);
    respdu->uiMsgType =pdu->uiMsgType;

    memcpy(respdu->uiData,caCurName,32);
    memcpy(respdu->caMsg,pdu->caMsg+size,pdu->uiMsgLen-size);

    char caRecvName[32] = {'\0'};
    for(int i=0;i<friend_count;i++){
        memcpy(caRecvName,pdu->caMsg+i*32,32);
        MyTcpServer::getinstance().resend(caRecvName,respdu);
    }
    free(respdu);
    respdu = NULL;


    PDU* repdu =mkPDU(0);
    repdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPEND;
    return repdu;
}

PDU *MsgHandle::shareAgreeFile(PDU *pdu)
{
    QString strSharePath = pdu->caMsg;
    QString strRecvPath = QString("%1/%2")
            .arg(Server::getinstance().get_RootPath())
            .arg(pdu->uiData);

    int index = strSharePath.lastIndexOf('/');
    QString strFileName =strSharePath.right(
                strSharePath.size()-index-1);
    strRecvPath = strRecvPath+"/"+strFileName;
    bool ret =QFile::copy(strSharePath,strRecvPath);

    PDU*respdu = mkPDU(0);
    memcpy(respdu->uiData,&ret,sizeof(bool));
    respdu->uiMsgType =ENUM_MSG_TYPE_SHARE_FILE_AGREE_RESPEND;
    return respdu;
}

PDU *MsgHandle::loadFile(PDU *pdu)
{
    qDebug()<<"loadfile start;";
    PDU* respdu =mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_LOAD_FILE_RESPEND;
    int ret = 1;
    if(LoadFile){
        ret = 0 ;
        memcpy(respdu->uiData,&ret,sizeof(int));
        qDebug()<<"loadfile ret = "<<ret;
        return respdu;
    }
    char loadFilePath[32] = {'\0'};
    memcpy(loadFilePath,pdu->caMsg,pdu->uiMsgLen);
    m_LoadFile.setFileName(loadFilePath);

    qint64 LoadFileSize = m_LoadFile.size();

    if(!m_LoadFile.open(QIODevice::ReadOnly)){
        ret = -1;
        memcpy(respdu->uiData,&ret,sizeof(int));
        qDebug()<<"loadfile ret = "<<ret;
        return respdu;
    }
    memcpy(respdu->uiData,&ret,sizeof(int));
    memcpy(respdu->uiData,&LoadFileSize,sizeof(int));
    qDebug()<<"loadfile ret = "<<ret;
    qDebug()<<"LoadFileSize  = "<<LoadFileSize;
    return respdu;
}

PDU *MsgHandle::loadFileData(MyTcpSocket *socket)
{
    qDebug()<<"loadFileData start";
    PDU* respdu =mkPDU(4096);
    respdu->uiMsgType = ENUM_MSG_TYPE_LOAD_FILE_AGREE_RESPEND;
    qint64 ret = 0;
    while(true){
        ret =m_LoadFile.read(respdu->caMsg, 4096);
        if(ret<=0){
            break;
        }
        respdu->uiMsgLen = ret;
        respdu->uiPDULen = ret + sizeof(PDU);
        socket->write((char*)respdu,respdu->uiMsgLen);

    }
    m_LoadFile.close();
    LoadFile = false;
    free(respdu);
    respdu = NULL;
    return respdu;
}
