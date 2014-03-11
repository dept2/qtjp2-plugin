CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

INCLUDE(CompilerFlags)

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

FIND_PACKAGE(OpenJPEG REQUIRED)

SET(sources
  src/QtJP2OpenJPEGImageHandler.cpp
  src/QtJP2Plugin.cpp
)

SET(moc_headers
  src/QtJP2Plugin.h
)


QT4_WRAP_CPP(sources ${moc_headers})

SET(library_target qtjp2)
ADD_LIBRARY(${library_target} SHARED ${sources})
TARGET_LINK_LIBRARIES(${library_target} ${QT_LIBRARIES} openjpeg)
INSTALL(TARGETS ${library_target} DESTINATION ${QT_PLUGINS_DIR}/imageformats)
