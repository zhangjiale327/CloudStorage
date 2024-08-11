#include "index.h"
#include "onlineuser.h"
#include "ui_onlineuser.h"
#include "client.h"
OnlineUser::OnlineUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlineUser)
{
    ui->setupUi(this);

}

OnlineUser::~OnlineUser()
{
    delete ui;
}

void OnlineUser::updateOnlineWidgetList(QStringList onlineUser)
{
    ui->onlineUser_PB->clear();
    ui->onlineUser_PB->addItems(onlineUser);
}

void OnlineUser::on_onlineUser_PB_itemDoubleClicked(QListWidgetItem *item)
{
    QString strCurName = Client::getinstance().get_strLoginName();
    QString strTarName = item->text();
    qDebug()<<"添加好友请求: 当前用户名:"<<strCurName
           <<"目标用户名:"<<strTarName;
    PDU* pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->uiData,strCurName.toStdString().c_str(),32);
    memcpy(pdu->uiData+32,strTarName.toStdString().c_str(),32);
    Client::getinstance().sendPdu(pdu);
    return;
}
