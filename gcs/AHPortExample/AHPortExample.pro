# -------------------------------------------------
# Project created by QtCreator 2010-03-26T10:34:51
# -------------------------------------------------
QT += opengl
TARGET = AHPortExample
TEMPLATE = app
OBJECTS_DIR = obj/

UI_DIR = ui/
UI_HEADERS_DIR = tmp
UI_SOURCES_DIR = tmp
MOC_DIR = tmp

INCLUDEPATH += . \
    AHWidget/include \
    /usr/include/qwt-qt4 \
    include 
LIBS += -lQtNetwork \
    -lglut \
    -lGLU \
    -lqwt
SOURCES += src/main.cpp \
    src/ahportexample.cpp \
    AHWidget/src/AH.cpp \
    AHWidget/src/AH_Motion.cpp \
    AHWidget/src/ALT_text.cpp \
    AHWidget/src/ATT_Element.cpp \
    AHWidget/src/ATT_text.cpp \
    AHWidget/src/AH_Utility.cpp
HEADERS += include/ahportexample.h \
    AHWidget/include/AH.h
FORMS += ui/ahportexample.ui
