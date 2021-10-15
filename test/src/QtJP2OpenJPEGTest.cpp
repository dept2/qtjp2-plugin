// Local
#include "QtJP2OpenJPEGTest.h"

// Qt test
#include <QtTest/QtTest>

// Qt
#include <QImage>


void QtJP2OpenJPEGTest::lossless_data()
{
  QTest::addColumn<QString>("file");
  QTest::addColumn<QImage::Format>("outFormat");

  QTest::newRow("large rgb (jpeg)") << ":/unsplash.jpg" << QImage::Format_RGB32;
  QTest::newRow("argb (png)") << ":/test_coloralpha.png" << QImage::Format_ARGB32;
  QTest::newRow("monochrome (png)") << ":/test_gray1.png" << QImage::Format_Grayscale8;
  QTest::newRow("grayscale (png)") << ":/test_gray8.png" << QImage::Format_Grayscale8;
  QTest::newRow("gray + alpha (png)") << ":/test_graya16.png" << QImage::Format_ARGB32;
}


void QtJP2OpenJPEGTest::lossless()
{
  QFETCH(QString, file);

  QImage originalImage(file);
  QVERIFY(!originalImage.isNull());

  QByteArray convertedArray;
  QBENCHMARK
  {
    convertedArray.clear();
    QBuffer buffer(&convertedArray);
    buffer.open(QIODevice::WriteOnly);
    bool ok = originalImage.save(&buffer, "jp2", 100);
    QVERIFY(ok);
    buffer.close();
  }

  QImage convertedImage;
  convertedImage.loadFromData(convertedArray);

  QFETCH(QImage::Format, outFormat);
  QCOMPARE(convertedImage.format(), outFormat);

  QCOMPARE(originalImage.size(), convertedImage.size());
  for (int i = 0; i < originalImage.height(); ++i)
    for (int j = 0; j < originalImage.width(); ++j)
      QCOMPARE(originalImage.pixel(j, i), convertedImage.pixel(j, i));
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
