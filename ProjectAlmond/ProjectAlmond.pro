#-------------------------------------------------
#
# Project created by QtCreator 2015-04-05T23:54:38
#
#-------------------------------------------------

QT       += core gui widgets
CONFIG += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProjectAlmond
TEMPLATE = app


SOURCES += main.cpp\
    engine/osoba.cpp \
    engine/porodicnostablo.cpp \
    GUI/dialognovaosoba.cpp \
    GUI/glavniprozor.cpp \
    engine/dete.cpp \
    engine/brak.cpp \
    GUI/dijalogizmenaosobe.cpp \
    GUI/stablo.cpp \
    GUI/dijalogrelacija.cpp \
    GUI/gosoba.cpp \
    GUI/grelacija.cpp \
    alati/trazenjeputa.cpp \
    alati/uredjivanje.cpp \
    GUI/dijalogpretrage.cpp

HEADERS  += \
    engine/osoba.h \
    engine/porodicnostablo.h \
    GUI/dialognovaosoba.h \
    GUI/glavniprozor.h \
    engine/dete.h \
    engine/brak.h \
    GUI/dijalogizmenaosobe.h \
    GUI/stablo.h \
    GUI/dijalogrelacija.h \
    GUI/gosoba.h \
    GUI/grelacija.h \
    alati/trazenjeputa.h \
    alati/uredjivanje.h \
    GUI/dijalogpretrage.h

FORMS    += \
    GUI/dijalogizmenaosobe.ui \
    GUI/dijalogrelacija.ui \
    GUI/dialognovaosoba.ui \
    GUI/glavniprozor.ui \
    GUI/dijalogpretrage.ui

RESOURCES += \
    slike.qrc \
    jezici.qrc
TRANSLATIONS += \
    ProjectAlmond_en.ts \
    ProjectAlmond_de.ts \
    ProjectAlmond_es.ts \
    ProjectAlmond_sr.ts

OTHER_FILES +=
