TARGET = NextBasic
DESTDIR = $$_PRO_FILE_PWD_/_build
TEMPLATE = app
CONFIG += qt
#QT += gui
QT += widgets

win32:LIBS += -lwinmm
mac:LIBS += -framework AppKit

#DEFINES += USE_LOG
#DEFINES += USE_COMPILE_CONTROLLER

SOURCES += \
    src/textarea.cpp \
    src/maincontroller.cpp \
    src/main.cpp \
    src/guiloader.cpp \
    src/application.cpp \
    src/config.cpp \
    src/language.cpp \
    src/log.cpp \
    src/compilecontroller.cpp \
    src/findcontroller.cpp \
    src/time.c \
    src/qtiny/eventdispatcher.cpp \
    src/qtiny/qtiny.cpp \
    src/qtiny/moc_eventdispatcher.cpp

HEADERS += \
    src/textarea.h \
    src/stringutils.hpp \
    src/rapidxml.hpp \
    src/maincontroller.h \
    src/guiloader.h \
    src/controller.h \
    src/application.h \
    src/config.h \
    src/language.h \
    src/log.h \
    src/compilecontroller.h \
    src/findcontroller.h \
    src/time.h \
    src/qtiny/qtiny.h

OTHER_FILES += \
    todo.txt

