#include "file.h"
#include "ui_file.h"
#include "client.h"
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <QTimer>
File::File(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::File)
{
    ui->setupUi(this);
    strCurPath = QString("%1/%2").arg(Client::getinstance()
                                      .get_RootPath())
                                 .arg(Client::getinstance().get_strLoginName());
    userPath = strCurPath;
    flustFile();
    sharedfile = new ShareFile;
    upLoadaStat = false;
    loadFile = false;
}

File::~File()
{
    delete ui;
    delete sharedfile;
}

void File::flustFile()
{
    qDebug()<<"flush file start";
    PDU* pdu = mkPDU(strCurPath.toStdString().size());
    pdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.toStdString().size());
    qDebug()<<"flush_path:"<<strCurPath;
    Client::getinstance().sendPdu(pdu);

    //Client::getinstance().get_QTcpSocket().write((char*)pdu,sizeof(pdu));
//    QTimer::singleShot(1000,[&]{
//         Client::getinstance().get_QTcpSocket().write(
//                     strCurPath.toStdString().c_str(),strCurPath.toStdString().size());
    //});//用来测试半包

}//把当前路径和协议类型发送给服务器

void File::updateListWidget(QList<FileInfo *> PfILElIST)//更新窗口 刷新文件
{

    ui->listWidget->clear();//先清空

    foreach(FileInfo* PFileInfo, m_fILElIST){
        delete PFileInfo;
    }
    m_fILElIST.clear();
    m_fILElIST = PfILElIST;//刷新当前文件下的文件和目录


    int Pcount = PfILElIST.size();
    for(int i=0;i<Pcount;i++){
        QListWidgetItem *Pitem = new QListWidgetItem;//创建空间 用于输出在窗口内
        if(PfILElIST[i]->FileType==0){
            Pitem->setIcon(QIcon(QPixmap(":/Dir.jpg")));
        }else if(PfILElIST[i]->FileType==1){
            Pitem->setIcon(QIcon(QPixmap(":/File.jpg")));
        }//判断是文件还是目录 将其图标添加到item控件上
        Pitem->setText(PfILElIST[i]->FileName);//将文件或目录名添加到控件上
        qDebug()<<"PfILElIST[i]->FileName"<<PfILElIST[i]->FileName;
        ui->listWidget->addItem(Pitem);//窗口上添加该控件
    }
}

void File::uploaddata()
{
    QFile file(upLoadPath);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"上传文件","上传文件失败!");
        return;
    }

    upLoadaStat = true;
    PDU* datapdu = mkPDU(4096);
    datapdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_DATA_REQUEST;
    int ret = 0;
    while(true){
        ret = file.read(datapdu->caMsg,4096);
        if(ret == 0){

            break;
        }
        if(ret < 0){
            QMessageBox::warning(this,"上传文件","读取文件失败!");
            break;
        }
        datapdu->uiMsgLen = ret;
        datapdu->uiPDULen =ret+sizeof(PDU);
        Client::getinstance().get_QTcpSocket().write((char*)datapdu,datapdu->uiPDULen);
    }
    upLoadaStat = false;
    file.close();
    free(datapdu);
    datapdu = NULL;
}

void File::rsloadFile(qint64 loadfilesize)
{
    qDebug()<<"File:: downLoadFile start";
    loadFile = true;
    loadFileTotal = loadfilesize;
    recevedLoadFileSize = 0;
    m_loadfile.setFileName(saveFilePath);

    if(!m_loadfile.open(QIODevice::WriteOnly)){
        QMessageBox::warning(this,"下载文件","打开文件失败");
    }

    PDU*pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_LOAD_FILE_AGREE_REQUEST;
    Client::getinstance().sendPdu(pdu);
    qDebug()<<"File::loadfile end";
}

void File::loadFileData(char *buffer, int size)
{
    m_loadfile.write(buffer,size);
    recevedLoadFileSize += size;
    if(recevedLoadFileSize < loadFileTotal){
        return;
    }
    m_loadfile.close();
    loadFile = false;
    QMessageBox::information(this,"下载文件","下载文件完成!");
}

QString File::getstrCurPath()
{
    return strCurPath;
}

QString File::getuserPath()
{
    return userPath;
}


void File::on_mkDir_PB_clicked()
{
    QString strNewDirName= QInputDialog::getText(this,"创建文件","文件夹名");
    if(strNewDirName.isEmpty() || strNewDirName.toStdString().size()>32){
        QMessageBox::information(this,"创建文件","文件夹名字不合法!");
        return;
    }

    PDU* pdu = mkPDU(Index::getinstance().get_file()->strCurPath.toStdString().size());
    pdu->uiMsgType=ENUM_MSG_TYPE_MKDIR_REQUEST;

    memcpy(pdu->uiData,strNewDirName.toStdString().c_str(),strNewDirName.toStdString().size());
    //将文件名放在uidata里面 因为文件名可以放的下 而路径不一定在uidata放的下 所以放在Msg中
    memcpy(pdu->caMsg,Index::getinstance().get_file()->strCurPath.toStdString().c_str(),
           Index::getinstance().get_file()->strCurPath.toStdString().size());
    Client::getinstance().sendPdu(pdu);
}

