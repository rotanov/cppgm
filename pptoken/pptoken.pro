TEMPLATE = app
CONFIG += console
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
    pptoken.cpp

HEADERS += \
    DebugPPTokenStream.h \
    IPPTokenStream.h \
    Unicode.hpp \
