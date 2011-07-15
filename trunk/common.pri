CONFIG += qt warn-on

defineReplace(cleanPath) {
    win32:1 ~= s|\\\\|/|g
    contains(1, ^/.*):pfx = /
    else:pfx =
    segs = $$split(1, /)
    out =
    for(seg, segs) {
        equals(seg, ..):out = $$member(out, 0, -2)
        else:!equals(seg, .):out += $$seg
    }
    return($$join(out, /, $$pfx))
}

defineReplace(qtLibraryName) {
   unset(LIBRARY_NAME)
   LIBRARY_NAME = $$1
   CONFIG(debug, debug|release) {
      !debug_and_release|build_pass {
          mac:RET = $$member(LIBRARY_NAME, 0)_debug
              else:win32:RET = $$member(LIBRARY_NAME, 0)d
      }
   }
   isEmpty(RET):RET = $$LIBRARY_NAME
   return($$RET)
}

isEmpty(LIBRARY_BASENAME) {
    LIBRARY_BASENAME = lib
}

DEFINES += LIBRARY_BASENAME=\\\"$$LIBRARY_BASENAME\\\"

SOURCE_TREE = $$cleanPath($$PWD)
isEmpty(BUILD_TREE) {
    sub_dir = $$_PRO_FILE_PWD_
    sub_dir ~= s,^$$re_escape($$SOURCE_TREE),,
    BUILD_TREE = $$cleanPath($$OUT_PWD)
    BUILD_TREE ~= s,$$re_escape($$sub_dir)$,,
}

APP_PATH = $$BUILD_TREE/bin
win32:contains(TEMPLATE, vc.*)|contains(TEMPLATE_PREFIX, vc):vcproj = 1
LIBRARY_PATH = $$BUILD_TREE/$$LIBRARY_BASENAME
!isEqual(SOURCE_TREE, $$BUILD_TREE):copydata = 1

INCLUDEPATH += $$SOURCE_TREE/src
DEPENDPATH += $$INCLUDEPATH

LIBS += -L$$LIBRARY_PATH

DEFINES += QT_NO_CAST_FROM_ASCII
DEFINES += QT_NO_CAST_TO_ASCII
!macx:DEFINES += QT_USE_FAST_OPERATOR_PLUS QT_USE_FAST_CONCATENATION

win32 {
    BUILD_PATH = $$BUILD_TREE/build
} else {
    BUILD_PATH = $$BUILD_TREE/.build
}

CONFIG(debug, debug|release) {
    OBJECTS_DIR = $$BUILD_PATH/obj/$$TARGET/debug
    MOC_DIR = $$BUILD_PATH/moc/$$TARGET/debug
} else {
    OBJECTS_DIR = $$BUILD_PATH/obj/$$TARGET/release
    MOC_DIR = $$BUILD_PATH/moc/$$TARGET/release
}

RCC_DIR = $$BUILD_PATH/rcc/$$TARGET
UI_DIR = $$BUILD_PATH/uic/$$TARGET

linux-g++-* {
    # Bail out on non-selfcontained libraries. Just a security measure
    # to prevent checking in code that does not compile on other platforms.
    QMAKE_LFLAGS += -Wl,--allow-shlib-undefined -Wl,--no-undefined
}

win32-msvc* {
    #Don't warn about sprintf, fopen etc being 'unsafe'
    DEFINES += _CRT_SECURE_NO_WARNINGS
}
