#include "pre_definition.h"
#include "headers.h"
void open_device(const char* deviceName)
{
    recordLog("start to init device......");
    fd = open(deviceName,O_RDWR);     /* open camera device */
    if(fd==-1){
        errno_exit("打开失败！");
    }
}
void init_device()
{
    int returnValue;

    returnValue = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    if(returnValue==-1){
        recordLog("ioctl(fd, VIDIOC_QUERYCAP, &cap) return value: "+QString::number(returnValue,10).toUpper());
    }else{
        recordLog("ioctl(fd, VIDIOC_QUERYCAP, &cap) return value: "+QString::number(returnValue,10).toUpper());
        recordLog("cap.capabilities value: "+QString::number(cap.capabilities,16).toUpper());
    }
    /* my device value :4000001 */
    CLEAR(fmt);
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 320;
    fmt.fmt.pix.height = 240;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    returnValue = ioctl(fd, VIDIOC_S_FMT, &fmt);
    if(returnValue == -1){
        errno_exit("return value wrong: "+QString::number(returnValue,10).toUpper());
    }else{
        recordLog("ioctl(fd, VIDIOC_S_FMT, &fmt) return value: "+QString::number(returnValue,10).toUpper());
    }
    recordLog("init device finish!!!!!!");
}
//申请内存
void init_mmap()
{
    int returnValue;
    /* struct v4l2_requestbuffers req;结构体初始化,分配内存 */
    recordLog("start to request memery......");

    CLEAR(req);
    req.count = 4;      /* 最大是5 */
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    returnValue = ioctl(fd, VIDIOC_REQBUFS, &req);
    if(returnValue == -1){
        errno_exit("ioctl(fd, VIDIOC_REQBUFS, &req) return value wrong: "+QString::number(returnValue,10).toUpper());
    }else{
        recordLog("ioctl(fd, VIDIOC_REQBUFS, &req) return value :"+QString::number(returnValue,10).toUpper());
    }
    /*  calloc函数原型：void *calloc(size_t n, size_t size)；
     * 功 能： 在内存的动态存储区中分配n个长度为size的连续空间，
     * 函数返回一个指向分配起始地址的指针；如果分配不成功，返回NULL。
     */
    buffers = (struct buffer *)calloc(req.count, sizeof(*buffers));
    if(buffers == NULL){
        errno_exit("内存分配失败！");
    }else {
        for(n_buffers = 0; n_buffers < req.count; ++n_buffers){
            struct v4l2_buffer buf;
            CLEAR(buf);
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = n_buffers;
            ioctl(fd, VIDIOC_QUERYBUF, &buf);
            buffers[n_buffers].length = buf.length;
            /* 把申请的内存映射到buffers数组 */
            buffers[n_buffers].start = (unsigned char *)mmap(NULL,
                                                             buf.length,
                                                             PROT_READ | PROT_WRITE ,
                                                             MAP_SHARED ,
                                                             fd,
                                                             buf.m.offset);
        }
    }
    recordLog("init mmap finish!!!");
}
//开启捕获
void start_capture()
{
    unsigned int i;
    int returnValue;
    enum v4l2_buf_type type;    /* 数据流类型 */

    for (i = 0; i < n_buffers; ++i){
        struct v4l2_buffer buf;
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        /* 将缓冲帧放入队列 */
        ioctl(fd, VIDIOC_QBUF, &buf);
    }
    recordLog("n_burrers value: "+QString::number(n_buffers,10).toUpper());
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    returnValue = ioctl(fd, VIDIOC_STREAMON, &type);
    if(returnValue == -1){
        errno_exit("ioctl(fd, VIDIOC_STREAMON, &type) return value wrong: "+QString::number(returnValue,10).toUpper());
    }else{
        recordLog("ioctl(fd, VIDIOC_STREAMON, &type) return value :"+QString::number(returnValue,10).toUpper());
        recordLog("capture start......");
    }
}
//读取一帧
void read_frame()
{
    int returnValue;
    struct v4l2_buffer buf;     /* [struct v4l2_buffer] use to save frames */
    CLEAR(buf);
    recordLog("start read frame ......");
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    returnValue = ioctl(fd, VIDIOC_DQBUF, &buf);    //read frame
    if(returnValue == -1){
        errno_exit("ioctl(fd, VIDIOC_DQBUF, &buf) return value wrong: "+QString::number(returnValue,10).toUpper());
    }else{
        recordLog("ioctl(fd, VIDIOC_DQBUF, &buf) return value :"+QString::number(returnValue,10).toUpper());
        recordLog("read frame......");
    }
    //format_thread = new yuv2rgb24(buffers,fd);
    //connect(format_thread,SIGNAL(signal_sendQImg(QImage)),this,SLOT(slots_play1Frame(QImage)));
    //format_thread->start();
    //process_image(buffers[buf.index].start, buf.bytesused,yuv_file);
    //ui->label_2->setText(QString::number(buffers[1].length,16).toUpper());
}

void stop_capture(){
    int returnValue;
    enum v4l2_buf_type type;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    returnValue = ioctl(fd, VIDIOC_STREAMOFF, &type);
    if(returnValue == -1){
        errno_exit("ioctl(fd, VIDIOC_STREAMOFF, &type) return value wrong: "+QString::number(returnValue,10).toUpper());
    }else{
        recordLog("ioctl(fd, VIDIOC_STREAMOFF, &type) return value :"+QString::number(returnValue,10).toUpper());
        recordLog("capture stop......");
    }
}

void close_device(void){
    ::close(fd);
    recordLog("close device......");
}

void release_memery(void){
    unsigned int i;
    for (i = 0; i < n_buffers; ++i)
    {
        munmap(buffers[i].start, buffers[i].length);
    }
    free(buffers);
    recordLog("memery release finish......");
}
void errno_exit(QString info){
    recordLog(info);
    exit(EXIT_FAILURE);
}
