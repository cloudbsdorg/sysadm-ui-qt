TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on release
QT = core gui widgets
QMAKE_LIBDIR = /usr/local/lib/qt5 /usr/local/lib

TARGET=sysadm-client
target.path=/usr/local/bin

INSTALLS += target



RESOURCES += ../icons/icons.qrc \
			../styles/styles.qrc
			
SOURCES	+= main.cpp \
		mainUI.cpp
		
HEADERS += globals.h \
		mainUI.h

FORMS += mainUI.ui

include(../Core/core.pri)



