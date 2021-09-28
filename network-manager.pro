TEMPLATE = lib
QT += network
qtHaveModule(gui){
    QT += gui
    DEFINES += QT_HAVE_GUI
}else{
QT -= gui
}
CONFIG += staticlib
SOURCES += \
    $$PWD/src/networkmanager.cpp \
    $$PWD/src/networkresponse.cpp \

HEADERS += \
    $$PWD/src/networkmanager.h \
    $$PWD/src/networkresponse.h \
    $$PWD/src/router.h


unix {
    target.path = /usr/lib
    INSTALLS += target
}
