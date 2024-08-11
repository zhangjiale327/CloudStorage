#include "index.h"
#include "ui_index.h"
#include<QMainWindow.h>
Index::Index(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Index)
{
    ui->setupUi(this);
}

Index::~Index()
{
    delete ui;
}

File* Index::get_file()
{
    return ui->filePage;
}

Index &Index::getinstance()
{
    static Index instance;
    return instance;
}

Friend* Index::get_friend()
{
    return ui->friendPage;
}

void Index::on_friend_PB_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->friend_PB->setStyleSheet("QpushButton{border:none;background-color:rgb(255, 0, 0);padding:20px;}");
    ui->file_PB->setStyleSheet("QpushButton{border:none;padding:20px;}");
}

void Index::on_file_PB_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->file_PB->setStyleSheet("QpushButton{border:none;background-color:rgb(255, 0, 0);padding:20px;}");
    ui->friend_PB->setStyleSheet("QpushButton{border:none;padding:20px;}");
}
