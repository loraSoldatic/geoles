#-------------------------------------------------
#
# Project created by QtCreator 2019-06-06T10:07:32
#
#-------------------------------------------------

QT       += core gui serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = geotest
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig
#PKGCONFIG += opencv
PKGCONFIG += \
    gstreamer-1.0 \
    gstreamer-base-1.0 \
#    gstreamer-interfaces-1.0 \
    gstreamer-video-1.0 \
    gstreamer-pbutils-1.0


INCLUDEPATH += MdbData \
            MSerial \



DEPENDPATH += MdbData \
            MSerial


HEADERS  += mainwindow.h \
    MSerial/mserial.h \
    MdbData/mdbdata.h \
    MdbData/common_name.h

SOURCES += main.cpp\
    mainwindow.cpp \
    MSerial/mserial.cpp \
    MdbData/mdbdata.cpp

        mainwindow.cpp


FORMS    += mainwindow.ui

LIBS += -lbcm2835
#LIBS += -L/opt/vc/lib -lmmal -lmmal_core -lmmal_util
#LIBS += -L/usr/local/lib -I/usr/local/include -lraspicam
LIBS += -lraspicam
LIBS += -L/usr/local/lib



# install
#target.path = root
target.path = /
INSTALLS += target

