#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QObject>
#define FOUR_TWO_TWO 2
#define CLEAR(x) memset(&(x), 0, sizeof(x))

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);   
    this->setWindowFlags(Qt::FramelessWindowHint);
    // 2 line stop rewrite
    File_ProcessLog = "./process.log";
    file = new QFile(File_ProcessLog);  //log file
    //yuv_file = fopen("image.yuv","w");
    device_name = "/dev/video3";
    count = 4;		//我们设置为4，最大是5
    x = 0;
    if(cascade.load("/root/qtProBin/haarcascade_frontalface_alt.xml"))
    recordLog("load xml sucess.......");
    nestedCascade.load("/root/qtProBin/haarcascade_frontalface_alt.xml");

    mydevice = new device();
    myled8 = new led8_i2c();
    connect(mydevice,SIGNAL(sendMessage(int)),this,SLOT(getMessage(int)));
    connect(mydevice,SIGNAL(sendI2cvalue(unsigned char)),this,SLOT(getI2cvalue(unsigned char)));

    //start device thread: key
    mydevice->start();
    getfacepath = "./getface.bmp";

    open_device();		//只是打开设备：open()
    init_device();		//初始化摄像头参数信息
    init_mmap();
    start_capture();
    read_frame();

    //IIC2Led8 timer setting
    Setting_frameNumer = 30;
    timer = new QTimer();
    timer->setInterval(Setting_frameNumer); /* 开始计时，超时则发出timeout()信号,触发槽函数 readFrame() */
    connect(timer, SIGNAL(timeout()), this, SLOT(timerReadFrame()));
    /* 每秒处理30帧 1000ms/33ms ~= 30 */
    timer->start();                         /* 次两句和timer->start(33)效果是一样的 */

    //showpw clear
    ui->showPw->clear();
    getPasswd.clear();
    ui->light->clear();
    i2cvalue = 0;

    realPasswd.push_back(1);
    realPasswd.push_back(2);
    realPasswd.push_back(3);
    realPasswd.push_back(4);

    addForm = new form_addFace();
}

MainWindow::~MainWindow()
{
    stop_capture();
    release_memery();
    close_device();
    fclose(yuv_file);
    errno_exit("however exit......");
    delete ui;
}
// 检测文件是否存在，不存在则创建
void MainWindow::isfileexist()
{
    if(!file->exists()){
        file->open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text);
    }else{
        file->open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text);
    }
}
//向文件中写入内容
void MainWindow::recordLog(QString info)
{
    dateTime = QDateTime::currentDateTime();
    strDateTime = dateTime.toString("yyyy-mm-dd hh:mm:ss");
    isfileexist();
    QTextStream out(file);
    out << "[" << strDateTime << "] " << info << endl;
    file->close();
}

//start V4L2 programing
//打开设备.only open device
void MainWindow::open_device()
{
    recordLog("start to init Camera device......");
    fd = open(device_name,O_RDWR);     /* open camera device */
    if(fd==-1){
        errno_exit("打开失败！");
    }
}
//初始化设备.set frame format
void MainWindow::init_device()
{
    int returnValue;
    // print usb cam feature
    returnValue = ioctl(fd,VIDIOC_QUERYCAP,&cap);
    /* my device value(cap.capabilities) :4000001 */
    if(returnValue==-1){
        recordLog("ioctl(fd, VIDIOC_QUERYCAP, &cap) return value: "+QString::number(returnValue,10).toUpper());
    }else{
        recordLog("ioctl(fd, VIDIOC_QUERYCAP, &cap) return value: "+QString::number(returnValue,10).toUpper());
        recordLog("cap.capabilities value: "+QString::number(cap.capabilities,16).toUpper());
    }
    //set usb camera format
    CLEAR(fmt);
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;			//设置设备类型——视频捕捉<——usb摄像头
    //设置像素大小
    fmt.fmt.pix.width = 320;
    fmt.fmt.pix.height = 240;
    //设置设备捕获图像格式
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    //该设置不清楚，是一种参数
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    //功能：VIDIOS_S_FMT和fmt是同类型
    // VIDIOC_S_FMT命令: 设置捕获视频的格式
    returnValue = ioctl(fd,VIDIOC_S_FMT,&fmt);
    if(returnValue == -1){
        errno_exit("return value wrong: "+QString::number(returnValue,10).toUpper());
    }else{
        recordLog("ioctl(fd, VIDIOC_S_FMT, &fmt) return value: "+QString::number(returnValue,10).toUpper());
    }
    recordLog("init device finish!!!!!!");
}
//申请内存.
void MainWindow::init_mmap()
{
    int returnValue;
    //int i;
    struct v4l2_requestbuffers req;		//内存映射
    /* struct v4l2_requestbuffers req;结构体初始化,分配内存 */
    recordLog("start to request memery......");

    //we will request memery by memery mapping
    CLEAR(req);
    //these three parameters must be setten by application/by programmer
    //申请一个拥有四个缓冲帧的缓冲区       ****@@@@****
    req.count = count;      /* 最大是5 */
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    //VIDIOC_REQBUFS是命令的意思，可以解释为传命令要设置某项参数，req解释为该项参数具体是多少,then give then to function
    //VIDIOC_REQBUFS命令：向驱动提出申请内存的请求
    returnValue = ioctl(fd, VIDIOC_REQBUFS, &req);
    if(returnValue == -1){
        errno_exit("ioctl(fd, VIDIOC_REQBUFS, &req) return value wrong: "+QString::number(returnValue,10).toUpper());
    }else{
        recordLog("ioctl(fd, VIDIOC_REQBUFS, &req) return value :"+QString::number(returnValue,10).toUpper());
    }
    //request memery finish

    // start to mmap
    /* calloc函数原型：void *calloc(size_t n, size_t size)；
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
            /* VIDIOC_QUERYBUF: 获取缓冲帧的地址 */
            ioctl(fd, VIDIOC_QUERYBUF, &buf);
            buffers[n_buffers].length = buf.length;
            /* 把申请的内存映射到buffers数组 */
            buffers[n_buffers].start = (unsigned char *)mmap(NULL,
                                                             buf.length,
                                                             PROT_READ | PROT_WRITE ,
                                                             MAP_SHARED ,
                                                             fd,
                                                             buf.m.offset);
            /* 将缓冲帧放入队列.这有个问题，这些buf 看起来和前面申请的buf 没什么关系，为什么呢? */
            struct v4l2_buffer queuebuffer;
            CLEAR(queuebuffer);
            queuebuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            queuebuffer.memory = V4L2_MEMORY_MMAP;
            queuebuffer.index = n_buffers;
            ioctl(fd, VIDIOC_QBUF, &queuebuffer);
        }
