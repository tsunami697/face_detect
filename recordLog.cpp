#include "recordLog.h"
#include "headers.h"
void isfileexist(void){
    if(!file->exists()){
        file->open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text);
    }else{
        file->open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text);
    }
}

void recordLog(QString info){
    dateTime = QDateTime::currentDateTime();
    strDateTime = dateTime.toString("yyyy-mm-dd hh:mm:ss");
    isfileexist();
    QTextStream out(file);
    out << "[" << strDateTime << "] " << info << endl;
    file->close();
}
