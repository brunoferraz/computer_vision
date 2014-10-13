#-------------------------------------------------
#
# Project created by QtCreator 2014-09-03T12:25:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Panoramica
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    renderarea.cpp \
    cvlib.cpp \
    debugset.cpp \
    resultwindow.cpp \
    pairs.cpp

HEADERS  += mainwindow.h \
    renderarea.h \
    cvlib.h \
    debugset.h \
    resultwindow.h \
    pairs.h

FORMS    += mainwindow.ui \
    resultwindow.ui

INCLUDEPATH += /include

QMAKE_CXXFLAGS += -std=c++11

unix:!macx: LIBS += -lopencv_core
unix:!macx: LIBS += -lopencv_imgproc
unix:!macx: LIBS += -lopencv_features2d
unix:!macx: LIBS += -lopencv_nonfree
unix:!macx: LIBS += -lopencv_flann
