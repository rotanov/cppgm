TEMPLATE = lib
TARGET = common
CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++11

HEADERS += \
	Unicode.hpp \
    Tokenizer.hpp \
    Utils.hpp \
    Constants.hpp \
    DebugPPTokenStream.hpp \
    IPPTokenStream.hpp \
    PostTokenStream.hpp \
    DebugPostTokenOutputStream.hpp

SOURCES += \
    Utils.cpp \
    PostTokenStream.cpp \
    Unicode.cpp
