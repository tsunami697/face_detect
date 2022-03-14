#ifndef YUV2RGB24_H
#define YUV2RGB24_H
#include <QImage>
#include <QString>>
#include <QByteArray>
#include <QFile>
#include <QObject>
#include <QThread>
#include <QDateTime>
class yuv2rgb24:public QThread
{
    Q_OBJECT
public:
    yuv2rgb24(struct buffer *get_buffer,int fd);
    ~yuv2rgb24();
    void one_Frame();
protected:    
    void run();
private:
    //写好了，不要修改
    void isfileexist();
    void recordLog(QString info);

    unsigned char* yuv2rgb(unsigned char* YUY2buff, int count);
    QImage convert2QImage();
    unsigned char judge(unsigned char num);

    buffer *get_buffers;
    buffer *get_buffers1;

    QByteArray output;
    QImage img;

    QFile *file;                    /* = new QFile(File_ProcessLog);*/
    QDateTime dateTime;
    QString strDateTime;
    QString File_ProcessLog;
    int fd1;
    QImage sendImg;
signals:
    void signal_sendQImg(QImage);
};
#endif // YUV2RGB24_H
