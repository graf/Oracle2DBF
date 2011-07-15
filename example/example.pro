TARGET = example

include(../common.pri)

TEMPLATE = app
DESTDIR = $$BUILD_TREE/bin

LIBS *= -l$$qtLibraryName(QDbf)

HEADERS += \
    mainwindow.h
SOURCES += \
    main.cpp \
    mainwindow.cpp

target.path = /bin
INSTALLS += target
