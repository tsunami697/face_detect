#ifndef RECORDLOG_H
#define RECORDLOG_H
#include "headers.h"
QFile *file;

QString File_ProcessLog;
QDateTime dateTime;
QString strDateTime;
QByteArray output;
QByteArray byte_array;


/* function list */
void isfileexist(void);
void recordLog(QString info);

#endif // RECORDLOG_H
