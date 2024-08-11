#include "server.h"

#include <QApplication>
#include "operatedb.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OperateDB::getinstance().connect();
    Server w;
   // w.show(); 服务器我们不需要展示界面 所以关闭show
    return a.exec();
}
