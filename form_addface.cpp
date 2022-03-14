#include "form_addface.h"
#include "ui_form_addface.h"
#include "mainwindow.h"
form_addFace::form_addFace(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::form_addFace)
{
    ui->setupUi(this);    
    MainWindow  *mw;
    connect(mw,SIGNAL(signal_sendImg(QImage)),this,SLOT(slot_getImage(QImage)));
    temp = 4;    
}

form_addFace::~form_addFace()
{
    delete ui;
}

//slot function
void form_addFace::slot_getImage(QImage img){
    faceImg = img;
}

//get picture
void form_addFace::on_pushButton_clicked()
{
    ui->label->setPixmap(QPixmap::fromImage(faceImg));
    //保存人脸
    Mat image_cut=face1(Rect(face[0].x*2, face[0].y*2, face[0].width*2, face[0].height*2));
    cv::resize(image_cut, image_cut, Size(92, 113));
    cvtColor( image_cut, image_cut, COLOR_BGR2GRAY );
    string  str = format("./facesDB/MyFace%d.bmp",temp);                     //success
    imwrite(str, image_cut);
    QImage img;
    QString path;
    path = "./facesDB/MyFace"+QString::number(temp,10).toUpper()+".bmp";
    QMessageBox::information(NULL,"notice！",QString::number(temp,10).toUpper(),
                             QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
    img.load(path);
    ui->label_2->setPixmap(QPixmap::fromImage(img));
    ui->label_3->setText(path);
    temp++;
}

//add face
void form_addFace::on_pushButton_2_clicked()
{
    QStringList string_list;
    //获取faceDB中的.bmp文件名字
    QDir dir("./facesDB/");
    if(!dir.exists()){
        QMessageBox::information(NULL,"警告！","faceDB文件夹不存在！",
                                 QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
    }else{
        //查看路径中后缀为.bmp格式的文件
        QStringList filters;
        filters<<QString("*.bmp");
        //设置类型过滤器，只为文件格式
        dir.setFilter(QDir::Files|QDir::NoSymLinks);
        //设置文件名称过滤器，只为filters格式
        dir.setNameFilters(filters);
        //统计.bmp格式的文件数目
        int dir_count = dir.count();
        if(dir_count<=0){
            QMessageBox::information(NULL,"警告！","文件数目为0",
                                     QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        }else{
            for(int j=0; j<dir_count; ++j){
                QString file_name = dir[j];
                string_list.append(file_name);
            }
        }
        QString filename = "./train.txt";        //当前文件夹中
        QFile file(filename);
        if(!file.open(QIODevice::ReadWrite|QIODevice::Text)){
            QMessageBox::warning(this,"file write","can't open",QMessageBox::Yes);
        }
        QTextStream in(&file);
        QString text;
        for(int x=0;x<string_list.count();++x){
            text = QString::number((x+1),10).toUpper()+" "+"./facesDB/"+string_list.value(x)+"\n";
            in<<text;
            qDebug()<<text;
        }
        //ui->label_2.setPixmap(QImage::fromPixmap());
        file.close();
        faceDetect.learn();
        //learn();
    }
}
