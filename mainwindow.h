#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QByteArray>
#include <QImage>
#include <QPixmap>
#include <QTimer>

#include "stdlib.h"
#include <errno.h>  //new
#include <cv.h>
#include <highgui.h>
#include <opencv.hpp>
#include <core/core.hpp>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <linux/prctl.h>
#include "yuv2rgb24.h"
#include "pre_definition.h"
#include "face_detect.h"
#include "opencv.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include "device.h"
#include <QFont>
#include <QTextCursor>
#include "led8_i2c.h"
#include "form_addface.h"
#define SLAVE_ADDR1 0x48

#define CLEAR(x) memset (&(x), 0, sizeof (x))
struct buffer {
    unsigned char* start;
    size_t length;
};
namespace Ui {
    class MainWindow;
}
using namespace cv;
using namespace std;
class yuv2rgb24;
class device;
class led8_i2c;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //写好了，不要修改
    void isfileexist();
    void recordLog(QString info);

    QImage img;
    Mat mat;
    CascadeClassifier cascade, nestedCascade;
    //i2c
    device *mydevice;

private:
    int count;
    Ui::MainWindow *ui;   
    buffer *buffers;
    const char* device_name;
    int fd;                         //设备号
    int pixel_format;               //视频格式
    int n_buffers;
    struct v4l2_capability cap;     //检测属性
    struct v4l2_format fmt;         //格式设置
    struct v4l2_buffer enqueue,dequeue ;  //定义出入队的操作结构体成员
    unsigned char *oneFrame;
    FILE *yuv_file;
    int x;
    unsigned char frame_buffer[320*240*3];
    //用于操作文件日志，写好了，不要修改
    QString File_ProcessLog;
    QFile *file;
    QDateTime dateTime;
    QString strDateTime;
    QByteArray output;
    QByteArray byte_array;
    yuv2rgb24 *format_thread;
    QImage currentFrame;
    QImage getFace;
    QString getfacepath;
    //key passwd
    vector<int> getPasswd;
    vector<int> realPasswd;
    //i2c
    int i2cvalue;
    led8_i2c *myled8;

    face_detect face;
    int nearest;

    //UV相关操作函数
    void open_device();
    void init_device();
    void init_mmap();
    void start_capture();
    void read_frame();

    void stop_capture();
    void close_device();
    void release_memery();
    void process_image(unsigned char *p, int size, FILE *yuv_file);
    unsigned char* yuyv_2_rgb24(unsigned char *YUY2buff, int count);
    unsigned char judge(unsigned char num);
    //错误处理函数
    void errno_exit(QString info);
    //timer
    QTimer *timer;
    int Setting_frameNumer;

    // add face form
    form_addFace *addForm;
private slots:
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void slots_play1Frame(QImage imgFromThread);
    void getMessage(int x);
    void getI2cvalue(unsigned char x);
    void timerReadFrame();
signals:
    void signal_sendImg(QImage sendImg);
};

#endif // MAINWINDOW_H
