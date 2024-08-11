#include "chat.h"
#include "protocol.h"
#include "ui_chat.h"
#include "client.h"

#include <QMessageBox>

Chat::Chat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
}

Chat::~Chat()
{
    delete ui;
}

void Chat::updateShow_LW(QString strMsg)
{
    ui->show_LW->append(strMsg);
}

void Chat::on_send_PB_clicked()
{
    QString strMsg = ui->inPut_LE->text();//获取要发送的消息
    ui->inPut_LE->clear();
    if(strMsg.isEmpty()){
        QMessageBox::information(this,"聊天","请不要发送空消息!");
        return;
    }

    PDU*pdu=mkPDU(strMsg.toStdString().size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_CHAT_FRIEND_REQUEST;
    memcpy(pdu->uiData, Client::getinstance().get_strLoginName().toStdString().c_str(),32);//当前用户名
    memcpy(pdu->uiData+32,m_strCharName.toStdString().c_str(),32); //聊天对方用户名

    memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.toStdString().size());//实际消息内容
    QString Msg = QString("%1: \n   %2\n").arg(Client::getinstance().get_strLoginName().toStdString().c_str()).arg(strMsg);
    ui->show_LW->append(Msg);
    qDebug()<<"发送给服务器消息 chat";
    qDebug()<<"消息类型 chat="<<pdu->uiMsgType;
    qDebug()<<"发送消息内容 chat="<<pdu->caMsg;
    Client::getinstance().sendPdu(pdu);//客户端发送给服务器
}
