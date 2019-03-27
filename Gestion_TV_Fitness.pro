QT += core
QT += network
QT += xml
QT += gui

TARGET = Gestion_TV_Fitness
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += ./include

SOURCES += src/main.cpp \
    src/tMainClass.cpp \
    src/tPlayer.cpp \
    src/tTcpSocket.cpp

HEADERS += \
    include/tBackground.h \
    include/tEvent.h \
    include/tMainClass.h \
    include/tMedia.h \
    include/tParametreGlobal.h \
    include/tPlayer.h \
    include/tTcpSocket.h \
    include/tTv.h \
    include/tScheduler.h \
    include/tools.h

INSTALLS        += target
target.files    = Gestion_TV_Fitness
target.path     = /home/pi
