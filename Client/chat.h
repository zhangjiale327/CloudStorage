#ifndef CHAT_H
#define CHAT_H

#include <QWidget>

namespace Ui {
class Chat;
}

class Chat : public QWidget
{
    Q_OBJECT

public:
    explicit Chat(QWidget *parent = nullptr);
    ~Chat();
    QString m_strCharName;//用来记录对面的聊天用户
    void updateShow_LW(QString strMsg);//更新聊天框

private slots:
    void on_send_PB_clicked();//发送按钮

private:
    Ui::Chat *ui;
};

#endif // CHAT_H
