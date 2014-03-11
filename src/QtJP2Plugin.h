#ifndef QTJP2PLUGIN_H
#define QTJP2PLUGIN_H

// Qt
#include <QImageIOPlugin>


class QtJP2Plugin : public QImageIOPlugin
{
  Q_OBJECT
#if QT_VERSION >= 0x050000 // Qt5
  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "jp2.json")
#endif

  public:
    QtJP2Plugin(QObject* parent = 0);
    QStringList keys() const;
    Capabilities capabilities(QIODevice* device, const QByteArray& format) const;
    QImageIOHandler* create(QIODevice* device, const QByteArray& format = QByteArray()) const;
};

#endif // QTJP2PLUGIN_H
