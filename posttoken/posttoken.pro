TEMPLATE = app
TARGET = posttoken
CONFIG += console
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += ../common

LIBS += -L../common -lcommon

SOURCES += \
    posttoken.cpp