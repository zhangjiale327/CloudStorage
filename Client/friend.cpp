#include "friend.h"
#include "protocol.h"
#include "ui_friend.h"
#include "client.h"
#include <QInputDialog>
#include <qmessagebox.h>
#include <QMessageBox>

Friend::Friend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Friend)
{
    ui->setupUi(this);
    onelineUser=new OnlineUser;
    m_chat=new Chat;
    //flushFriendOline(Client::getinstance().get_strLoginName());
}

Friend::~Friend()
{
    delete ui;
    delete onelineUser;
    delete m_chat;
}

OnlineUser *Friend::getOnlineUser()
{
    return onelineUser;
}

Chat *Friend::get_m_chat()
{
    return m_chat;
}

void Friend::flushFriendOline(QString curName)
{
    if(curName==NULL) return;
    PDU*pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_ONLINE_REQUEST;
    memcpy(pdu->uiData,curName.toStdString().c_str(),32);
    Client::getinstance().sendPdu(pdu);
}

void Friend::updateFriendLw(QStringList strFriends)
{
    ui->onlineFriend_LW->clear();
    ui->onlineFriend_LW->addItems(strFriends);
}

QListWidget *Friend::getFriend_LW()
{
    return ui->onlineFriend_LW;
}
//刷新好友界面
void Friend::on_findFriend_PB_clicked()
{
    QString Name=QInputDialog().getText(this,"查找","用户名");
    qDebug()<<"要查找的用户名"<<Name;
    if(Name.isEmpty()|| Name.toStdString().size()>32 ){
//        QMessageBox::information(this,"查找","用户名不合法!");
        return;
    }
    PDU* pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_FINDFRIEND_REQUEST;
    memcpy(pdu->uiData,Name.toStdString().c_str(),32);
    Client::getinstance().sendPdu(pdu);
    return;
}

void Friend::on_onlineUser_PB_clicked()
{
    if(getOnlineUser()->isHidden()){
        getOnlineUser()->show();
    }//如果在线人数界面已经出来 则不需要再次出现该窗口
    PDU* pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_ONLINEUSER_REQUEST;
    Client::getinstance().sendPdu(pdu);
    return;
}

void Friend::on_flushFriend_PB_2_clicked()
{
    flushFriendOline(Client::getinstance().get_strLoginName());
}

void Friend::on_delUser_PB_clicked()//删除好友
{

      QListWidgetItem* PItem= ui->onlineFriend_LW->currentItem();//获取要删除的当前用户
      if(PItem==NULL){
          QMessageBox::information(this,"删除好友","请选择要删除的好友!");
          return;
      }
      QString strItem=PItem->text();
      int ret=QMessageBox::question(this,"删除好友",QString("是否删除好友'%1'").arg(strItem));
      if(ret!=QMessageBox::Yes){
          return;
      }
      PDU*pdu=mkPDU(0);
      pdu->uiMsgType=ENUM_MSG_TYPE_DEL_ONLINE_FRIEND_REQUEST;
      memcpy(pdu->uiData,Client::getinstance().get_strLoginName().toStdString().c_str(),32);
      memcpy(pdu->uiData+32,strItem.toStdString().c_str(),32);
      Client::getinstance().sendPdu(pdu);
}

void Friend::on_chat_PB_clicked()
{
    QListWidgetItem* PItem= ui->onlineFriend_LW->currentItem(); //获取要聊天的当前用户
    if(PItem==NULL){
        QMessageBox::information(this,"聊天","请选择要聊天的好友!");
        return;
    }
    qDebug()<<"friend PItem->text()"<<PItem->text();
    if(m_chat->isHidden()){
        m_chat->show();
    }
    m_chat->m_strCharName = PItem->text();
}
