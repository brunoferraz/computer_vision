#-------------------------------------------------
#
# Project created by QtCreator 2014-07-01T16:09:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VisaoExercicio
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imageviewer.cpp \
    pin.cpp \
    pinmanager.cpp \
    line.cpp \
    cvlib.cpp \
    imageviewer_ex2.cpp

HEADERS  += mainwindow.h \
    imageviewer.h \
    pin.h \
    pinmanager.h \
    line.h \
    cvlib.h \
    imageviewer_ex2.h

FORMS    += mainwindow.ui
