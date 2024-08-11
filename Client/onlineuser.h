#ifndef ONLINEUSER_H
#define ONLINEUSER_H

#include <QWidget>
#include <qlistwidget.h>

namespace Ui {
class OnlineUser;
}

class OnlineUser : public QWidget
{
    Q_OBJECT

public:
    explicit OnlineUser(QWidget *parent = nullptr);
    ~OnlineUser();
    void updateOnlineWidgetList(QStringList);

private slots:
    void on_onlineUser_PB_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::OnlineUser *ui;
};

#endif // ONLINEUSER_H
