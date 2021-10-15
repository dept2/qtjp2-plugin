#ifndef QTJP2OPENJPEGTEST_H
#define QTJP2OPENJPEGTEST_H

// Qt
#include <QObject>

// OpenJPEG
#include <openjpeg.h>


class QtJP2OpenJPEGTest : public QObject
{
  Q_OBJECT

  private slots:
    void lossless_data();
    void lossless();

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
