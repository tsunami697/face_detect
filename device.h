/***************************************************
 *
 * 功能：pwm、key 操作类
 *
 * ************************************************/
#ifndef DEVICE_H
#define DEVICE_H
//I2C module
#include "i2c-dev.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <QObject>
#include <QThread>
#include "record.h"
#include "linux/input.h"
#include "face_detect.h"
//#define SLAVE_ADDR1 0x48
using namespace std;
class device : public QThread
{
    Q_OBJECT
public:
    device();
    ~device();
    void read_keyvalue();
    int pwm_init();
    int key2num(int x);

    //QByteArray getpasswd;
    vector<int> getpasswd;
    int pwm_fd;
    //key
    int key_fd;
    int count;
    struct input_event ev_key;
    int flag;
    long freq;
    int i;
    record *myrecord;
protected:
    void run();
signals:
    void sendMessage(int x);
    void sendI2cvalue(unsigned char x);

};

#endif // DEVICE_H
