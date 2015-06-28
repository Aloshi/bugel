#-------------------------------------------------
#
# Project created by QtCreator 2015-06-10T07:26:42
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bugel
TEMPLATE = app
CONFIG += c++11

QMAKE_CXXFLAGS += -wd4100

SOURCES += main.cpp\
        mainwindow.cpp \
    timelinelayerwidget.cpp \
    timeline.cpp \
    timelinecontainer.cpp \
    timelinewidget.cpp \
    playbackwidget.cpp \
    util.cpp \
    timelineeventswidget.cpp \
    noteevent.cpp

HEADERS  += mainwindow.h \
    timelinelayerwidget.h \
    timeline.h \
    timelinecontainer.h \
    timelinewidget.h \
    playbackwidget.h \
    util.h \
    timelineeventswidget.h \
    noteevent.h \
    selection.h

FORMS    += mainwindow.ui \
    timelinelayerwidget.ui \
    timelinecontainer.ui \
    playbackwidget.ui

