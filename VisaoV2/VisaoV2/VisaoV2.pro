#-------------------------------------------------
#
# Project created by QtCreator 2014-08-11T10:40:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VisaoV2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imagemanager.cpp \
    displaywindow.cpp \
    debugset.cpp

HEADERS  += mainwindow.h \
    imagemanager.h \
    displaywindow.h \
    debugset.h

FORMS    += mainwindow.ui \
    displaywindow.ui
