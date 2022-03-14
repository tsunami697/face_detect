#include "yuv2rgb24.h"
#include "mainwindow.h"
yuv2rgb24::yuv2rgb24(buffer *get_buffer,int fd)
{
    get_buffers = (struct buffer *)calloc(4, sizeof(*get_buffers));
    get_buffers = get_buffer;
    File_ProcessLog = "./process.log";
    file = new QFile(File_ProcessLog);  //log file
    fd1 = fd;
}

yuv2rgb24::~yuv2rgb24()
{
    unsigned int i;
    for (i = 0; i < 4; ++i)
    {
        munmap(get_buffers[i].start, get_buffers[i].length);
    }
    free(get_buffers);
}
void yuv2rgb24::run(){
    int returnValue;
    unsigned char * value;
    while(1){
        struct v4l2_buffer buf;     /* [struct v4l2_buffer] use to save frames */
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        returnValue = ioctl(fd1, VIDIOC_DQBUF, &buf); //get out frame

        value = yuv2rgb(get_buffers[0].start, buf.bytesused);
        QImage sendImg(value,320,240,QImage::Format_RGB888);
        emit signal_sendQImg(sendImg);
        ioctl(fd1, VIDIOC_QBUF, &buf);

        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        returnValue = ioctl(fd1, VIDIOC_QBUF, &buf);
    }
}
void yuv2rgb24::one_Frame(){
    int returnValue;
    unsigned char * value;
    struct v4l2_buffer buf;     /* [struct v4l2_buffer] use to save frames */
    CLEAR(buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    returnValue = ioctl(fd1, VIDIOC_DQBUF, &buf); //get out frame

    value = yuv2rgb(get_buffers[0].start, buf.bytesused);
    QImage sendImg(value,320,240,QImage::Format_RGB888);
    emit signal_sendQImg(sendImg);
    ioctl(fd1, VIDIOC_QBUF, &buf);

    //CLEAR(buf);
    //buf.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    //buf.memory = V4L2_MEMORY_MMAP;
    returnValue = ioctl(fd1, VIDIOC_QBUF, &buf);
}
unsigned char* yuv2rgb24::yuv2rgb(unsigned char *YUY2buff, int count){
    int dwSize = count;
    output.clear();         //must have
    for(int i=0;i<dwSize;i+=4)
    {
        unsigned char Y0 = *YUY2buff;
        unsigned char U  = *(++YUY2buff);
        unsigned char Y1 = *(++YUY2buff);
        unsigned char V  = *(++YUY2buff);
        ++YUY2buff;
        // RGB第一个像素点
        output.append(judge((char)(Y0 + 1.370705 * (V-128))));
        output.append(judge((char)(Y0 - (0.698001 * (V-128)) - (0.337633 * (U-128)))));
        output.append(judge((char)(Y0 + (1.732446 * (U-128)))));
        // RGB第二个像素点
        output.append(judge((char)(Y1 + (1.370705 * (V-128)))));
        output.append(judge((char)(Y1 - (0.698001 * (V-128)) - (0.337633 * (U-128)))));
        output.append(judge((char)(Y1 + (1.732446 * (U-128)))));
    }
    unsigned char *c = (unsigned char *)output.data();
    return c;
}

unsigned char yuv2rgb24::judge(unsigned char num){
    if(num<0)
        return 0;
    else if(num>255)
        return 255;
    else
        return num;
}
//写好了，不要修改
void yuv2rgb24::isfileexist(){
    if(!file->exists()){
        file->open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text);
    }else{
        file->open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text);
    }
}
void yuv2rgb24::recordLog(QString info){
    dateTime = QDateTime::currentDateTime();
    strDateTime = dateTime.toString("yyyy-mm-dd hh:mm:ss");
    isfileexist();
    QTextStream out(file);
    out << "[" << strDateTime << "] " << info << endl;
    file->close();
}
