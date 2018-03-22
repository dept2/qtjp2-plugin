// Local
#include "QtJP2OpenJPEGImageHandler.h"

// Qt
#include <QImage>
#include <QVariant>
#include <QDebug>


// File types magic bytes
static const char* jp2_rfc3745_magic = "\x00\x00\x00\x0c\x6a\x50\x20\x20\x0d\x0a\x87\x0a";
static const char* jp2_magic = "\x0d\x0a\x87\x0a";
static const char* j2k_codestream_magic = "\xff\x4f\xff\x51";

/*! Количество под-степеней сжатия, регламентируется ANSI/NIST*/
static const int numCompressRates = 10;

/*! Множитель для под-степеней сжатия, регламентируется ANSI/NIST*/
static const int previewCompressRateMult = 10;

/*!  Минимальное разрешение для подсчета количества разрешений, регламентируется ANSI/NIST */
static const int minSize = 64;

/*! Минимальное количество разрешений, регламентируется ANSI/NIST*/
static const int minResolutions = 6;

/*! Размер блока для сжатия*/
static const int defaultBlockSize = 32;


/*! Преобразование изображения из бинарного формата во внутренних формат библиотеки OpenJpeg.
  \param[in] source - исходное изображение
  \return указатель на изображение в формате библиотеки OpenJpeg
*/
opj_image_t* qImageToOpenjpeg(const QImage& source)
{
  int spp;
  if (source.format() == QImage::Format_Indexed8)
    spp = source.isGrayscale() ? 1 : 3;
#if QT_VERSION >= 0x050500
  else if (source.format() == QImage::Format_Grayscale8)
    spp = 1;
#endif
  else if (source.format() == QImage::Format_Mono || source.format() == QImage::Format_MonoLSB)
    spp = 1;
  else
    spp = 3;

  if (source.hasAlphaChannel())
    qWarning() << "Alpha channels in images is unsupported by libopenjpeg. The alpha channel will be ignored.";

  const int bps = 8;

  opj_image_cmptparm_t cmptparm[spp];
  memset(cmptparm, 0, sizeof(cmptparm));

  // Заполняются структуры параметров, используемых для создания изображения формата OpenJPEG
  for (int i = 0; i < spp; ++i)
  {
    cmptparm[i].prec = bps;
    cmptparm[i].bpp = bps;
    cmptparm[i].sgnd = 0;
    cmptparm[i].dx = 1;
    cmptparm[i].dy = 1;
    cmptparm[i].w = source.width();
    cmptparm[i].h = source.height();
  }

  opj_image_t* image = opj_image_create(spp, &cmptparm[0], spp == 1 ? OPJ_CLRSPC_GRAY : OPJ_CLRSPC_SRGB);
  if (!image)
  {
    qWarning() << "Can't create image object";
    return 0;
  }

  // Размеры изображения
  image->x0 = 0;
  image->y0 = 0;
  image->x1 = source.width();
  image->y1 = source.height();

  // Копирование данных
  for (int y = 0; y < source.height(); ++y)
  {
    for (int x = 0; x < source.width(); ++x)
    {
      QRgb pixel = source.pixel(x, y);

      if (spp == 3)
      {
        image->comps[0].data[y * source.width() + x] = qRed(pixel);
        image->comps[1].data[y * source.width() + x] = qGreen(pixel);
        image->comps[2].data[y * source.width() + x] = qBlue(pixel);
      }
      else if (spp == 1)
      {
        image->comps[0].data[y * source.width() + x] = qGray(pixel);
      }
    }
  }

  return image;
}


/*! Преобразование изображения из внутреннего формата библиотеки OpenJpeg в бинарный вид.
  \param[in]  source - изображение в формате библиотеки OpenJpeg
  \return   QImage
*/
QImage openjpegToQImage(opj_image_t* source)
{
  const int width = source->comps[0].w;
  const int height = source->comps[0].h;

  if (source->numcomps != 3 && source->numcomps != 1)
  {
    qWarning() << "Unsupported components count\n";
    return QImage();
  }

  QImage::Format format = (source->numcomps == 3) ? QImage::Format_RGB32 : QImage::Format_Indexed8;
  QImage image(width, height, format);
  if (format == QImage::Format_Indexed8)
  {
    // Заполнить палитру
    QVector<QRgb> colors;
    for (int i = 0; i <= 255; ++i)
      colors << qRgb(i, i, i);

    image.setColorTable(colors);
  }

  for (int y = 0; y < height; ++y)
  {
    uchar* line = image.scanLine(y);
    for (int x = 0; x < width; ++x)
    {
      uchar* pixel = (source->numcomps == 1) ? (line + x) : (line + (x * 4));
      if (source->numcomps == 1)
      {
        *pixel = source->comps[0].data[y * width + x];
      }
      else if (source->numcomps == 3)
      {
        pixel[2] = source->comps[0].data[y * width + x];
        pixel[1] = source->comps[1].data[y * width + x];
        pixel[0] = source->comps[2].data[y * width + x];
      }
    }
  }

  return image;
}


