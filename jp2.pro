TARGET = qtjp2

QT       += core gui
CONFIG   += plugin
TEMPLATE = lib

PLUGIN_TYPE = imageformats
PLUGIN_CLASS_NAME = QtJp2Plugin

HEADERS += src/QtJP2Plugin.h src/QtJP2OpenJPEGImageHandler.h
SOURCES += src/QtJP2Plugin.cpp src/QtJP2OpenJPEGImageHandler.cpp
contains(QT_MAJOR_VERSION, 5) {
  OTHER_FILES += jp2.json
}

unix|win32-g++*: LIBS += -lopenjpeg
else:win32: LIBS += libopenjpeg.lib

target.path = $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target
