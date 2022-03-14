#include "face_detect.h"

face_detect::face_detect()
{
    //count = 0;
    faceImgArr          = 0; // 指向训练人脸和测试人脸的指针（在学习和识别阶段指向不同）
    personNumTruthMat   = 0; // 人脸图像的ID号
    nTrainFaces         = 0; // 训练图像的数目
    nEigens             = 0; // 自己取的主要特征值数目
    pAvgTrainImg        = 0; // 训练人脸数据的平均值
    eigenVectArr        = 0; // 投影矩阵，也即主特征向量
    eigenValMat         = 0; // 特征值
    projectedTrainFaceMat= 0; // 训练图像的投影
}
face_detect::~face_detect(){
    cvReleaseMat(&personNumTruthMat);
}
//检测人脸、画圈
Mat face_detect::detectAndDraw(Mat &img, CascadeClassifier &cascade, CascadeClassifier &nestedCascade, double scale, bool tryflip)
{
    double t = 0;

    //定义一些颜色，用来标示不同的人脸
    const static Scalar colors[] =
    {
        Scalar(255,128,0),
        Scalar(255,0,0),
        Scalar(255,255,0),
        Scalar(0,255,0),
        Scalar(0,128,255),
        Scalar(0,255,255),
        Scalar(0,0,255),
        Scalar(255,0,255)
    };
    Mat gray, smallImg;
    //    gray = img;
    //转成灰度图像，Harr特征基于灰度图
    cvtColor( img, gray, COLOR_BGR2GRAY );
    //改变图像大小，使用双线性差值
    double fx = 1 / scale;
    resize( gray, smallImg, Size(), fx, fx, INTER_LINEAR );
    //变换后的图像进行直方图均值化处理
    equalizeHist( smallImg, smallImg );
    //程序开始和结束插入此函数获取时间，经过计算求得算法执行时间
    t = (double)getTickCount();
    //检测人脸
    //detectMultiScale函数中smallImg表示的是要检测的输入图像为smallImg，faces表示检测到的人脸目标序列，1.1表示
    //每次图像尺寸减小的比例为1.1，2表示每一个目标至少要被检测到3次才算是真的目标(因为周围的像素和不同的窗口大
    //小都可以检测到人脸),CV_HAAR_SCALE_IMAGE表示不是缩放分类器来检测，而是缩放图像，Size(30, 30)为目标的
    //最小最大尺寸
    cascade.detectMultiScale( smallImg, faces,
                              1.1, 2, 0
                              //|CASCADE_FIND_BIGGEST_OBJECT
                              //|CASCADE_DO_ROUGH_SEARCH
                              |CASCADE_SCALE_IMAGE,
                              Size(30, 30) );
    //如果使能，翻转图像继续检测
    if( tryflip )
    {
        flip(smallImg, smallImg, 1);
        cascade.detectMultiScale( smallImg, faces2,/* 1.1, 3, CV_HAAR_DO_ROUGH_SEARCH, Size(50, 50));*/
                                  1.1, 2, 0
                                  //|CASCADE_FIND_BIGGEST_OBJECT
                                  //|CASCADE_DO_ROUGH_SEARCH
                                  |CASCADE_SCALE_IMAGE,
                                  Size(30, 30));
    }
    t = (double)getTickCount() - t;
    //string str = facePath.toStdString();
    Mat mat1 = imread("/root/qtProBin/getface.bmp");
    for ( size_t i = 0; i < faces2.size(); i++ )
    {
        //保存人脸
        Mat image_cut=img(Rect(faces2[i].x*2, faces2[i].y*2, faces2[i].width*scale, faces2[i].height*scale));
        resize(image_cut, image_cut, Size(92, 113));
        cvtColor( image_cut, image_cut, COLOR_BGR2GRAY );
        //string  str = format("/root/qtProBin/facesTest/MyFace%d.bmp", i);  //success
        string  str = format("/root/qtProBin/facesTest/test.bmp", i);  //success
        imwrite(str, image_cut);
        //画方框
        rectangle(mat1 , Rect(faces2[i].x*2, faces2[i].y*2, faces2[i].width*scale, faces2[i].height*scale), Scalar(0, 0, 255), 3);
    }
    return mat1;
}
//bgr888->mat
Mat face_detect::QImage2cvMat(QImage image)
{
    cv::Mat mat;
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.bits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.bits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_RGB2BGR);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.bits(), image.bytesPerLine());
        break;
    }
    return mat;
}

