TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -L$$/usr/lib/ -lboost_random -lsfml-system -lsfml-window -lsfml-graphics -lGL

INCLUDEPATH += $/usr/include
DEPENDPATH += $/usr/include
#unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../../../../usr/lib/libboost_random.a


SOURCES += \
        main.cpp \
    outputopengl.cpp \
    langevin.cpp \
    defs.cpp \
    simulation.cpp \
    CHData.cpp \
    CahnHilliard.cpp \
    coeffs.cpp

HEADERS += \
    outputopengl.h \
    defs.h \
    langevin.h \
    defs.h \
    CahnHilliard.h \
    coeffs.h \
    CHData.h \
    simulation.h
