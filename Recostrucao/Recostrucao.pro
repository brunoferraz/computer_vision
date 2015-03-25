#-------------------------------------------------
#
# Project created by QtCreator 2014-10-15T14:25:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Recostrucao
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    util.cpp \
    debugset.cpp \
    imagecanvas.cpp \
    pin.cpp \
    pair.cpp

HEADERS  += mainwindow.h \
    util.h \
    debugset.h \
    imagecanvas.h \
    pin.h \
    pair.h

FORMS    += mainwindow.ui

INCLUDEPATH += /include

QMAKE_CXXFLAGS += -std=c++11

unix:!macx: LIBS += -lopencv_core
unix:!macx: LIBS += -lopencv_imgproc
unix:!macx: LIBS += -lopencv_features2d
unix:!macx: LIBS += -lopencv_nonfree
unix:!macx: LIBS += -lopencv_flann
