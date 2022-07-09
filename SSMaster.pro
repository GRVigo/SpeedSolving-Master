QT       += core gui
# QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

RC_ICONS = Cube.ico

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE += -O3

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    algorithm.cpp \
    algset.cpp \
    ceor.cpp \
    cfop.cpp \
    collection.cpp \
    corners.cpp \
    cube.cpp \
    deep_eval.cpp \
    deep_search.cpp \
    edges.cpp \
    lbl.cpp \
    leor.cpp \
    main.cpp \
    mainwindow.cpp \
    mehta.cpp \
    method.cpp \
    nautilus.cpp \
    petrus.cpp \
    roux.cpp \
    searchthread.cpp \
    tinyxml2.cpp \
    zz.cpp

HEADERS += \
    algorithm.h \
    algset.h \
    ceor.h \
    cfop.h \
    collection.h \
    corners.h \
    cube.h \
    cube_definitions.h \
    deep_eval.h \
    deep_search.h \
    edges.h \
    lbl.h \
    leor.h \
    mainwindow.h \
    mehta.h \
    method.h \
    nautilus.h \
    petrus.h \
    roux.h \
    searchthread.h \
    tinyxml2.h \
    zz.h

FORMS += \
    algset.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    art.qrc
