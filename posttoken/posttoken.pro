TEMPLATE = app
TARGET = posttoken
CONFIG += console
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

PRE_TARGETDEPS = ../common/libcommon.a

INCLUDEPATH += ../common

LIBS += -L../common -lcommon

SOURCES += \
    posttoken.cpp
