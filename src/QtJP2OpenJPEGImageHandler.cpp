// Local
#include "QtJP2OpenJPEGImageHandler.h"

// STL
#include <cmath>

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
    spp++;

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
    cmptparm[i].w = static_cast<OPJ_UINT32>(source.width());
    cmptparm[i].h = static_cast<OPJ_UINT32>(source.height());
  }

  opj_image_t* image = opj_image_create(static_cast<OPJ_UINT32>(spp), &cmptparm[0], (spp > 2) ? OPJ_CLRSPC_SRGB : OPJ_CLRSPC_GRAY);
  if (!image)
  {
    qWarning() << "Can't create image object";
    return 0;
  }

  // Размеры изображения
  image->x0 = 0;
  image->y0 = 0;
  image->x1 = static_cast<OPJ_UINT32>(source.width());
  image->y1 = static_cast<OPJ_UINT32>(source.height());

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
      else if (spp == 4)
      {
        image->comps[0].data[y * source.width() + x] = qRed(pixel);
        image->comps[1].data[y * source.width() + x] = qGreen(pixel);
        image->comps[2].data[y * source.width() + x] = qBlue(pixel);
        image->comps[3].data[y * source.width() + x] = qAlpha(pixel);
        image->comps[3].alpha = 1;
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
  const int width = static_cast<int>(source->comps[0].w);
  const int height = static_cast<int>(source->comps[0].h);

  // TODO: Grayscale with alpha Channel should have source->numcomps == 2
  // QImage doesn't have such format. Maybe we can convert it in ARGB32
  if (source->numcomps != 4 && source->numcomps != 3 && source->numcomps != 1)
  {
    qWarning() << QString("Unsupported components count: %1\n").arg(source->numcomps);
    return QImage();
  }

  QImage::Format format = (source->numcomps == 3) ? QImage::Format_RGB32 :
                          (source->numcomps == 4) ? QImage::Format_ARGB32 : QImage::Format_Indexed8;

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
      else if (source->numcomps == 4)
      {
        pixel[3] = source->comps[3].data[y * width + x];
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

  // Sequence should be strictly descending
  compressRates[0] = rateMax;
  compressRates[numCompressRates - 1] = rateMin;

  for(int i = 1; i < numCompressRates - 1; ++i)
    compressRates[i] = compressRates[0] - delta * i;
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


namespace QtJP2
{
  void message(const char* msg, void* client_data)
  {
    Q_UNUSED(msg);
    Q_UNUSED(client_data);
    //qWarning() << msg;
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
    if (device->seek(device->pos() + size))
      return OPJ_OFF_T(size);
    else
      return OPJ_OFF_T(-1);
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
      if (opj_decode(codec, stream, opj_image))
      {
        if (opj_end_decompress(codec, stream))
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
        qWarning("Error decode image");
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


bool QtJP2OpenJPEGImageHandler::write(const QImage& image)
{
  QIODevice* d = device();
  if (!d)
    return false;

  OPJ_CODEC_FORMAT codecFormat = OPJ_CODEC_UNKNOWN;
  auto formatValue = format();
  if (formatValue == "j2k")
    codecFormat = OPJ_CODEC_J2K;
  else if (formatValue == "jp2")
    codecFormat = OPJ_CODEC_JP2;
  else
  {
    qWarning("Unknown image format to encode");
    return false;
  }

  opj_cparameters_t parameters;
  opj_set_default_encoder_parameters(&parameters);

  opj_codec_t* codec = opj_create_compress(codecFormat);
  opj_set_info_handler(codec, &QtJP2::message, NULL);
  opj_set_warning_handler(codec, &QtJP2::message, NULL);
  opj_set_error_handler(codec, &QtJP2::message, NULL);

  //opj_setup_encoder
  opj_image_t* opj_image = qImageToOpenjpeg(image);

  // Quality
  const int minQuality = 1;
  const int maxQuality = 100;

  if (m_quality == -1)
    m_quality = 100;
  if (m_quality <= minQuality)
    m_quality = minQuality;
  if (m_quality > maxQuality)
    m_quality = maxQuality;

  double compressRate = pow((double(100) / double(m_quality)), 2);

  double compressRates[numCompressRates];
  countCompressRates(compressRate, compressRate * previewCompressRateMult, compressRates);

  for (int i = 0; i < numCompressRates; ++i)
  {
    parameters.tcp_rates[i] = float(compressRates[i]);
    parameters.tcp_numlayers++;
  }

  parameters.cp_disto_alloc = 1;
  parameters.irreversible = (compressRate <= 1.0) ? 0 : 1;
  parameters.tcp_mct = static_cast<char>(opj_image->numcomps >= 3 ? 1 : 0);
  parameters.numresolution = countNumResolutions(std::max(image.width(), image.height()));
  parameters.prog_order = OPJ_RLCP;
  parameters.cblockh_init = parameters.cblockw_init = defaultBlockSize;

  if (opj_setup_encoder(codec, &parameters, opj_image))
  {
    opj_stream_t* stream = createStream(d, false);

    if (!opj_start_compress(codec, opj_image, stream))
    {
      qWarning("Error start compress");
      return false;
    }

    if (!opj_encode(codec, stream))
    {
      qWarning("Error encode image");
      return false;
    }

    if (!opj_end_compress(codec, stream))
    {
      qWarning("Error compress image");
      return false;
    }

    opj_stream_destroy(stream);
  }
  else
  {
    qWarning("Failed to setup encoder");
    return false;
  }

  opj_destroy_codec(codec);
  opj_image_destroy(opj_image);
  return true;
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
  {
    qWarning() << "Header is incorrect:" << header;
    return result;
  }

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
  if (!device->isSequential())
  {
    opj_stream_set_skip_function(stream, &QtJP2::streamSkip);
    opj_stream_set_seek_function(stream, &QtJP2::streamSeek);
  }

  return stream;
}
