#ifndef HEADERS_H
#define HEADERS_H

#include <QString>
#include <QFile>
#include <QThread>
#include <QObject>
#include <QMainWindow>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QByteArray>
#include <QImage>
#include <QPixmap>

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
#include "recordLog.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))
struct buffer
{
    unsigned char *start;
    size_t length;
};

#endif // HEADERS_H
