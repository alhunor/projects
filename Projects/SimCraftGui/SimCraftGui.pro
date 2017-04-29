#-------------------------------------------------
#
# Project created by QtCreator 2016-04-15T23:08:15
#
#-------------------------------------------------

DEFINES += QTHUNOR

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = SimCraftGui
TEMPLATE = app

INCLUDEPATH += $$PWD/../mystuff \
               $$PWD/../3rdParty/boost_1_60_0

# LIBS += -L$$OUT_PWD/../3rdParty/boost_1_60_0/lib32-msvc-14.0/libboost_date_time-vc100-mt-gd-1_60.lib
LIBS += L:/Projects/3rdParty/boost_1_60_0/lib32-msvc-10.0/libboost_date_time-vc100-mt-gd-1_60.lib \
    Comdlg32.lib


SOURCES += main.cpp\
        mainwindow.cpp \
    arcanemage.cpp \
    combatLog.cpp \
    ../mystuff/dates.cpp \
    ../mystuff/filesystems.cpp \
    ../mystuff/tokenizer.cpp \
    ../mystuff/utils.cpp \
    ../mystuff/editDistance.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    arcanemage.h \
    ../simCraftLog/combatlog.h \
    combatlog.h \
    ../mystuff/dates.h \
    ../mystuff/filesystems.h \
    ../mystuff/tokenizer.h \
    ../mystuff/utils.h \
    ../mystuff/editDistance.h \
    qcustomplot.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc

DISTFILES += \
    combatlog-elite.txt
