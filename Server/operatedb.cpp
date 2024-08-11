#include "operatedb.h"
#include "mytcpserver.h"
#include <QDebug>
#include <QSqlQuery>
OperateDB &OperateDB::getinstance()
{
    static OperateDB instance;
    return instance;
}

void OperateDB::connect()
{
    m_DB.setHostName("localhost");
    m_DB.setDatabaseName("qt");
    m_DB.setPort(3306);
    m_DB.setUserName("root");
    m_DB.setPassword("123456");
    //初始化要连接的数据库消息
    if(m_DB.open()){
        qDebug()<<"连接数据库成功!";
    }else{
        qDebug()<<"连接数据库失败!";
    }
}

bool OperateDB::handleRegist(char* name,char* pwd)//处理注册功能
{
    if(name==NULL || pwd==NULL){
        return false;
    }
    QString sql=QString("select * from user_info where name='%1';").arg(name);
    qDebug()<<"执行的sql语句"<<sql;//用来判断是否sql语句写错了
    QSqlQuery q;//创建一个sql语句执行对象 用于执行sql语句

    if(!q.exec(sql) || q.next()){
        //q.exec() 用于执行sql语句 返回值为是否执行成功 我们对其返回值进行取反
        //如果返回值为false 则进入判断语句 直接返回false q.next() 的作用是从查询结果中取出一条数据
        //如果能够取出一条数据返回真 返回真的话说明数据库已经存在这个用户 则直接返回false
        return false;
    }
    //上面没有查询到已经存在的 接下来就把该数据插入到数据库中
    sql=QString("insert into user_info(name,pwd) values('%1','%2');").arg(name).arg(pwd);
    qDebug()<<"执行的sql语句"<<sql;//用来判断是否sql语句写错了
    return q.exec(sql);//判断是否注册成功
}

bool OperateDB::handleLogoff(char *name, char *pwd)
{
    if(name==NULL || pwd==NULL){
        return false;
    }
    QString sql=QString("select * from user_info where name='%1';").arg(name);
    qDebug()<<"logoff sql1:"<<sql;
    QSqlQuery q;
    q.exec(sql);
    if(!q.next()){
        return false;
    }
    sql=QString("select * from user_info where name='%1' and pwd='%2';").arg(name).arg(pwd);
    qDebug()<<"logoff sql2:"<<sql;
    q.exec(sql);
    if(!q.next()){
        return false;
    }
    sql=QString("delete from friend where user_id = (select id from user_info where name='%1') or friend_id =(select id from user_info where name='%2');").arg(name).arg(pwd);
    qDebug()<<"logoff sql3:"<<sql;
    q.exec(sql);
    sql=QString("delete from user_info where name='%1';").arg(name);
    q.exec(sql);
    qDebug()<<"logoff sql4:"<<sql;
    return true;
}

bool OperateDB::handleLogin(char *name, char *pwd)
{
    QString sql=QString("select * from user_info where name='%1' and pwd='%2';").arg(name).arg(pwd);
    qDebug()<<"执行的sql语句"<<sql;//用来判断是否sql语句写错了
    QSqlQuery q;//创建一个sql语句执行对象 用于执行sql语句
    q.exec(sql);
    if(!q.next()){
        //q.exec() 用于执行sql语句 返回值为是否执行成功 我们对其返回值进行取反
        //如果返回值为false 则进入判断语句 直接返回false q.next() 的作用是从查询结果中取出一条数据
        //如果能够取出一条数据返回真 返回真的话说明数据库已经存在这个用户 则直接返回false
        return false;
    }
    sql=QString("update user_info set is_online=1 where name='%1';").arg(name);
    q.exec(sql);
    qDebug()<<"执行的sql语句"<<sql;//用来判断是否sql语句写错了
    return true;//判断是否登录成功
}

void OperateDB::handleoffline(const char* name)
{
    if(name==NULL) {
        return;
    }
    //执行sql语句
    QString sql=QString("update user_info set is_online=0 where name='%1';").arg(name);
//    qDebug()<<"执行的sql语句"<<sql;//用来判断是否sql语句写错了
    QSqlQuery q;
    q.exec(sql);
}

