TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../blindscan-s2.c \
    ../diseqc.c

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    ../blindscan-s2.h \
    ../diseqc.h

DISTFILES += \
    ../Makefile

