#ifndef INDEX_H
#define INDEX_H

#include <QWidget>
#include "friend.h"
#include"file.h"
namespace Ui {
class Index;
}

class Index : public QWidget
{
    Q_OBJECT

public:
    ~Index();
    Friend* get_friend();
    File* get_file();
    static Index& getinstance();
private slots:
    void on_friend_PB_clicked();//好友界面按钮

    void on_file_PB_clicked();//文件界面按钮

private:
    Ui::Index *ui;
    explicit Index(QWidget *parent = nullptr);
    Index(const Index&instance)=delete;
    Index& operator=(const Index&instance)=delete;
};

#endif // INDEX_H
