#-------------------------------------------------
#
# Project created by QtCreator 2017-11-02T12:48:34
#
#-------------------------------------------------

QT       += core gui

TARGET = facedetect
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    yuv2rgb24.cpp \
    face_detect.cpp \
    device.cpp \
    record.cpp \
    led8_i2c.cpp \
    form_addface.cpp

HEADERS  += mainwindow.h \
    yuv2rgb24.h \
    pre_definition.h \
    face_detect.h \
    device.h \
    record.h \
    led8_i2c.h \
    form_addface.h

FORMS    += mainwindow.ui \
    form_addface.ui

INCLUDEPATH += INCLUDEPATH += /usr/local/qwt-6.0.1-arm/include/
INCLUDEPATH += /usr/local/qwt-6.0.1-arm/include/
LIBS += -L"/usr/local/qwt-6.0.1-arm/lib/" -lqwt

INCLUDEPATH += /usr/local/arm/lib/opencv/include/
INCLUDEPATH += /usr/local/arm/lib/opencv/include/opencv/
INCLUDEPATH += /usr/local/arm/lib/opencv/include/opencv2/
INCLUDEPATH += /mnt/hgfs/16-17-3/ubuntuOpencvU4l2/facedetect/
LIBS += /usr/local/arm/lib/opencv/lib/*.so

OTHER_FILES +=

RESOURCES += \
    rc.qrc
