# -------------------------------------------------
# Project created by QtCreator 2010-04-25T11:58:01
# -------------------------------------------------
QT += opengl \
    network
CONFIG += debug
TARGET = gcs
TEMPLATE = app
OBJECTS_DIR = obj/
INCLUDEPATH += . \
    include \
    AHWidget/include \
    reuse/ \
    ../../../heliconnect10/trunk/
UI_DIR = ui
UI_HEADERS_DIR = tmp
UI_SOURCES_DIR = tmp
RCC_DIR = tmp
MOC_DIR = tmp
LIBS += -lQtNetwork \
    -lglut \
    -lGLU \
    -lqwt
SOURCES += src/main.cpp \
    src/gcsmainwindow.cpp \
    src/systemstatus.cpp \
    AHWidget/src/ATT_text.cpp \
    AHWidget/src/ATT_Element.cpp \
    AHWidget/src/ALT_text.cpp \
    AHWidget/src/AH_Utility.cpp \
    AHWidget/src/AH_Motion.cpp \
    AHWidget/src/AH.cpp \
    reuse/ahns_logger.cpp \
    src/aboutform.cpp \
    src/wificomms.cpp \
    src/telemetrythread.cpp
HEADERS += include/gcsmainwindow.h \
    include/systemstatus.h \
    AHWidget/include/AH.h \
    AHWidget/include/AH.h \
    reuse/ahns_logger.h \
    include/aboutform.h \
    include/wificomms.h \
    include/telemetrythread.h \
    ../../../heliconnect10/trunk/state.h \
    ../../../heliconnect10/trunk/commands.h
FORMS += ui/gcsmainwindow.ui \
    ui/systemstatus.ui \
    ui/aboutform.ui \
    ui/wificomms.ui
RESOURCES += resources/gcs.qrc
