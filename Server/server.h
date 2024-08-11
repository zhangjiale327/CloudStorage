#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QFile>
#include <QDebug>
QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

class Server : public QWidget
{
    Q_OBJECT

public:
    Server(QWidget *parent = nullptr);
    ~Server();
    void loadconfig();//查看配置文件
    static Server& getinstance();
    QString& get_RootPath();
private:
    Ui::Server *ui;
    QString m_strIp;//IP地址
    quint16 m_Port;//端口号 将这两者设置为成员变量的目的是为了我们能在其他地方用到它
    QString m_RootPath;
};
#endif // SERVER_H
