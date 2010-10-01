# -------------------------------------------------
# Project created by QtCreator 2010-04-25T11:58:01
# -------------------------------------------------
HELI = $(HELICONNECT)
QT += network \
    opengl
CONFIG += debug
TARGET = gcs
TEMPLATE = app
OBJECTS_DIR = obj/
UI_DIR = ui
UI_HEADERS_DIR = tmp
UI_SOURCES_DIR = tmp
RCC_DIR = tmp
MOC_DIR = tmp
linux-g++ { 
    message(QMAKE BUILDING FOR LINUX)
    INCLUDEPATH += . \
        include \
        AHWidget/include \
        reuse/ \
        /usr/include/qwt-qt4/ \
        $$HELI \
        udp/
    LIBS += -lQtNetwork \
        -lglut \
        -lGLU \
        -lqwt \
        -L/usr/lib \
        -lViconDataStreamSDK_CPP \
        -lgsl \
        -lgslcblas
}
macx-g++ { 
    message(QMAKE BUIDING FOR MACOSX)
    QWT_ROOT = /usr/local/qwt-5.2.1-svn/
    QWTLIB = qwt
    INCLUDEPATH += . \
        include \
        AHWidget/include \
        reuse/ \
        /usr/local/qwt-5.2.1-svn/include/ \
        $$HELI \
        udp/
    LIBS += -L/usr/local/qwt-5.2.1-svn/lib \
        -lqwt \
        -framework \
        GLUT
}
SOURCES += src/main.cpp \
    src/gcsmainwindow.cpp \
    src/systemstatus.cpp \
    AHWidget/src/ATT_text.cpp \
    AHWidget/src/ATT_Element.cpp \
    AHWidget/src/ALT_text.cpp \
    AHWidget/src/AH_Utility.cpp \
    AHWidget/src/AH_Motion.cpp \
    AHWidget/src/AH.cpp \
    $$HELI/state.c \
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
    src/datalogger.cpp \
    src/flightcontrol.cpp \
    src/gainscontrol.cpp \
    src/parametercontrol.cpp \
    $$HELI/primitive_serialisation.c \
    src/transmitconsole.cpp \
    src/viconthread.cpp
HEADERS += include/gcsmainwindow.h \
    include/systemstatus.h \
    AHWidget/include/AH.h \
    AHWidget/include/AH.h \
    reuse/ahns_logger.h \
    $$HELI/commands.h \
    $$HELI/state.h \
    include/aboutform.h \
    include/wificomms.h \
    include/telemetrythread.h \
    reuse/ahns_timeformat.h \
    include/receiveconsole.h \
    include/dataplotter.h \
    include/ioimagehandler.h \
    include/bfimagefeed.h \
    include/bfglrenderer.h \
    include/bfcameracomms.h \
    $$HELI/primitive_serialisation.h \
    include/datalogger.h \
    include/flightcontrol.h \
    include/gainscontrol.h \
    include/parametercontrol.h \
    include/transmitconsole.h \
    include/viconthread.h \
    include/Client.h
FORMS += ui/gcsmainwindow.ui \
    ui/systemstatus.ui \
    ui/aboutform.ui \
    ui/wificomms.ui \
    ui/receiveconsole.ui \
    ui/dataplotter.ui \
    ui/flightcontrol.ui \
    ui/gainscontrol.ui \
    ui/parametercontrol.ui \
    ui/transmitconsole.ui
RESOURCES += resources/gcs.qrc