//        enqueue.type = V4L2_BUF_TYPE_VIDEO_CAPTURE ;
//        dequeue.type = V4L2_BUF_TYPE_VIDEO_CAPTURE ;
//        enqueue.memory = V4L2_MEMORY_MMAP ;
//        dequeue.memory = V4L2_MEMORY_MMAP ;
    }
    recordLog("init mmap finish!!!");
}
//开启捕获
void MainWindow::start_capture()
{
    int returnValue;
    enum v4l2_buf_type type;    /* data stream type-数据流类型 */
    recordLog("n_burrers value: "+QString::number(n_buffers,10).toUpper());
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    returnValue = ioctl(fd, VIDIOC_STREAMON, &type);
    if(returnValue == -1){
        errno_exit("ioctl(fd, VIDIOC_STREAMON, &type) return value wrong: "+QString::number(returnValue,10).toUpper());
    }else{
        recordLog("ioctl(fd, VIDIOC_STREAMON, &type) return value :"+QString::number(returnValue,10).toUpper());
        recordLog("capture start......");
    }
    format_thread = new yuv2rgb24(buffers,fd);
    connect(format_thread,SIGNAL(signal_sendQImg(QImage)),this,SLOT(slots_play1Frame(QImage)));
    format_thread->start();
}
//读取一帧 read frame and free frame
void MainWindow::read_frame()
{
    //start thread in a timer, oh my god
//    format_thread = new yuv2rgb24(buffers,fd);
//    connect(format_thread,SIGNAL(signal_sendQImg(QImage)),this,SLOT(slots_play1Frame(QImage)));
//    format_thread->start();
    //format_thread->one_Frame();
    //format_thread->start();
}

//this is unuseful
void MainWindow::process_image(unsigned char *p, int size, FILE *yuv_file){
    recordLog("image size(xs number): "+QString::number(size,10).toUpper());

    recordLog("start convertion......");
    //  fwrite(p,size, 1, yuv_file);
    recordLog("yuv write finish......");
    unsigned char* value = yuyv_2_rgb24(p,size);
    QImage img(value,320,240,QImage::Format_RGB888);
    ui->label->setPixmap(QPixmap::fromImage(currentFrame));
    currentFrame.save("image.bmp");
    recordLog("finish convertion......");
    //mydevice->i2c2led8();
}

//this is unuseful
unsigned char MainWindow::judge(unsigned char num){
    if(num<0)
        return 0;
    else if(num>255)
        return 255;
    else
        return num;
}

//yuyv2rgb24
unsigned char* MainWindow::yuyv_2_rgb24(unsigned char *YUY2buff, int count){
    //recordLog("开始转换......");
    output.clear();
    int dwSize = count;
    recordLog("count value: "+QString::number(count).toUpper());
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
    //recordLog("转换完成......");
    //recordLog("byte_array.count: "+QString::number(output.count()).toUpper());
    unsigned char *c = (unsigned char *)output.data();
    return c;
}

void MainWindow::errno_exit(QString info)
{
    recordLog(info);
    exit(EXIT_FAILURE);
}

void MainWindow::close_device(){
    ::close(fd);
    recordLog("close device......");
}


