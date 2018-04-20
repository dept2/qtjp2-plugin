// Local
#include "QtJP2OpenJPEGTest.h"

// Qt test
#include <QtTest/QtTest>

// Qt
#include <QImage>


void QtJP2OpenJPEGTest::bmpLossless()
{
  QImage originalImage(":/test_rome.bmp");
  QVERIFY(!originalImage.isNull());

  QByteArray convertedArray;
  QBuffer buffer(&convertedArray);
  buffer.open(QIODevice::WriteOnly);
  QImage convertedImage;
  QBENCHMARK
  {
    originalImage.save(&buffer, "jp2", 100);
  }

  buffer.close();

  qWarning() << "Size" << convertedArray.size();
  convertedImage.loadFromData(convertedArray);

  QCOMPARE(originalImage.size(), convertedImage.size());
  for (int i = 0; i < originalImage.height(); ++i)
    for (int j = 0; j < originalImage.width(); ++j)
    {
      QRgb original = originalImage.pixel(j, i);
      QRgb converted = convertedImage.pixel(j, i);
      QCOMPARE(qRed(original), qRed(converted));
      QCOMPARE(qGreen(original), qGreen(converted));
      QCOMPARE(qBlue(original), qBlue(converted));
    }
}


void QtJP2OpenJPEGTest::pngLossless()
{
  QImage originalImage(":/test_color16.png");
  QVERIFY(!originalImage.isNull());

  QByteArray convertedArray;
  QBuffer buffer(&convertedArray);
  buffer.open(QIODevice::WriteOnly);
  QImage convertedImage;
  QBENCHMARK
  {
    originalImage.save(&buffer, "jp2", 100);


    buffer.close();

    qWarning() << "Size" << convertedArray.size() << buffer.size();
    convertedImage.loadFromData(convertedArray);

    QCOMPARE(originalImage.size(), convertedImage.size());
    for (int i = 0; i < originalImage.height(); ++i)
      for (int j = 0; j < originalImage.width(); ++j)
      {
        QRgb original = originalImage.pixel(j, i);
        QRgb converted = convertedImage.pixel(j, i);
        QCOMPARE(qRed(original), qRed(converted));
        QCOMPARE(qGreen(original), qGreen(converted));
        QCOMPARE(qBlue(original), qBlue(converted));
      }
  }
}


void QtJP2OpenJPEGTest::pngAlphaLossless()
{
  QImage originalImage(":/test_coloralpha.png");
  QVERIFY(!originalImage.isNull());

  QByteArray convertedArray;
  QBuffer buffer(&convertedArray);
  buffer.open(QIODevice::WriteOnly);
  QImage convertedImage;
  QBENCHMARK
  {
    originalImage.save(&buffer, "jp2", 100);
  }

  buffer.close();

  qWarning() << "Size" << convertedArray.size();
  convertedImage.loadFromData(convertedArray);

  QCOMPARE(originalImage.size(), convertedImage.size());
  for (int i = 0; i < originalImage.height(); ++i)
    for (int j = 0; j < originalImage.width(); ++j)
    {
      QRgb original = originalImage.pixel(j, i);
      QRgb converted = convertedImage.pixel(j, i);
      QCOMPARE(qRed(original), qRed(converted));
      QCOMPARE(qGreen(original), qGreen(converted));
      QCOMPARE(qBlue(original), qBlue(converted));
    }
}


void QtJP2OpenJPEGTest::pngGrayLossless()
{
  QImage originalImage(":/test_gray1.png");
  QVERIFY(!originalImage.isNull());

  QByteArray convertedArray;
  QBuffer buffer(&convertedArray);
  buffer.open(QIODevice::WriteOnly);
  QImage convertedImage;
  QBENCHMARK
  {
    originalImage.save(&buffer, "jp2", 100);


    buffer.close();

    qWarning() << "Size" << convertedArray.size();
    convertedImage.loadFromData(convertedArray);

    QCOMPARE(originalImage.size(), convertedImage.size());
    for (int i = 0; i < originalImage.height(); ++i)
      for (int j = 0; j < originalImage.width(); ++j)
      {
        QRgb original = originalImage.pixel(j, i);
        QRgb converted = convertedImage.pixel(j, i);
        QCOMPARE(qRed(original), qRed(converted));
        QCOMPARE(qGreen(original), qGreen(converted));
        QCOMPARE(qBlue(original), qBlue(converted));
      }
  }
}


