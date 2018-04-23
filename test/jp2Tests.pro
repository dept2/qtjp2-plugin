TARGET = qtjp2tests

QT       += testlib

HEADERS += src/QtJP2OpenJPEGTest.h
SOURCES += src/QtJP2OpenJPEGTest.cpp

RESOURCES = files/files.qrc

packagesExist(libopenjp2) {
  CONFIG += link_pkgconfig
  PKGCONFIG += libopenjp2
} else {
  error("libopenjp2 library not found")
}
