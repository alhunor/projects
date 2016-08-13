#-------------------------------------------------
#
# Project created by QtCreator 2016-07-23T13:25:13
#
#-------------------------------------------------

QT       += core gui \
        multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HuGo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    bullet.cpp \
    enemy.cpp \
    game.cpp \
    score.cpp \
    player.cpp \
    goview.cpp

HEADERS  += mainwindow.h \
    bullet.h \
    enemy.h \
    game.h \
    score.h \
    player.h \
    goview.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
