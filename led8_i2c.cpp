#include "led8_i2c.h"

led8_i2c::led8_i2c()
{
    //IIC 数码管
    led8_fd = open("/dev/led8",O_RDWR|O_NDELAY);
    i2c_fd = open("/dev/i2c-0",O_RDWR);
    i2c_init();
}
led8_i2c::~led8_i2c()
{
    close(led8_fd);
    close(i2c_fd);
}
void led8_i2c::run(){
    while(1){
        //emit sendI2cvalue(led8_init());
        led8_init();
    }
}

//该函数即完成对数码管的显示，又完成lable显示
int led8_i2c::led8_init(){
    vector<int> temp = i2c2led8(i2c_read());
    char buf[4]={6,6,6,6};
    buf[0] = 0;			//最高为不会到千位，所以直接置0
    buf[1] = temp[0];
    buf[2] = temp[1];
    buf[3] = temp[2];
    if(!led8_fd){
        exit(1);
    }else{
        write (led8_fd, buf, 4);	//写led
        return i2c_read();			//返回读到的值，给label再显示
    }
}
int led8_i2c::i2c_init(){
    if(!i2c_fd){
        exit(1);
    }else{
        return 1;
    }
}

int led8_i2c::i2c_read(){
    i = ioctl(i2c_fd,I2C_SLAVE,SLAVE_ADDR1);
    addr1[0]= 0x40;
    //读数据之前，要像第六位写1
    write(i2c_fd,addr1,1);
    //读数据
    read(i2c_fd,&data1,1);
    return data1;
}

//将IIC取到的值转为可以让led显示的数据格式
vector<int> led8_i2c::i2c2led8(int x){
    vector<int> i2cvalue;
    if(x/100==0)			//取百位放入容器
        i2cvalue.push_back(0);
    else
        i2cvalue.push_back((x/100));
    if((x%100/10)==0)		//取十位放入容器
        i2cvalue.push_back(0);
    else
        i2cvalue.push_back((x%100/10));
    if(x%10==0)				//取个位放入容器
        i2cvalue.push_back(0);
    else
        i2cvalue.push_back((x%10));
    return i2cvalue;		//返回容器
}
