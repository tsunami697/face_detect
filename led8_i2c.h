#ifndef LED8_I2C_H
#define LED8_I2C_H
#include <QThread>
#include "device.h"
#include <QObject>
#include "mainwindow.h"
#define SLAVE_ADDR1 0x48
class led8_i2c:public QThread
{
    //Q_OBJECT
public:
    led8_i2c();
    ~led8_i2c();
    int led8_init();
private:
    int i2c_init();
    int i2c_read();

    vector<int> i2cValueProcess(int);

    int i2c_fd;
    int led8_fd;
    int i;
    vector<int> i2c2led8(int x);
    unsigned char addr,addr1[1];
    int data1;
protected:
    void run();
signals:
    void sendI2cvalue(int x);
};

#endif // LED8_I2C_H
