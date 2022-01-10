TEMPLATE = lib
QT += network

contains(CONFIG,have_gui){
    QT += gui
    DEFINES += QT_HAVE_GUI
}else{
QT -= gui
}

CONFIG += staticlib

SOURCES += \
    networkmanager.cpp \
    networkresponse.cpp \

HEADERS += \
    networkmanager.h \
    networkresponse.h \
    router.h


unix {
    target.path = /usr/lib
    INSTALLS += target
}

