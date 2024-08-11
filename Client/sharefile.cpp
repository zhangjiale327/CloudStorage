#include "sharefile.h"
#include "ui_sharefile.h"
#include "index.h"
#include "client.h"
ShareFile::ShareFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShareFile)
{
    ui->setupUi(this);
}

ShareFile::~ShareFile()
{
    delete ui;
}

void ShareFile::updateFriend_LW()
{
    ui->listWidget->clear();
    QListWidget* friend_LW = Index::getinstance().get_friend()->getFriend_LW();
    for(int i=0;i<friend_LW->count();i++){
        QListWidgetItem* item = friend_LW->item(i);
        QListWidgetItem* newItem = new QListWidgetItem(*item);
        ui->listWidget->addItem(newItem);
    }
}

void ShareFile::on_allselect_PB_clicked()
{
    for(int i=0;i<ui->listWidget->count();i++){
        ui->listWidget->item(i)->setSelected(true);
    }
}

void ShareFile::on_cacleselect_PB_clicked()
{
    for(int i=0;i<ui->listWidget->count();i++){
        ui->listWidget->item(i)->setSelected(false);
    }
}

void ShareFile::on_ok_PB_clicked()
{
    QString strCurPath = Index::getinstance().get_file()->getstrCurPath();
    QString strPath = strCurPath+"/"+Index::getinstance().get_file()->sharedFileName;

    QString strCurName = Client::getinstance().get_strLoginName();
    QList<QListWidgetItem*> pItems = ui->listWidget->selectedItems();
    int friend_count = pItems.size();
    PDU* pdu = mkPDU(friend_count*32 + strPath.toStdString().size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;

    memcpy(pdu->uiData,strCurName.toStdString().c_str(),32);
    memcpy(pdu->uiData+32,&friend_count,sizeof(int));

    for(int i=0;i<friend_count;i++){
        memcpy(pdu->caMsg+i*32,pItems.at(i)->text().toStdString().c_str(),32);

    }
    memcpy(pdu->caMsg+friend_count*32,strPath.toStdString().c_str(),strPath.toStdString().size());
    Client::getinstance().sendPdu(pdu);
}
