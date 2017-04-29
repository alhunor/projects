#-------------------------------------------------
#
# Project created by QtCreator 2015-12-27T20:53:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += $$PWD/../mystuff \
	       $$PWD/../3rdParty/boost_1_60_0

# LIBS += -L$$OUT_PWD/../3rdParty/boost_1_60_0/lib32-msvc-14.0/libboost_date_time-vc100-mt-gd-1_60.lib
LIBS += L:/Projects/3rdParty/boost_1_60_0/lib32-msvc-10.0/libboost_date_time-vc100-mt-gd-1_60.lib \
    Comdlg32.lib


TARGET = FileFinder
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mymodel.cpp \
    ../mystuff/editDistance.cpp \
    ../mystuff/filesystems.cpp \
    ../mystuff/dates.cpp

HEADERS  += mainwindow.h \
    mymodel.h \
    ../mystuff/editDistance.h \
    ../mystuff/filesystems.h \
    ../mystuff/mutex.h \
    ../mystuff/dates.h

FORMS    += mainwindow.ui