int OperateDB::handleFindFriend(const char *name)
{
    if(name==NULL){
        return false;
    }
    QString sql=QString("select is_online from user_info where name='%1';").arg(name);
    //查看当前用户的在线状态是否为1
    qDebug()<<"查找用户执行的SQL语句1:"<<sql;
    QSqlQuery q;
    q.exec(sql);
    //执行sql语句
    if(!q.next()){
        //如果没有数据 说明不存在
        return -1;
    }
    return q.value(0).toInt();
    //获取在线状态字段 返回的就是1 0
}

QStringList OperateDB::handleOnlineUser()
{
    QString sql=QString("select name from user_info where is_online=1;");
    QStringList onlineUser;
    QSqlQuery q;
    q.exec(sql);
    while(q.next()){
        onlineUser.append(q.value(0).toString());
    }
    return onlineUser;
}

int OperateDB::handleAddFriend(char *strCurName, char *strTarName)
{
    if(strCurName==NULL || strTarName== NULL){
        return -1;
    }
    QString sql=QString("select * from friend where(user_id=(select id from user_info where name='%1')and friend_id=(select id from user_info where name='%2'))or(friend_id=(select id from user_info where name='%3')and user_id=(select id from user_info where name='%4'));").arg(strCurName).arg(strTarName).arg(strCurName).arg(strTarName);
    QSqlQuery q;
    q.exec(sql);
    if(q.next()){
        return 2;
    }
    sql=QString("select is_online from user_info where name ='%1';").arg(strTarName);
    q.exec(sql);
    q.next();
    return q.value(0).toInt();
}

void OperateDB::handleAgreeAddFriend(char *strCurName, char *strTarName)
{
    if(strCurName==NULL || strTarName== NULL){
        return;
    }
    QString sql=QString(R"(
                        insert into friend(user_id,friend_id)
                        values (
                        (select id from user_info where name="%1"),
                        (select id from user_info where name="%2")
                        );)").arg(strCurName).arg(strTarName);
    QSqlQuery q;
    qDebug()<<"将用户好友关系插入到friend表执行的sql语句"<<sql;
    q.exec(sql);
}

bool OperateDB::handleDelOnlineFriend(char *strCurName, char *strDelName)
{
    if(strCurName == NULL || strDelName ==NULL){
        return false;
    }
    QString sql1=QString(R"((user_id=(select id from user_info where name='%1') and  friend_id=(select id from user_info where name='%2'))or(friend_id=(select id from user_info where name='%3') and user_id=(select id from user_info where name='%4'));)").arg(strCurName).arg(strDelName).arg(strCurName).arg(strDelName);
    QSqlQuery q;
    QString sql2=QString("select * from friend where %1").arg(sql1);
    //qDebug()<<"SQL1"<<sql2;
    q.exec(sql2);
    if(!q.next()){
        return false;
    }
    sql2=QString("delete from friend where %1").arg(sql1);
    //qDebug()<<"SQL2"<<sql2;
    return q.exec(sql2);
}

QStringList OperateDB::handleflushOnlineFriend(char *curName)
{
    QStringList strList;
    QString sql=QString(R"(
                        select name from user_info
                          where id in(
                          select user_id from friend where friend_id= (select id from user_info where name='%1')
                          union
                          select friend_id from friend where user_id= (select id from user_info where name='%2')
                          ) and is_online=1;)").arg(curName).arg(curName);
    QSqlQuery q;
    //qDebug()<<"flush friend sql"<<sql;
    q.exec(sql);
    while(q.next()){
        strList.append(q.value(0).toString());
    }
    return strList;
}

OperateDB::~OperateDB()
{
    m_DB.close();
}

OperateDB::OperateDB(QObject *parent) : QObject(parent)
{
    m_DB=QSqlDatabase::addDatabase("QMYSQL");//初始化操作数据库对象
}