QImage face_detect::cvMat2QImage(const Mat& mat){
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        //printf("set colors\n");
        image.setNumColors(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        //qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        //qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

void face_detect::getface(QString facepath){
    facePath = facepath;
}

//学习阶段代码
void face_detect::learn(){
    int i, offset;

    //加载训练图像集
    nTrainFaces = loadFaceImgArray("./train.txt");
    if( nTrainFaces < 2 )
    {
        fprintf(stderr,
                "Need 2 or more training faces\n"
                "Input file contains only %d\n", nTrainFaces);
        return;
    }
    printf("nTrainFaces = %d",nTrainFaces);
    // 进行主成分分析
    doPCA();
    //将训练图集投影到子空间中
    projectedTrainFaceMat = cvCreateMat( nTrainFaces, nEigens, CV_32FC1 );
    offset = projectedTrainFaceMat->step / sizeof(float);
    for(i=0; i<nTrainFaces; i++)
    {
        //int offset = i * nEigens;
        cvEigenDecomposite(
                faceImgArr[i],
                nEigens,
                eigenVectArr,
                0, 0,
                pAvgTrainImg,
                //projectedTrainFaceMat->data.fl + i*nEigens);
                projectedTrainFaceMat->data.fl + i*offset);
    }
    //将训练阶段得到的特征值，投影矩阵等数据存为.xml文件，以备测试时使用
    storeTrainingData();
}

//识别阶段代码
int face_detect::recognize(){
    int i, nTestFaces  = 0;         // 测试人脸数
    CvMat * trainPersonNumMat = 0;  // 训练阶段的人脸数
    float * projectedTestFace = 0;
    int near;
    // 加载测试图像，并返回测试人脸数
    nTestFaces = loadFaceImgArray("./test.txt");
    printf("%d test faces loaded\n", nTestFaces);

    // 加载保存在.xml文件中的训练结果
    if( !loadTrainingData( &trainPersonNumMat ) ) return 0;

    //
    projectedTestFace = (float *)cvAlloc( nEigens*sizeof(float) );
    for(i=0; i<nTestFaces; i++)
    {
        int iNearest, nearest, truth;

        //将测试图像投影到子空间中
        cvEigenDecomposite(
                faceImgArr[i],
                nEigens,
                eigenVectArr,
                0, 0,
                pAvgTrainImg,
                projectedTestFace);

        iNearest = findNearestNeighbor(projectedTestFace);
        if(iNearest == -1){
            qDebug()<<"don't have this face!";
            near = -1;
        }else{
            truth    = personNumTruthMat->data.i[i];
            nearest  = trainPersonNumMat->data.i[iNearest];
            near = nearest;
            qDebug()<<"nearest = "<< nearest <<"Truth = "<< truth;
            //printf("nearest = %d, Truth = %d\n", nearest, truth);
        }
    }
    return near;
    //    truth    = personNumTruthMat->data.i[i];
    //    nearest  = trainPersonNumMat->data.i[iNearest];
    //qDebug()<<"nearest = "+QString::number(nearest,10).toUpper()+", Truth = "+QString::number(truth,10).toUpper();
    //printf("nearest = %d, Truth = %d\n", nearest, truth);
}

//主成分分析
void face_detect::doPCA(){
    int i;
    CvTermCriteria calcLimit;
    //CvSize 描述矩阵的尺寸
    CvSize faceImgSize;

    // 自己设置主特征值个数
    nEigens = nTrainFaces-1;

    //分配特征向量存储空间
    faceImgSize.width  = faceImgArr[0]->width;
    faceImgSize.height = faceImgArr[0]->height;
    //eigenVectArr 投影矩阵
    eigenVectArr = (IplImage**)cvAlloc(sizeof(IplImage*) * nEigens);    //分配个数为主特征值个数
    for(i=0; i<nEigens; i++)
        eigenVectArr[i] = cvCreateImage(faceImgSize, IPL_DEPTH_32F, 1);

    //分配 主特征值 存储空间
    eigenValMat = cvCreateMat( 1, nEigens, CV_32FC1 );

    // 分配平均图像存储空间
    pAvgTrainImg = cvCreateImage(faceImgSize, IPL_DEPTH_32F, 1);

    // 设定PCA分析结束条件
    calcLimit = cvTermCriteria( CV_TERMCRIT_ITER, nEigens, 1);

    // 计算平均图像，特征值，特征向量
    cvCalcEigenObjects(
            nTrainFaces,
            (void*)faceImgArr,
            (void*)eigenVectArr,
            CV_EIGOBJ_NO_CALLBACK,
            0,
            0,
            &calcLimit,
            pAvgTrainImg,
            eigenValMat->data.fl);

    cvNormalize(eigenValMat, eigenValMat, 1, 0, CV_L1, 0);
    qDebug()<<"【step】: doPCA() finish!";
}

//存储训练结果
void face_detect::storeTrainingData(){
    CvFileStorage * fileStorage;
    int i;

    fileStorage = cvOpenFileStorage( "./facedata.xml", 0, CV_STORAGE_WRITE );

    //存储特征值，投影矩阵，平均矩阵等训练结果
    cvWriteInt( fileStorage, "nEigens", nEigens );
    cvWriteInt( fileStorage, "nTrainFaces", nTrainFaces );
    cvWrite(fileStorage, "trainPersonNumMat", personNumTruthMat, cvAttrList(0,0));
    cvWrite(fileStorage, "eigenValMat", eigenValMat, cvAttrList(0,0));
    cvWrite(fileStorage, "projectedTrainFaceMat", projectedTrainFaceMat, cvAttrList(0,0));
    cvWrite(fileStorage, "avgTrainImg", pAvgTrainImg, cvAttrList(0,0));
    for(i=0; i<nEigens; i++)
    {
        char varname[200];
        sprintf( varname, "eigenVect_%d", i );
        cvWrite(fileStorage, varname, eigenVectArr[i], cvAttrList(0,0));
    }


    cvReleaseFileStorage( &fileStorage );
    qDebug()<<"【step】: storeTrainingData() finish!";

}

//加载保存过的训练结果
int  face_detect::loadTrainingData(CvMat ** pTrainPersonNumMat){
    CvFileStorage * fileStorage;
    int i;

    fileStorage = cvOpenFileStorage( "./facedata.xml", 0, CV_STORAGE_READ );
    if( !fileStorage )
    {
        fprintf(stderr, "Can't open facedata.xml\n");
        return 0;
    }

    nEigens = cvReadIntByName(fileStorage, 0, "nEigens", 0);
    nTrainFaces = cvReadIntByName(fileStorage, 0, "nTrainFaces", 0);
    *pTrainPersonNumMat = (CvMat *)cvReadByName(fileStorage, 0, "trainPersonNumMat", 0);
    eigenValMat  = (CvMat *)cvReadByName(fileStorage, 0, "eigenValMat", 0);
    projectedTrainFaceMat = (CvMat *)cvReadByName(fileStorage, 0, "projectedTrainFaceMat", 0);
    pAvgTrainImg = (IplImage *)cvReadByName(fileStorage, 0, "avgTrainImg", 0);
    eigenVectArr = (IplImage **)cvAlloc(nTrainFaces*sizeof(IplImage *));
    for(i=0; i<nEigens; i++)
    {
        char varname[200];
        sprintf( varname, "eigenVect_%d", i );
        eigenVectArr[i] = (IplImage *)cvReadByName(fileStorage, 0, varname, 0);
    }


    cvReleaseFileStorage( &fileStorage );

    return 1;

}

//寻找最接近的图像
int  face_detect::findNearestNeighbor(float * projectedTestFace){
    currTime1 = QDateTime::currentDateTime();
    getTime1 = currTime1.toString("mm:ss.zzz");
    //    qDebug()<<"currTime: "<<getTime;

    double leastDistSq = DBL_MAX;       //定义最小距离，并初始化为无穷大
    int i, iTrain, iNearest = 0;
    qDebug()<<"leastDistSq: "<<leastDistSq;
    for(iTrain=0; iTrain<nTrainFaces; iTrain++) //nTrainFaces 训练图像数目
    {
        double distSq=0;

        for(i=0; i<nEigens; i++)
        {
            float d_i = projectedTestFace[i] - projectedTrainFaceMat->data.fl[iTrain*nEigens + i];
            distSq += d_i*d_i / eigenValMat->data.fl[i];  // Mahalanobis算法计算的距离:distSq
            //  distSq += d_i*d_i; // Euclidean算法计算的距离
        }
        //        qDebug()<<"eEigens: "+QString::number(nEigens,10).toUpper();
        qDebug()<<"eEigens: "+QString::number(nEigens,10).toUpper()<<"; distSq: "<<distSq <<"; leastDistSq: "<< leastDistSq;
        if(distSq < leastDistSq)
        {
            leastDistSq = distSq;
            iNearest = iTrain;
        }
    }
    currTime2 = QDateTime::currentDateTime();
    getTime2 = currTime2.toString("mm:ss.zzz");
    qDebug()<<"currTime1: "<<getTime1;
    qDebug()<<"currTime2: "<<getTime2;
    //return iNearest;
    if(leastDistSq>1000000&&leastDistSq<5000000){
        qDebug()<<"return iNearest value: "<< iNearest;
        return iNearest;
    }
    else{
        qDebug()<<"return iNearest value:" << QString::number(-1,10).toUpper();
        return -1;
    }
}

//加载txt文件的列举的图像，加载到了faceImgArr数组里
int  face_detect::loadFaceImgArray(char *filename){
    FILE * imgListFile = 0;
    char imgFilename[512];
    int iFace, nFaces=0;

    if( !(imgListFile = fopen(filename, "r")) )
    {
        fprintf(stderr, "Can\'t open file %s\n", filename);
        return 0;
    }else{
        qDebug()<<"file open seccessfully!";
    }

    // 统计人脸数   fgets():从文件结构体指针stream中读取数据，每次读取一行。
    while( fgets(imgFilename, 512, imgListFile) ) ++nFaces;
    rewind(imgListFile);

    // 分配人脸图像存储空间和人脸ID号存储空间
    faceImgArr        = (IplImage **)cvAlloc( nFaces*sizeof(IplImage *) );
    personNumTruthMat = cvCreateMat( 1, nFaces, CV_32SC1 );     //cvMat opencv中的矩阵

    for(iFace=0; iFace<nFaces; iFace++)
    {
        // 从文件中读取序号和人脸名称
        fscanf(imgListFile,
               "%d %s", personNumTruthMat->data.i+iFace, imgFilename);
        qDebug()<<"id: "<<QString::number(*(personNumTruthMat->data.i)+iFace,10).toUpper();
        qDebug()<<imgFilename;
        // 加载人脸图像
        faceImgArr[iFace] = cvLoadImage(imgFilename, CV_LOAD_IMAGE_GRAYSCALE);

        if( !faceImgArr[iFace] )
        {
            fprintf(stderr, "Can\'t load image from %s\n", imgFilename);
            return 0;
        }
    }
    //查看数组里人脸图像，多的就迭代
    //    cvNamedWindow("0");
    //    cvShowImage("0",faceImgArr[0]);
    //    cvNamedWindow("1");
    //    cvShowImage("1",faceImgArr[1]);
    //    cvNamedWindow("2");
    //    cvShowImage("2",faceImgArr[2]);

    fclose(imgListFile);
    return nFaces;
}