void File::on_flushFile_PB_clicked()//刷新文件按钮
{
    flustFile();//刷新文件封装函数
}

void File::on_delDir_PB_clicked()
{
     QListWidgetItem*PItem = ui->listWidget->currentItem();//获取当前选择文件
     if(PItem==NULL){
          QMessageBox::warning(this,"删除文件夹","请选择文件夹!");
          return;
     }
     QString strDelFileName = PItem->text();

     for(int i=0;i<m_fILElIST.size();i++){
         if(m_fILElIST[i]->FileName == strDelFileName && m_fILElIST[i]->FileType == 1){
             QMessageBox::warning(this,"删除文件夹","请选择文件夹!");
             return;
         }
     }
     int ret = QMessageBox::question(this,"删除文件夹",QString("确认删除%1？").arg(strDelFileName));
     if(ret!=QMessageBox::Yes){
         return;
     }
     QString curPath = QString("%1/%2").arg(strCurPath).arg(strDelFileName);
     PDU* pdu = mkPDU(curPath.toStdString().size()+1);
     memcpy(pdu->caMsg,curPath.toStdString().c_str(),curPath.toStdString().size()+1);
     pdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_REQUEST;

     Client::getinstance().sendPdu(pdu);
 }

void File::on_delFile_PB_clicked()
{
    QListWidgetItem*PItem = ui->listWidget->currentItem();//获取当前选择文件
    if(PItem==NULL){
         QMessageBox::warning(this,"删除文件","请选择文件!");
         return;
    }
    QString strDelFileName = PItem->text();

    for(int i=0;i<m_fILElIST.size();i++){
        if(m_fILElIST[i]->FileName == strDelFileName && m_fILElIST[i]->FileType == 0){
            QMessageBox::warning(this,"删除文件","请选择文件");
            return;
        }
    }
    int ret = QMessageBox::question(this,"删除文件",QString("确认删除%1？").arg(strDelFileName));
    if(ret!=QMessageBox::Yes){
        return;
    }
    QString curPath = QString("%1/%2").arg(strCurPath).arg(strDelFileName);
    PDU* pdu = mkPDU(curPath.toStdString().size()+1);
    memcpy(pdu->caMsg,curPath.toStdString().c_str(),curPath.toStdString().size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_REQUEST;

    Client::getinstance().sendPdu(pdu);
}

void File::on_reName_PB_clicked()
{
    QListWidgetItem*PItem = ui->listWidget->currentItem();//获取当前选择文件夹或文件
    if(PItem==NULL){
         QMessageBox::warning(this,"重命名","请选择文件或文件夹!");
         return;
    }
    QString strOldName = PItem->text();
    QString strNewName= QInputDialog::getText(this,"重命名","文件夹名");
    qDebug()<<"strOldName="<<strOldName
           <<"strNewName"<<strNewName;
    QString curPath =strCurPath; //QString("%1/%2").arg(strCurPath).arg(strOldName);
    PDU* pdu = mkPDU(curPath.toStdString().size()+1);

    memcpy(pdu->uiData,strOldName.toStdString().c_str(),32);
    memcpy(pdu->uiData+32,strNewName.toStdString().c_str(),32);
    memcpy(pdu->caMsg,curPath.toStdString().c_str(),curPath.toStdString().size());
    pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_REQUEST;

    Client::getinstance().sendPdu(pdu);
}

void File::on_listWidget_itemDoubleClicked(QListWidgetItem *item)//双击进入文件夹
{
    QString curFileName = item->text();
    for(int i=0;i<m_fILElIST.size();i++){
        if(m_fILElIST[i]->FileName == curFileName && m_fILElIST[i]->FileType == 1){
            return;
        }
    }
    strCurPath = strCurPath+"/"+curFileName;
    flustFile();
}

void File::on_return_PB_clicked()//返回上一层目录
{
    if(strCurPath == userPath){
        QMessageBox::warning(this,"返回上一层目录","目前为最顶层目录!");
        return;
    }
    int index = strCurPath.lastIndexOf('/');
    strCurPath.remove(index,strCurPath.size()-index);
    flustFile();
}

void File::on_moveFile_PB_clicked()
{
    if(ui->moveFile_PB->text() == "移动文件"){
        //如果当前按钮是"移动文件"按钮 则进入到这个判断条件
        //获取选择要移动的文件名 以及现在文件所在的路径
        //并将按钮名字改为"确认"
        QListWidgetItem* pItem = ui->listWidget->currentItem();
        if(pItem == NULL){
            QMessageBox::warning(this,"移动文件","请选择要移动的文件!");
            return;
        }
        m_mvFileName = pItem->text();
        m_mvFilePath = strCurPath+"/"+m_mvFileName;
        //利用成员变量记录 要移动的文件及路径
        //因为如果不用成员变量 下次进来按钮为 "确认"时候
        //则不会进入到当前判断条件 上一次记录的要移动的文件及路径就丢失了
        QMessageBox::information(this,"移动文件","请选择要移动到的目录!");
        ui->moveFile_PB->setText("确认/取消");
        return;
    }
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    QString strTarPath;
    QString box;
    //用于记录目标路径
    if(pItem ==NULL){
        //如果为空 说明在当前界面中没有选择要移动的目录
        //可以移动到当前目录下
        strTarPath = strCurPath+"/"+m_mvFileName;
        //更新目标路径为当前路径下 加上文件名
        box = QString("是否移动到当前目录下？");
    }else{
        for(int i=0;i<m_fILElIST.size();i++){
            //判断选择要移动到的文件是否是目录 如果不是目录
            if(m_fILElIST[i]->FileName == pItem->text() && m_fILElIST[i]->FileType == 1){
                QMessageBox::warning(this,"移动文件","选择的不是文件夹!");
                return;
            }
        }
        strTarPath = strCurPath+"/"+pItem->text()+"/"+m_mvFileName;
        //更新目录 当前目录+要移动到的目录 在加上文件名
         box = QString("是否移动到所选目录下？");
    }
    int ret = QMessageBox::question(this,"移动文件",box,"确认","取消");
    //询问是否移动文件
    qDebug()<<"mvfile ret"<<ret;
    ui->moveFile_PB->setText("移动文件");
    //并按钮名改为"移动文件"
//    if(!ret){
//        return;
//    }
    int mv_len = m_mvFilePath.toStdString().size();
    int mv_Tar_len = strTarPath.toStdString().size();
    //获取移动前路径长度 以及移动后路径长度
    PDU* pdu = mkPDU(mv_len+mv_Tar_len+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_MVFILE_REQUEST;
    memcpy(pdu->uiData,&mv_len,sizeof(int));
    memcpy(pdu->uiData+sizeof(int),&mv_Tar_len,sizeof(int));
    //长度放在uidata里
    memcpy(pdu->caMsg,m_mvFilePath.toStdString().c_str(),mv_len);
    memcpy(pdu->caMsg+mv_len,strTarPath.toStdString().c_str(),mv_Tar_len);
    //实际路径放在caMsg中
    qDebug()<<"mv_len"<<mv_len
           <<"mv_tar_len"<<mv_Tar_len
          <<"mv_path"<<m_mvFilePath
         <<"mv_tar_path"<<strTarPath;
    //根据uidata里面的数据长度 读取caMsg的消息

    Client::getinstance().sendPdu(pdu);
}

void File::on_uploadFile_PB_clicked()//上传文件
{
    if(upLoadaStat){
        QMessageBox::warning(this,"上传文件","正在上传文件中，请稍等!");
    }
    upLoadPath = QFileDialog::getOpenFileName();
    qDebug()<<"upLoadPath"<<upLoadPath;
    int index = upLoadPath.lastIndexOf("/");
    QString upLoadName = upLoadPath.right(upLoadPath.toStdString().size()-index-1);
    qDebug()<<"upLoadName"<<upLoadName;
    QFile file(upLoadPath);
    qint64 file_size = file.size();

    PDU * pdu = mkPDU(strCurPath.toStdString().size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_REQUEST;
    memcpy(pdu->uiData,upLoadName.toStdString().c_str(),upLoadName.toStdString().size());
    memcpy(pdu->uiData+32,&file_size,sizeof(int));
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.toStdString().size());
    Client::getinstance().sendPdu(pdu);
}

void File::on_shareFile_PB_clicked()
{
    QListWidgetItem*PItem = ui->listWidget->currentItem();//获取当前选择文件夹或文件
    if(PItem==NULL){
         QMessageBox::warning(this,"分享文件","请选择要分享的文件!");
         return;
    }
    sharedFileName = PItem->text();
    sharedfile->updateFriend_LW();
    if(sharedfile->isHidden()){
        sharedfile->show();
    }
}

void File::on_installFile_PB_clicked()
{
    if(loadFile){
        QMessageBox::warning(this,"文件下载","文件正在下载!");
        return;
    }
    QListWidgetItem*PItem = ui->listWidget->currentItem();//获取当前选择文件夹或文件
    if(PItem==NULL){
         QMessageBox::warning(this,"下载文件","请选择要下载的文件!");
         return;
    }
    QString loadFilePath = strCurPath + "/"+PItem->text();
    qDebug()<<"loadFilePath"<<loadFilePath;
    saveFilePath.clear();
    saveFilePath = QFileDialog::getSaveFileName();//保存要下载到的路径
    if(saveFilePath.isEmpty()){
        QMessageBox::warning(this,"下载文件","请选择要保存的位置!");
        return;
    }
    PDU* pdu = mkPDU(loadFilePath.toStdString().size()+1);

    pdu->uiMsgType = ENUM_MSG_TYPE_LOAD_FILE_REQUEST;

    memcpy(pdu->caMsg,loadFilePath.toStdString().c_str(),loadFilePath.toStdString().size());
    Client::getinstance().sendPdu(pdu);


}
