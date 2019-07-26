TEMPLATE = lib
QT += network gui
CONFIG += staticlib
SOURCES += \
    $$PWD/src/networkmanager.cpp \
    $$PWD/src/networkresponse.cpp \
    $$PWD/src/router.cpp

HEADERS += \
    $$PWD/src/networkmanager.h \
    $$PWD/src/networkresponse.h \
    $$PWD/src/router.h
