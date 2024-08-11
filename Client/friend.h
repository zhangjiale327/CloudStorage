#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include "chat.h"
#include "onlineuser.h"
namespace Ui {
class Friend;
}

class Friend : public QWidget
{
    Q_OBJECT

public:
    ~Friend();
    explicit Friend(QWidget *parent = nullptr);
    OnlineUser* getOnlineUser();
    Chat* get_m_chat();
    void flushFriendOline(QString curName);
    void updateFriendLw(QStringList strFriends);
    QListWidget* getFriend_LW();

private slots:
    void on_findFriend_PB_clicked();

    void on_onlineUser_PB_clicked();

    void on_flushFriend_PB_2_clicked();

    void on_delUser_PB_clicked();

    void on_chat_PB_clicked();

private:
    Ui::Friend *ui;
    OnlineUser* onelineUser;
    Chat* m_chat;
};

#endif // FRIEND_H
