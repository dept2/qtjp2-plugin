// Local
#include "QtJP2OpenJPEGImageHandler.h"

// Qt
#include <QImage>
#include <QVariant>
#include <QDebug>


/*! Сигнатура начала потока формата afis_jp2, служит для определения используемого декодера. */
const char* jp2Sign = "\x00\x00\x00\x0C\x6A\x50";

/*! Количество под-степеней сжатия, регламентируется ANSI/NIST*/
const int numCompressRates = 10;

/*! Множитель для под-степеней сжатия, регламентируется ANSI/NIST*/
const int previewCompressRateMult = 10;

/*!  Минимальное разрешение для подсчета количества разрешений, регламентируется ANSI/NIST */
const int minSize = 64;

/*! Минимальное количество разрешений, регламентируется ANSI/NIST*/
const int minResolutions = 6;

/*! Размер блока для сжатия*/
const int defaultBlockSize = 32;


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

  opj_image_t* image = opj_image_create(spp, &cmptparm[0], spp == 1 ? CLRSPC_GRAY : CLRSPC_SRGB);
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
  \return
  - 0 - успешно
  - 1 - ошибка
*/
QByteArray pack(const QImage& source, int quality, OPJ_CODEC_FORMAT format = CODEC_JP2)
{
  double compressRate = 100. / (double)quality;
  double compressRates[numCompressRates];
  countCompressRates(compressRate, compressRate * previewCompressRateMult, compressRates);

  opj_cparameters_t parameters;
  memset(&parameters, 0, sizeof(parameters));

  opj_event_mgr_t eventManager;
  memset(&eventManager, 0, sizeof(opj_event_mgr_t));
  eventManager.error_handler = NULL;
  eventManager.warning_handler = NULL;
  eventManager.info_handler = NULL;

  opj_set_default_encoder_parameters(&parameters);

  const char* cmt = "badlogin/OpenJPEG";
  parameters.cp_comment = new char[strlen(cmt) + 1];
  strcpy(parameters.cp_comment, cmt);

  opj_image_t* image = qImageToOpenjpeg(source);

  opj_cinfo_t* cinfo = opj_create_compress(format);
  opj_set_event_mgr((opj_common_ptr)cinfo, &eventManager, stderr);

  if (!image)
  {
    delete parameters.cp_comment;
    return QByteArray();
  }

  for (int i = 0; i < numCompressRates; ++i)
  {
    parameters.tcp_rates[i] = (float)compressRates[i];
    parameters.tcp_numlayers++;
    parameters.cp_disto_alloc = 1;
  }

  parameters.irreversible = (compressRate <= 1.0) ? 0 : 1;
  parameters.tcp_mct = static_cast<char>(image->numcomps == 3 ? 1 : 0);
  parameters.numresolution = countNumResolutions(std::max(source.width(), source.height()));
  parameters.prog_order = RLCP;
  parameters.cblockh_init = parameters.cblockw_init = defaultBlockSize;

  opj_setup_encoder(cinfo, &parameters, image);
  opj_cio_t* cio = opj_cio_open((opj_common_ptr)cinfo, NULL, 0);
  if (!opj_encode(cinfo, cio, image, NULL))
  {
    qWarning("Can't encode image");
    delete parameters.cp_comment;
    opj_cio_close(cio);
    opj_destroy_compress(cinfo);
    opj_image_destroy(image);
    return QByteArray();
  }

  int codestream_length = cio_tell(cio);

  QByteArray result;
  result.append(reinterpret_cast<char*>(cio->buffer), codestream_length);

  opj_cio_close(cio);
  delete parameters.cp_comment;
  opj_destroy_compress(cinfo);
  opj_image_destroy(image);
  return result;
}


QImage unpack(const QByteArray& source)
{
  opj_event_mgr_t event_mgr;
  memset(&event_mgr, 0, sizeof(opj_event_mgr_t));
  event_mgr.error_handler = NULL;
  event_mgr.warning_handler = NULL;
  event_mgr.info_handler = NULL;

  opj_dparameters_t parameters;
  opj_set_default_decoder_parameters(&parameters);

  const char* data = source.data();

  opj_dinfo_t* dinfo = opj_create_decompress(memcmp(data, jp2Sign, 6) == 0 ? CODEC_JP2 : CODEC_J2K);
  opj_set_event_mgr((opj_common_ptr)dinfo, &event_mgr, stderr);
  opj_setup_decoder(dinfo, &parameters);

  opj_cio_t* cio = opj_cio_open((opj_common_ptr)dinfo, (unsigned char*)data, source.size());

  opj_image_t* opj_image = opj_decode(dinfo, cio);
  if (!opj_image)
  {
    qWarning("Error decompressing image");
    return QImage();
  }

  QImage image = openjpegToQImage(opj_image);
  if (image.isNull())
  {
    qWarning("Image not converted");
    return QImage();
  }

  opj_destroy_decompress(dinfo);
  opj_image_destroy(opj_image);
  opj_cio_close(cio);

  return image;
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
  return (QtJP2OpenJPEGImageHandler::canRead(device()) != CODEC_UNKNOWN);
}


bool QtJP2OpenJPEGImageHandler::read(QImage* image)
{
  if (!device())
    return false;

  QByteArray imageArray = device()->readAll();
  *image = unpack(imageArray);

  return true;
}


bool QtJP2OpenJPEGImageHandler::write(const QImage& image)
{
  if (!device())
    return false;

  OPJ_CODEC_FORMAT codecFormat = CODEC_JP2;
  if (format() == "j2k")
    codecFormat = CODEC_J2K;

  QByteArray imageArray = pack(image, m_quality, codecFormat);
  device()->write(imageArray);

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


OPJ_CODEC_FORMAT QtJP2OpenJPEGImageHandler::canRead(QIODevice* ioDevice)
{
  if (ioDevice)
  {
    unsigned char header[12];
    if (ioDevice->peek(reinterpret_cast<char*>(header), sizeof(header)) == sizeof(header))
    {
      char matchHeaderJp2[] = "\000\000\000\fjP  \r\n\207\n";
      char matchHeaderJ2k[] = "\377\117\377\121\000";
      if (memcmp(header, matchHeaderJp2, sizeof(matchHeaderJp2) - 1) == 0)
        return CODEC_JP2;
      else if (memcmp(header, matchHeaderJ2k, sizeof(matchHeaderJ2k) - 1) == 0)
        return CODEC_J2K;
    }
  }

  return CODEC_UNKNOWN;
}
