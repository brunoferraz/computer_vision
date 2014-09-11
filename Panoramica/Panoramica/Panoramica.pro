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
    debugset.cpp

HEADERS  += mainwindow.h \
    renderarea.h \
    cvlib.h \
    debugset.h

FORMS    += mainwindow.ui