/*! count array of comress rates, accompliance to ANSI/NIST standard
  \param[in] rateMin - минимальная степень сжатия
  \param[in] rateMax - максимальная степень сжатия
  \param[out] compressRates   - возвращает заполненный массив степеней сжатия
*/
void countCompressRates(double rateMin, double rateMax, double* compressRates)
{
  double delta = (rateMax - rateMin) / 9.0;

  compressRates[0] = rateMin;
  compressRates[numCompressRates - 1] = rateMax;

  for(int i = 1; i < numCompressRates - 1; ++i)
    compressRates[i] = compressRates[0] + delta * i;
}


/*! count number of resolutions, accompliance to ANSI/NIST standard
  \param[in] size      - размер изображения
  \return количество необходимых разрешений
*/
int countNumResolutions(int size)
{
   int i = 1;
   while (size > minSize)
   {
      ++i;
      size /= 2;
   }

   return std::max(minResolutions, i);
}


/*! Упаковать изображение в jpeg2000 - формат
  \param[in] rawImage         - исходное изображение
  \param[in] width             - ширина изображения
  \param[in] height            - высота изображения
  \param[in] samplesPerPixel - число компонент в цвете.
  - 1 - for GRAY images
  - 3 - for GRB images
  \param[in] bitsPerSample   - битность компоненты цвета (8 или 16)
  \param[in] compress_rate     - уровень сжатия.
  - менее 1.0 - lossless compression
  - более 1.0 - lossy compression
  \param[in] comment           - комментарий (если NULL - "liba8jp2/OpenJPEG").
  \param[out] jp2image    - упакованное afis_jp2 изображение
  \param[out] jp2len      - размер afis_jp2 изображения
  \note Generated JPEG200 is ANSI/NIST-ITL 1-2007 compliaced.
  \note image - память необходимо освобождать снаружи с помощью liba8jp2_free()
*/
//QByteArray pack(const QImage& source, int quality, OPJ_CODEC_FORMAT format = OPJ_CODEC_JP2)
//{
//  double compressRate = 100. / double(quality);
//  double compressRates[numCompressRates];
//  countCompressRates(compressRate, compressRate * previewCompressRateMult, compressRates);

//  opj_cparameters_t parameters;
//  memset(&parameters, 0, sizeof(parameters));

//  opj_set_default_encoder_parameters(&parameters);

//  const char* cmt = "dept2/OpenJPEG";
//  parameters.cp_comment = new char[strlen(cmt) + 1];
//  strcpy(parameters.cp_comment, cmt);

//  opj_image_t* image = qImageToOpenjpeg(source);
//  opj_codec_t* cinfo = opj_create_compress(format);

//  if (!image)
//  {
//    delete parameters.cp_comment;
//    return QByteArray();
//  }

//  for (int i = 0; i < numCompressRates; ++i)
//  {
//    parameters.tcp_rates[i] = float(compressRates[i]);
//    parameters.tcp_numlayers++;
//    parameters.cp_disto_alloc = 1;
//  }

//  parameters.irreversible = (compressRate <= 1.0) ? 0 : 1;
//  parameters.tcp_mct = static_cast<char>(image->numcomps == 3 ? 1 : 0);
//  parameters.numresolution = countNumResolutions(std::max(source.width(), source.height()));
//  parameters.prog_order = OPJ_RLCP;
//  parameters.cblockh_init = parameters.cblockw_init = defaultBlockSize;

//  opj_setup_encoder(cinfo, &parameters, image);
//  opj_cio_t* cio = opj_cio_open((opj_common_ptr)cinfo, NULL, 0);
//  if (!opj_encode(cinfo, cio, image, NULL))
//  {
//    qWarning("Can't encode image");
//    delete parameters.cp_comment;
//    opj_cio_close(cio);
//    opj_destroy_codec(cinfo);
//    opj_image_destroy(image);
//    return QByteArray();
//  }

//  int codestream_length = cio_tell(cio);

//  QByteArray result;
//  result.append(reinterpret_cast<char*>(cio->buffer), codestream_length);

//  opj_cio_close(cio);
//  delete parameters.cp_comment;
//  opj_destroy_codec(cinfo);
//  opj_image_destroy(image);
//  return result;
//}


namespace QtJP2
{
  void message(const char* msg, void* client_data)
  {
    Q_UNUSED(msg);
    Q_UNUSED(client_data);
  }

  OPJ_SIZE_T streamRead(void* buffer, OPJ_SIZE_T size, void* d)
  {
    QIODevice* device = static_cast<QIODevice*>(d);
    qint64 bytesRead = device->read(static_cast<char*>(buffer), qint64(size));
    return OPJ_SIZE_T(bytesRead > 0 ? bytesRead : -1);
  }

  OPJ_SIZE_T streamWrite(void* buffer, OPJ_SIZE_T size, void* d)
  {
    QIODevice* device = static_cast<QIODevice*>(d);
    qint64 bytesWritten = device->write(static_cast<const char*>(buffer), qint64(size));
    return bytesWritten != -1 ? OPJ_SIZE_T(bytesWritten) : 0;
  }

