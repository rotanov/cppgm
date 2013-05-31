TEMPLATE = lib
TARGET = common
CONFIG = staticlib

QMAKE_CXXFLAGS += -std=c++11

HEADERS += \
	Unicode.hpp

SOURCES += \
    pptoken.cpp