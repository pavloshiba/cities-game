TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    pssocket.cpp \
    cities.cpp \
    gamebot.cpp \
    citiesapp.cpp\
    bots.cpp \
    BotManager.cpp

HEADERS += \
    pssocket.h \
    asserts.h \
    platform.h \
    types.h \
    cities.h \
    gamebot.h \
    citiesapp.h \
    macroses.h \
    bots.h \
    BotManager.h