  OPJ_OFF_T streamSkip(OPJ_OFF_T size, void* d)
  {
    QIODevice* device = static_cast<QIODevice*>(d);
    qint64 bytesRead = device->read(qint64(size)).size();
    return OPJ_OFF_T(bytesRead > 0 ? bytesRead : -1);
  }

  OPJ_BOOL streamSeek(OPJ_OFF_T pos, void* d)
  {
    QIODevice* device = static_cast<QIODevice*>(d);
    if (!device->isSequential())
      return device->seek(pos);
    else
      return OPJ_FALSE;
  }

  void freeStreamUserData(void*)
  {}
}


QtJP2OpenJPEGImageHandler::QtJP2OpenJPEGImageHandler(QIODevice* device)
  : m_quality(100)
{
  setDevice(device);
}


QtJP2OpenJPEGImageHandler::~QtJP2OpenJPEGImageHandler()
{}


bool QtJP2OpenJPEGImageHandler::canRead() const
{
  return (codecFormat(device()) != OPJ_CODEC_UNKNOWN);
}


bool QtJP2OpenJPEGImageHandler::read(QImage* image)
{
  QIODevice* d = device();
  if (!d)
    return false;

  OPJ_CODEC_FORMAT format = codecFormat(d);
  if (format == OPJ_CODEC_UNKNOWN)
  {
    qWarning("Unknown image format to decode");
    return false;
  }

  opj_dparameters_t parameters;
  opj_set_default_decoder_parameters(&parameters);

  bool ok = false;
  opj_codec_t* codec = opj_create_decompress(format);
  opj_set_info_handler(codec, &QtJP2::message, NULL);
  opj_set_warning_handler(codec, &QtJP2::message, NULL);
  opj_set_error_handler(codec, &QtJP2::message, NULL);
  if (opj_setup_decoder(codec, &parameters))
  {
    opj_stream_t* stream = createStream(d, true);
    opj_image_t* opj_image = NULL;

    if (opj_read_header(stream, codec, &opj_image))
    {
      if (opj_decode(codec, stream, opj_image) && opj_end_decompress(codec, stream))
      {
        *image = openjpegToQImage(opj_image);
        ok = !(image->isNull());
        if (!ok)
          qWarning("Image not converted");
      }
      else
      {
        qWarning("Error decompressing image");
      }
    }
    else
    {
      qWarning("Error reading image header");
    }

    opj_image_destroy(opj_image);
    opj_stream_destroy(stream);
  }
  else
  {
    qWarning("Failed to setup decoder");
  }

  opj_destroy_codec(codec);
  return ok;
}


bool QtJP2OpenJPEGImageHandler::write(const QImage& /*image*/)
{
//  if (!device())
    return false;

//  OPJ_CODEC_FORMAT codecFormat = OPJ_CODEC_JP2;
//  if (format() == "j2k")
//    codecFormat = OPJ_CODEC_J2K;

//  QByteArray imageArray = pack(image, m_quality, codecFormat);
//  device()->write(imageArray);

//  return true;
}


void QtJP2OpenJPEGImageHandler::setOption(QImageIOHandler::ImageOption option, const QVariant& value)
{
  if (option == Quality)
  {
    bool ok;
    const int quality = value.toInt(&ok);
    if (ok)
      m_quality = quality;
  }
}


bool QtJP2OpenJPEGImageHandler::supportsOption(QImageIOHandler::ImageOption option) const
{
  return (option == QImageIOHandler::Quality);
}


OPJ_CODEC_FORMAT QtJP2OpenJPEGImageHandler::codecFormat(QIODevice* device)
{
  OPJ_CODEC_FORMAT result = OPJ_CODEC_UNKNOWN;
  if (!device)
    return result;

  QByteArray header = device->peek(12);
  if (header.size() != 12)
    return result;

  if (memcmp(header.constData(), jp2_rfc3745_magic, 12) == 0 || memcmp(header.constData(), jp2_magic, 4) == 0)
    result = OPJ_CODEC_JP2;
  else if (memcmp(header.constData(), j2k_codestream_magic, 4) == 0)
    result  = OPJ_CODEC_J2K;

  return result;
}


opj_stream_t* QtJP2OpenJPEGImageHandler::createStream(QIODevice* device, bool isRead)
{
  opj_stream_t* stream = opj_stream_create(OPJ_J2K_STREAM_CHUNK_SIZE, isRead);
  if (!stream)
    return NULL;

  opj_stream_set_user_data(stream, device, &QtJP2::freeStreamUserData);
  opj_stream_set_user_data_length(stream, OPJ_UINT64(device->bytesAvailable()));
  opj_stream_set_read_function(stream, &QtJP2::streamRead);
  opj_stream_set_write_function(stream, &QtJP2::streamWrite);
  opj_stream_set_skip_function(stream, &QtJP2::streamSkip);
  if (!device->isSequential())
    opj_stream_set_seek_function(stream, &QtJP2::streamSeek);

  return stream;
}
