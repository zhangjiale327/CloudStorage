#ifndef OPERATEDB_H
#define OPERATEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QDebug>
#include  <QStringList>
class OperateDB : public QObject
{
    Q_OBJECT
public:
    static OperateDB& getinstance();//将数据库类操作对象设置为单例模式
    void connect();//用于连接数据库的函数
    QSqlDatabase m_DB;//数据库操作对象
    bool handleRegist(char* name,char* pwd);//操作注册
    bool handleLogoff(char* name,char* pwd);//注销操作
    bool handleLogin(char* name,char*pwd); //操作登录
    void handleoffline(const char*name); //处理客户端与服务器断开连接后 将该客户端用户状态设置为0
    int handleFindFriend(const char*name);//查找好友
    QStringList handleOnlineUser();//查看在线用户
    int handleAddFriend(char* strCurName,char* strTarName);//添加好友
    void handleAgreeAddFriend(char* strCurName,char* strTarName);
    bool handleDelOnlineFriend(char* strCurName,char* strDelName);
    QStringList handleflushOnlineFriend(char*curName);
    ~OperateDB();
private:
    OperateDB(QObject *parent = nullptr);
    OperateDB(const OperateDB&instance)=delete;
    OperateDB& operator=(const OperateDB&instance)=delete;
};

#endif // OPERATEDB_H
