#ifndef RECORD_H
#define RECORD_H
#include <QString>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QFile>
class record
{
public:
    record(QString logfilename);
    ~record();
    //写好了，不要修改
    void isfileexist();
    void recordLog(QString info);
    //用于操作文件日志，写好了，不要修改
    QFile *file;
    QDateTime dateTime;
    QString strDateTime;
};

#endif // RECORD_H
