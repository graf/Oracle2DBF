TEMPLATE = lib

include(common.pri)

win32 {
    DLLDESTDIR = $$APP_PATH
}

DESTDIR = $$LIBRARY_PATH

TARGET = $$qtLibraryTarget($$TARGET)

CONFIG += dll

win32 {
    target.path = /bin
    target.files = $$DLLDESTDIR/$${TARGET}.dll
} else {
    target.path = /$$LIBRARY_BASENAME/$$PROJECT_NAME
}

INSTALLS += target
