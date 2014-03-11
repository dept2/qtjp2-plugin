// Local
#include "QtJP2OpenJPEGTest.h"

// Qt test
#include <QtTest/QtTest>

// Qt
#include <QImage>
#include <QPixmap>


void QtJP2OpenJPEGTest::lossless()
{
  QImage originalImage("/tmp/filecolor.jpg");

  QByteArray convertedArray;
  QBuffer buffer(&convertedArray);
  buffer.open(QIODevice::WriteOnly);
  originalImage.save(&buffer, "jp2", 100);
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
