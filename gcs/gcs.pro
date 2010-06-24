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
    /usr/include/qwt-qt4/ \
    ../../../heliconnect10/trunk/udp/ \
    udp/
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
    ../udp/src/little_host_serialisation.c \
    ../../../heliconnect10/trunk/udp/state.c \
    reuse/ahns_logger.cpp \
    src/aboutform.cpp \
    src/wificomms.cpp \
    src/telemetrythread.cpp \
    reuse/ahns_timeformat.cpp \
    src/receiveconsole.cpp \
    src/gcsmessages.cpp \
    src/dataplotter.cpp \
    src/bfglrenderer.cpp \
    src/bfcameracomms.cpp \
    src/bfimagefeed.cpp \
    src/datalogger.cpp
HEADERS += include/gcsmainwindow.h \
    include/systemstatus.h \
    AHWidget/include/AH.h \
    AHWidget/include/AH.h \
    reuse/ahns_logger.h \
    ../../../heliconnect10/trunk/udp/commands.h \
    ../../../heliconnect10/trunk/udp/state.h \
    include/aboutform.h \
    include/wificomms.h \
    include/telemetrythread.h \
    reuse/ahns_timeformat.h \
    include/receiveconsole.h \
    include/dataplotter.h \
    include/ioimagehandler.h \
    include/bfimagefeed.h \
    include/bfglrenderer.h \
    src/bfimagefeed.h \
    include/bfcameracomms.h \
    src/bfimagefeed.h \
    src/bfimagefeed.h \
    ../udp/include/primitive_serialisation.h \
    include/datalogger.h
FORMS += ui/gcsmainwindow.ui \
    ui/systemstatus.ui \
    ui/aboutform.ui \
    ui/wificomms.ui \
    ui/receiveconsole.ui \
    ui/dataplotter.ui
RESOURCES += resources/gcs.qrc
