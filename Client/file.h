#ifndef FILE_H
#define FILE_H

#include <QFile>
#include <QListWidget>
#include <QWidget>
#include "protocol.h"
#include "sharefile.h"
namespace Ui {
class File;
}

class File : public QWidget
{
    Q_OBJECT

public:
    explicit File(QWidget *parent = nullptr);
    ~File();
    void flustFile();
    QList<FileInfo*> m_fILElIST;//用于记录当前目录下所有的文件及目录
    void updateListWidget(QList<FileInfo*> PfILElIST);
    QString m_mvFileName;
    QString m_mvFilePath;
    QString upLoadPath;
    QString sharedFileName;
    bool upLoadaStat;
    void uploaddata();
    void rsloadFile(qint64 loadfilesize);
    void loadFileData(char*buffer ,int size);

    bool loadFile;
    QString saveFilePath;
    int loadFileTotal;
    int recevedLoadFileSize;
    QFile m_loadfile;


    ShareFile* sharedfile;
    QString getstrCurPath();
    QString getuserPath();
private slots:
    void on_mkDir_PB_clicked();

    void on_flushFile_PB_clicked();

    void on_delDir_PB_clicked();

    void on_delFile_PB_clicked();

    void on_reName_PB_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_return_PB_clicked();

    void on_moveFile_PB_clicked();

    void on_uploadFile_PB_clicked();

    void on_shareFile_PB_clicked();

    void on_installFile_PB_clicked();

private:
    Ui::File *ui;
    QString strCurPath; //用来记录当前用户的根目录文件路径
    QString userPath;   //用来记录当前在根目录下的那个文件目录下
};

#endif // FILE_H
