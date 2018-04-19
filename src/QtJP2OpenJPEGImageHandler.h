#ifndef QTJP2OPENJPEGIMAGEHANDLER_H
#define QTJP2OPENJPEGIMAGEHANDLER_H

// Qt
#include <QImageIOHandler>

// OpenJPEG
#include <openjpeg-2.3/openjpeg.h>


class QtJP2OpenJPEGImageHandler : public QImageIOHandler
{
  public:
    QtJP2OpenJPEGImageHandler(QIODevice* device);
    virtual ~QtJP2OpenJPEGImageHandler();

    bool canRead() const;
    bool read(QImage* image);
    bool write(const QImage& image);

    void setOption(ImageOption option, const QVariant& value);
    bool supportsOption(ImageOption option) const;

    static OPJ_CODEC_FORMAT codecFormat(QIODevice* device);

  private:
    int m_quality;

    static opj_stream_t* createStream(QIODevice* device, bool isRead);
};

#endif // QTJP2OPENJPEGIMAGEHANDLER_H
