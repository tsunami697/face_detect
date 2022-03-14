#ifndef FACE_DETECT_H
#define FACE_DETECT_H
#include "opencv.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include "cvaux.h"
#include "cxcore.h"

#include <QImage>
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QDebug>
using namespace std;
using namespace cv;
class face_detect
{
    //Q_OBJECT
public:
    face_detect();
    ~face_detect();
    Mat detectAndDraw(Mat& img,CascadeClassifier& cascade,
                      CascadeClassifier& nestedCascade,
                      double scal,bool tryflip);
    Mat QImage2cvMat(QImage image);
    CascadeClassifier cascade,nestedCascade;
    QImage cvMat2QImage(const Mat& mat);
    void getface(QString facepath);

    //int count;
    //Ptr <FaceRecognizer> modelPCA = createEigenFaceRecognizer();

    //face detect series func & variable
    //定义几个重要的全局变量
    IplImage ** faceImgArr;        //= 0; // 指向训练人脸和测试人脸的指针（在学习和识别阶段指向不同）
    CvMat    *  personNumTruthMat; //= 0; // 人脸图像的ID号
    int nTrainFaces;               //= 0; // 训练图像的数目
    int nEigens;                   //= 0; // 自己取的主要特征值数目
    IplImage * pAvgTrainImg;       //= 0; // 训练人脸数据的平均值
    IplImage ** eigenVectArr;      //= 0; // 投影矩阵，也即主特征向量
    CvMat * eigenValMat;           //= 0; // 特征值
    CvMat * projectedTrainFaceMat; //= 0; // 训练图像的投影
    QDateTime currTime1;
    QDateTime currTime2;
    QString getTime1;
    QString getTime2;

    //建立用于存放人脸的向量容器
    vector<Rect> faces, faces2;
    // 函数原型
    void learn();
    int recognize();
    void doPCA();
    void storeTrainingData();
    int  loadTrainingData(CvMat ** pTrainPersonNumMat);
    int  findNearestNeighbor(float * projectedTestFace);
    int  loadFaceImgArray(char *filename);
private:
    QString facePath;
};

#endif // FACE_DETECT_H
