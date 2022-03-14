#include "record.h"

record::record(QString logfilename)
{    
    file = new QFile(logfilename);  //log file
}
record::~record(){
    file->close();
}
// 检测文件是否存在，不存在则创建
void record::isfileexist()
{
    if(!file->exists()){
        file->open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text);
    }else{
        file->open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text);
    }
}
//向文件中写入内容
void record::recordLog(QString info)
{
    dateTime = QDateTime::currentDateTime();
    strDateTime = dateTime.toString("yyyy-mm-dd hh:mm:ss");
    isfileexist();
    QTextStream out(file);
    out << "[" << strDateTime << "] " << info << endl;
    file->close();
}
