#ifndef QTJP2OPENJPEGTEST_H
#define QTJP2OPENJPEGTEST_H

// Qt
#include <QObject>

// OpenJPEG
#include <openjpeg-2.3/openjpeg.h>


class QtJP2OpenJPEGTest : public QObject
{
  Q_OBJECT


  private slots:
    void pngLossless();
    void bmpLossless();
    void pngAlphaLossless();
    void pngGrayLossless();
    void pngGrayAlphaLossless();
    void pngGrayAlpha8();

    void grayJP();
    void testJK();
    void colorJP();
    void coloralphaJP();
    void test1JP();
    void test2JP();
    void test1JK();
    void test2JK();
};


#endif // QTJP2OPENJPEGTEST_H
