TARGET = QDbf

include(../library.pri)

DEFINES += QDBF_LIBRARY

INCLUDEPATH += $$PWD
DEPENDPATH += $$INCLUDEPATH

SOURCES += \
    qdbffield.cpp \
    qdbfrecord.cpp \
    qdbftable.cpp \
    qdbftablemodel.cpp
HEADERS += \
    qdbffield.h \
    qdbfrecord.h \
    qdbftable.h \
    qdbftablemodel.h \
    qdbf_global.h
