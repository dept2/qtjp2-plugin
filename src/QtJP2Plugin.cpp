#ifndef QT_NO_IMAGEFORMATPLUGIN

// Local
#include "QtJP2Plugin.h"
#include "QtJP2OpenJPEGImageHandler.h"

// OpenJPEG
#include <openjpeg.h>


QtJP2Plugin::QtJP2Plugin(QObject* parent)
  : QImageIOPlugin(parent)
{}


QStringList QtJP2Plugin::keys() const
{
  return QStringList() << "jp2" << "j2k";
}


QImageIOPlugin::Capabilities QtJP2Plugin::capabilities(QIODevice* device, const QByteArray& format) const
{
  if (format == "jp2" || format == "j2k")
    return Capabilities(CanRead | CanWrite);

  if (!format.isEmpty())
    return 0;

  if (!device->isOpen())
    return 0;

  Capabilities caps;
  if (device->isReadable() && (QtJP2OpenJPEGImageHandler::canRead(device) != CODEC_UNKNOWN))
    caps |= CanRead;

  if (device->isWritable())
    caps |= CanWrite;

  return caps;
}


QImageIOHandler* QtJP2Plugin::create(QIODevice* device, const QByteArray& format) const
{
  QImageIOHandler* handler = new QtJP2OpenJPEGImageHandler(device);
  handler->setFormat(format);
  return handler;
}

#if QT_VERSION < 0x050000 // Qt4
  Q_EXPORT_PLUGIN2(qtjp2, QtJP2Plugin)
#endif

#endif
