#-------------------------------------------------
#
# Project created by QtCreator 2015-09-23T23:13:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PlanarModel
TEMPLATE = app

INCLUDEPATH += "D:/Program Files (x86)/GnuWin32/include"

SOURCES += main.cpp\
        mainwidget.cpp \
    lattice.cpp \
    displaymag.cpp \
    displayen.cpp

HEADERS  += mainwidget.h \
    lattice.h \
    displaymag.h \
    displayen.h

FORMS    += mainwidget.ui

LIBS += "D:/Program Files (x86)/GnuWin32/lib/libgsl.a"
LIBS += "D:/Program Files (x86)/GnuWin32/lib/libgslcblas.a"
