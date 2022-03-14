#ifndef FORM_ADDFACE_H
#define FORM_ADDFACE_H

#include <QDialog>
#include <QMessageBox>
#include "face_detect.h"
#include "QDir"
namespace Ui {
    class form_addFace;
}
using namespace cv;
using namespace std;
class form_addFace : public QDialog
{
    Q_OBJECT

public:
    explicit form_addFace(QWidget *parent = 0);
    ~form_addFace();
    QImage faceImg;
    //建立用于存放人脸的向量容器
    vector<Rect> face;
    Mat face1;
private:
    Ui::form_addFace *ui;
    int temp;
    face_detect faceDetect;
private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void slot_getImage(QImage img);
};

#endif // FORM_ADDFACE_H