void MainWindow::on_pushButton_clicked()
{
//    vector<Mat> images;
//    vector<int> labels;
//    // images for first person
//    images.push_back(imread("./facedb/MyFcae0.jpg", CV_LOAD_IMAGE_GRAYSCALE));
//    labels.push_back(0);
//    // images for second person
//    images.push_back(imread("./facedb/MyFcae1.jpg", CV_LOAD_IMAGE_GRAYSCALE));
//    labels.push_back(1);

//    //    recordLog("iamges.size(): "+QString::number(images.size(),10).toUpper());

//    //    recordLog("labels[0].depth: "+QString::number(labels[0],10).toUpper());
//    //    recordLog("labels[1].depth: "+QString::number(labels[1],10).toUpper());
//    //    QImage imgcv = face.cvMat2QImage(images[1]);
//    //    ui->label->setPixmap(QPixmap::fromImage(imgcv));
//    Ptr<FaceRecognizer> model = createFisherFaceRecognizer();
//    model->train(images, labels);
//    recordLog("step train......");

//    Mat imgdetect = imread("img1.bmp", CV_LOAD_IMAGE_GRAYSCALE);
//    cv::resize(imgdetect,imgdetect,Size(92, 113));
//    int predicted = model->predict(imgdetect);
//    recordLog("predicted value: "+QString::number(predicted,10).toUpper());
}

void MainWindow::release_memery(){
    unsigned int i;
    for (i = 0; i < n_buffers; ++i)
    {
        munmap(buffers[i].start, buffers[i].length);
    }
    free(buffers);
    recordLog("memery release finish......");
}

//stop capture
void MainWindow::stop_capture(){
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

//slot function: to update frame on label and save it on currentFrame avoid using by button enable
void MainWindow::slots_play1Frame(QImage imgFromThread){
    currentFrame = imgFromThread;
    ui->label->setPixmap(QPixmap::fromImage(currentFrame));
}

void MainWindow::getMessage(int x){
    //99 100,仅作为提示
    switch (x){
    case 99:
        ui->notice->setFont(QFont("Timers" , 5 ,  QFont::Bold));
        ui->notice->setText("please input passwd......");break;
    case 100:
        ui->notice->setText("finish input passwd......");break;
    default:
        getPasswd.push_back(x);
        ui->showPw->setCursorPosition(0);
        //追加显示
        ui->showPw->insert(QString::number(x,10).toUpper());
        break;
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if(getPasswd == realPasswd){
        ui->notice->clear();
        ui->notice->setText("open door......");
        ui->notice->append("the door will close in 3s......");
        mydevice->pwm_init();
        getPasswd.clear();
        ui->showPw->clear();
    }
}

// cancel button
void MainWindow::on_pushButton_3_clicked()
{
    ui->notice->clear();
    //ui->showPw->clear();
}

//UI get_face button
void MainWindow::on_pushButton_4_clicked()
{
    getFace = currentFrame;
    getFace.save(getfacepath);

    //ui->label_9->setPixmap(QPixmap::fromImage(getFace));
    //face.getface(getfacepath);
    string str = getfacepath.toStdString();
    mat = imread("./getface.bmp");
    //imshow("test",mat);
    QImage imgcv = face.cvMat2QImage(face.detectAndDraw( mat, cascade, nestedCascade,2,1));
    ui->label_9->setPixmap(QPixmap::fromImage(imgcv));
    //  ui->label_9->setText(getfacepath);
    emit signal_sendImg(imgcv);

    // face & location transform
    addForm->face = face.faces;
    addForm->faceImg = imgcv;
    addForm->face1 = mat;
}

//timer slots：IIC2led and label
void MainWindow::timerReadFrame(){
    if(myled8->led8_init()>50){
        ui->light->setText("good evening...");
    }else{
        ui->light->setText("good morning...");
    }
    ui->label_2->setText(QString::number(myled8->led8_init(),10).toUpper());
}

// slot function get i2c value
void MainWindow::getI2cvalue(unsigned char x){
    ui->light->setText(QString::number(x,10).toUpper());
}

void MainWindow::on_pushButton_5_clicked()
{
    delete ui;
}

void MainWindow::on_pushButton_7_clicked()
{
    FILE * imgListFile;
    int temp=0;
    char name[512];
    QImage img;
    nearest = face.recognize();
    if(nearest != -1){
        if(!(imgListFile = fopen("train.txt", "r")))
        {
            fprintf(stderr, "Can\'t open file %s\n", "train.txt");
            qDebug()<<"can't open file";
            return;
        }else{
            qDebug()<<"file open seccessfully!";
        }
        while(!feof(imgListFile)){
            fscanf(imgListFile,"%d %s", &temp, name);
            qDebug()<<"temp value: "<<temp <<"neart value: "<<nearest;
            if(temp == nearest){
                qDebug()<<"temp value: "<<temp << "name: "<<name;
                img.load(name);
                ui->label_3->setPixmap(QPixmap::fromImage(img));
                //            Mat mat = imread(name);
                //            imshow("test",mat);
                break;
            }
        }
        ui->label_4->setText("nearest face: "+QString::number(nearest,10).toUpper());
        ui->label_5->setText("Welcome!");
        ui->notice->setText("open door......");
        ui->notice->append("the door will close in 3s......");
        mydevice->pwm_init();
    }
    else{
        //ui->label_4->clear();
        ui->label_5->setText("Notice: stranger!!");
    }
}

//setting form
void MainWindow::on_pushButton_6_clicked()
{
    //face.learn();
    addForm->show();
}