void QtJP2OpenJPEGTest::pngGrayAlphaLossless()
{
  QImage originalImage(":/test_graya16.png");
  QVERIFY(!originalImage.isNull());

  QByteArray convertedArray;
  QBuffer buffer(&convertedArray);
  buffer.open(QIODevice::WriteOnly);
  QImage convertedImage;
  QBENCHMARK
  {
    originalImage.save(&buffer, "jp2", 100);


    buffer.close();

    qWarning() << "Size" << convertedArray.size();
    convertedImage.loadFromData(convertedArray);

    QCOMPARE(originalImage.size(), convertedImage.size());
    for (int i = 0; i < originalImage.height(); ++i)
      for (int j = 0; j < originalImage.width(); ++j)
      {
        QRgb original = originalImage.pixel(j, i);
        QRgb converted = convertedImage.pixel(j, i);
        QCOMPARE(qRed(original), qRed(converted));
        QCOMPARE(qGreen(original), qGreen(converted));
        QCOMPARE(qBlue(original), qBlue(converted));
      }
  }
}


void QtJP2OpenJPEGTest::pngGrayAlpha8()
{
  QImage originalImage(":/test_graya8.png");
  QVERIFY(!originalImage.isNull());

  QByteArray convertedArray;
  QBuffer buffer(&convertedArray);
  buffer.open(QIODevice::WriteOnly);
  originalImage.save(&buffer, "jp2", 100);
  buffer.close();

  QImage convertedImage;
  convertedImage.loadFromData(convertedArray, "jp2");

  QCOMPARE(originalImage.size(), convertedImage.size());
  for (int i = 0; i < originalImage.height(); ++i)
    for (int j = 0; j < originalImage.width(); ++j)
    {
      QRgb original = originalImage.pixel(j, i);
      QRgb converted = convertedImage.pixel(j, i);
      QCOMPARE(qRed(original), qRed(converted));
      QCOMPARE(qGreen(original), qGreen(converted));
      QCOMPARE(qBlue(original), qBlue(converted));
    }
}

void QtJP2OpenJPEGTest::grayJP()
{
  QImage originalImage(":/test_blackcat.jp2");
  QVERIFY(!originalImage.isNull());

  QByteArray convertedArray;
  QBuffer buffer(&convertedArray);
  buffer.open(QIODevice::WriteOnly);
  originalImage.save(&buffer, "png", 100);
  buffer.close();

  QImage convertedImage;
  convertedImage.loadFromData(convertedArray);

  QCOMPARE(originalImage.size(), convertedImage.size());
  for (int i = 0; i < originalImage.height(); ++i)
    for (int j = 0; j < originalImage.width(); ++j)
    {
      QRgb original = originalImage.pixel(j, i);
      QRgb converted = convertedImage.pixel(j, i);
      QCOMPARE(qRed(original), qRed(converted));
      QCOMPARE(qGreen(original), qGreen(converted));
      QCOMPARE(qBlue(original), qBlue(converted));
    }
}


void QtJP2OpenJPEGTest::testJK()
{
  QImage originalImage(":/test_lossless.j2k");
  QVERIFY(!originalImage.isNull());

  QByteArray convertedArray;
  QBuffer buffer(&convertedArray);
  buffer.open(QIODevice::WriteOnly);
  originalImage.save(&buffer, "png", 100);
  buffer.close();

  QImage convertedImage;
  convertedImage.loadFromData(convertedArray);

  QCOMPARE(originalImage.size(), convertedImage.size());
  for (int i = 0; i < originalImage.height(); ++i)
    for (int j = 0; j < originalImage.width(); ++j)
    {
      QRgb original = originalImage.pixel(j, i);
      QRgb converted = convertedImage.pixel(j, i);
      QCOMPARE(qRed(original), qRed(converted));
      QCOMPARE(qGreen(original), qGreen(converted));
      QCOMPARE(qBlue(original), qBlue(converted));
    }
}


void QtJP2OpenJPEGTest::colorJP()
{
  QImage originalImage(":/test_colorcat.jp2");
  QVERIFY(!originalImage.isNull());

  QByteArray convertedArray;
  QBuffer buffer(&convertedArray);
  buffer.open(QIODevice::WriteOnly);
  originalImage.save(&buffer, "png", 100);
  buffer.close();

  QImage convertedImage;
  convertedImage.loadFromData(convertedArray);

  QCOMPARE(originalImage.size(), convertedImage.size());
  for (int i = 0; i < originalImage.height(); ++i)
    for (int j = 0; j < originalImage.width(); ++j)
    {
      QRgb original = originalImage.pixel(j, i);
      QRgb converted = convertedImage.pixel(j, i);
      QCOMPARE(qRed(original), qRed(converted));
      QCOMPARE(qGreen(original), qGreen(converted));
      QCOMPARE(qBlue(original), qBlue(converted));
    }
}


