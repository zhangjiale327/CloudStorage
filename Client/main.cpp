#include "client.h"
#include "index.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client::getinstance().show();
    return a.exec();
}
