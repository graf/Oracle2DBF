#-------------------------------------------------
#
# Project created by QtCreator 2011-07-11T23:29:11
#
#-------------------------------------------------

QT       += core gui sql

TARGET = oraJdbf

include(../common.pri)

TEMPLATE = app
DESTDIR = $$BUILD_TREE/bin

LIBS *= -l$$qtLibraryName(QDbf)

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

target.path = /bin

INSTALLS += target