void QtJP2OpenJPEGTest::coloralphaJP()
{
  QImage originalImage(":/test_coloralpha.jp2");
  QVERIFY(!originalImage.isNull());

  QByteArray convertedArray;
  QBuffer buffer(&convertedArray);
  buffer.open(QIODevice::WriteOnly);
  originalImage.save(&buffer, "png", 100);
  buffer.close();

  QImage convertedImage;
  convertedImage.loadFromData(convertedArray);

  QCOMPARE(originalImage.size(), convertedImage.size());
  for (int i = 0; i < originalImage.height(); ++i)
    for (int j = 0; j < originalImage.width(); ++j)
    {
      QRgb original = originalImage.pixel(j, i);
      QRgb converted = convertedImage.pixel(j, i);
      QCOMPARE(qRed(original), qRed(converted));
      QCOMPARE(qGreen(original), qGreen(converted));
      QCOMPARE(qBlue(original), qBlue(converted));
    }
}


void QtJP2OpenJPEGTest::test1JP()
{
  QImage originalImage(":/test1.jp2");
  QVERIFY(!originalImage.isNull());

  QByteArray convertedArray;
  QBuffer buffer(&convertedArray);
  buffer.open(QIODevice::WriteOnly);
  originalImage.save(&buffer, "png", 100);
  buffer.close();

  QImage convertedImage;
  convertedImage.loadFromData(convertedArray);

  QCOMPARE(originalImage.size(), convertedImage.size());
  for (int i = 0; i < originalImage.height(); ++i)
    for (int j = 0; j < originalImage.width(); ++j)
    {
      QRgb original = originalImage.pixel(j, i);
      QRgb converted = convertedImage.pixel(j, i);
      QCOMPARE(qRed(original), qRed(converted));
      QCOMPARE(qGreen(original), qGreen(converted));
      QCOMPARE(qBlue(original), qBlue(converted));
    }
}


void QtJP2OpenJPEGTest::test2JP()
{
  QImage originalImage(":/test2.jp2");
  QVERIFY(!originalImage.isNull());

  QByteArray convertedArray;
  QBuffer buffer(&convertedArray);
  buffer.open(QIODevice::WriteOnly);
  originalImage.save(&buffer, "png", 100);
  buffer.close();

  QImage convertedImage;
  convertedImage.loadFromData(convertedArray);

  QCOMPARE(originalImage.size(), convertedImage.size());
  for (int i = 0; i < originalImage.height(); ++i)
    for (int j = 0; j < originalImage.width(); ++j)
    {
      QRgb original = originalImage.pixel(j, i);
      QRgb converted = convertedImage.pixel(j, i);
      QCOMPARE(qRed(original), qRed(converted));
      QCOMPARE(qGreen(original), qGreen(converted));
      QCOMPARE(qBlue(original), qBlue(converted));
    }
}


void QtJP2OpenJPEGTest::test1JK()
{
  QImage originalImage(":/test_buxI.j2k");
  QVERIFY(!originalImage.isNull());

  QByteArray convertedArray;
  QBuffer buffer(&convertedArray);
  buffer.open(QIODevice::WriteOnly);
  originalImage.save(&buffer, "png", 100);
  buffer.close();

  QImage convertedImage;
  convertedImage.loadFromData(convertedArray);

  QCOMPARE(originalImage.size(), convertedImage.size());
  for (int i = 0; i < originalImage.height(); ++i)
    for (int j = 0; j < originalImage.width(); ++j)
    {
      QRgb original = originalImage.pixel(j, i);
      QRgb converted = convertedImage.pixel(j, i);
      QCOMPARE(qRed(original), qRed(converted));
      QCOMPARE(qGreen(original), qGreen(converted));
      QCOMPARE(qBlue(original), qBlue(converted));
    }
}


void QtJP2OpenJPEGTest::test2JK()
{
  QImage originalImage(":/test_buxR.j2k");
  QVERIFY(!originalImage.isNull());

  QByteArray convertedArray;
  QBuffer buffer(&convertedArray);
  buffer.open(QIODevice::WriteOnly);
  originalImage.save(&buffer, "png", 100);
  buffer.close();

  QImage convertedImage;
  convertedImage.loadFromData(convertedArray);

  QCOMPARE(originalImage.size(), convertedImage.size());
  for (int i = 0; i < originalImage.height(); ++i)
    for (int j = 0; j < originalImage.width(); ++j)
    {
      QRgb original = originalImage.pixel(j, i);
      QRgb converted = convertedImage.pixel(j, i);
      QCOMPARE(qRed(original), qRed(converted));
      QCOMPARE(qGreen(original), qGreen(converted));
      QCOMPARE(qBlue(original), qBlue(converted));
    }
}

QTEST_MAIN(QtJP2OpenJPEGTest)
