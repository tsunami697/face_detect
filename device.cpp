#include "device.h"

device::device()
{	// pwm key
    myrecord = new record("./process.log");
    key_fd= open("/dev/event0",0);
    pwm_fd = open("/dev/pwm",O_RDWR|O_NDELAY);
}
device::~device(){
    close(pwm_fd);
}

//pwm
int device::pwm_init(){

    freq=2000000;
    ioctl(pwm_fd,1,freq);       //turn right 90
    sleep(3);
    ioctl(pwm_fd,0);            //stop
    sleep(3);
    ioctl(pwm_fd,1,1000000);
    sleep(3);
    ioctl(pwm_fd,0);
}
//thread
void device::run(){
    while(1){
        read_keyvalue();
    }
}
//key
void device::read_keyvalue(){

    count=read(key_fd,&ev_key,sizeof(struct input_event));
//    for(i=0;i<(int)count/sizeof(struct input_event);i++){
//        //myrecord->recordLog("key number: "+QString::number(ev_key.code,10).toUpper());
//    }
    if(ev_key.value == 1){ //bian yan chu fa
        //getpasswd.push_back(ev_key.code);
        //myrecord->recordLog("getpasswd size: "+QString::number(getpasswd.size()).toUpper());
        if(ev_key.code == 1){
            flag = 1;
            myrecord->recordLog("start input passwd......");
            emit sendMessage(99);
        }
        if(ev_key.code == 3){
            flag = 0;
            emit sendMessage(100);
        }

        if(flag){
            if((ev_key.code!=1)&&(ev_key.code!=2)&&(ev_key.code!=3)&&(ev_key.code!=4)){
                getpasswd.push_back(key2num(ev_key.code));
                emit sendMessage(key2num(ev_key.code));
            }
        }else if(flag == 0){
            myrecord->recordLog("finish input passwd......");
        }
    }
}
int device::key2num(int x){
    switch (x){
    case 9:  return 1;
    case 17: return 2;
    case 25: return 3;

    case 10: return 4;
    case 18: return 5;
    case 26: return 6;

    case 11: return 7;
    case 19: return 8;
    case 27: return 9;

    case 20: return 0;
    }
}

