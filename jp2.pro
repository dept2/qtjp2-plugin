TARGET = qtjp2

QT       += core gui
CONFIG   += plugin
TEMPLATE = lib

HEADERS += src/QtJP2Plugin.h src/QtJP2OpenJPEGImageHandler.h
SOURCES += src/QtJP2Plugin.cpp src/QtJP2OpenJPEGImageHandler.cpp
contains(QT_MAJOR_VERSION, 5) {
  OTHER_FILES += jp2.json
}

packagesExist(libopenjp2) {
  CONFIG += link_pkgconfig
  PKGCONFIG += libopenjp2
} else {
  error("libopenjp2 library not found")
}

target.path += $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target
