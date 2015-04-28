#-------------------------------------------------
#
# Project created by QtCreator 2014-07-24T13:55:51
#
#-------------------------------------------------

#QT       += core

QT       -= gui


TARGET = reconstruction3D
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += static

CONFIG += c++11

QMAKE_CXXFLAGS += -std=c++11
#QMAKE_LFLAGS += -static-libgcc

TEMPLATE = app



SOURCES += src/Main.cpp \
    src/Octree.cpp \
    src/ReconstructOctree.cpp \
    src/TriSurfMesh.cpp \
    src/InitPara.cpp

HEADERS += \
    include/CImg.h \
    include/conncomp.h \
    include/connect.h \
    include/LinkedGraph.h \
    include/mesh.h \
    include/octree.h \
    include/opengl.h \
    include/ReconstructOctree.h \
    include/TriSurfMesh.h \
    include/util.h \
    include/para.h



unix:INCLUDEPATH += $$PWD
unix: LIBS += -lX11 -lpng


win32:INCLUDEPATH += C:/Users/tom/danforth/roots3d/reconstruction3D/

win32:LIBS += \
    $$PWD/lib/libpng16.lib \
    $$PWD/lib/x86/User32.Lib \
    $$PWD/lib/x86/Gdi32.Lib \
    $$PWD/lib/x86/shell32.lib
