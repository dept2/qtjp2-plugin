CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Test REQUIRED)

SET(sources
  src/QtJP2OpenJPEGTest.cpp
)

SET(moc src/QtJP2OpenJPEGTest.h)

SET(resources files/files.qrc)

QT5_WRAP_CPP(sources ${moc})
QT5_ADD_RESOURCES(sources ${resources})

SET(binary_target qtjp2test)
ADD_EXECUTABLE(${binary_target} ${sources} ${resources})
TARGET_LINK_LIBRARIES(${binary_target}
  Qt5::Core
  Qt5::Gui
  Qt5::Test
  openjp2
)
