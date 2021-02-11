#-------------------------------------------------
#
# Project created by QtCreator 2020-01-30T16:49:09
#
#-------------------------------------------------

QT              += core gui

QMAKE_CXXFLAGS  += -fpermissive -std=c++2a

linux: {
	QMAKE_CXXFLAGS  += -I/usr/include/postgresql
	QMAKE_LFLAGS    += -L/usr/lib/x86_64-linux-gnu
}

win32: {
    QMAKE_CXXFLAGS  += -I../postgresql/include
    QMAKE_LFLAGS    += -L../postgresql/lib
}

QMAKE_LFLAGS    += -lpq

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RaqMed
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++2a

SOURCES += \
	main.cpp \
	patient.cpp \
    calendar.cpp \
    qutils.cpp \
    addappointmentform.cpp \
    addpatientform.cpp \
    patientlineedit.cpp \
    appointmentwidget.cpp \
    edittabmodel.cpp \
    patientdelegate.cpp \
    db.cpp \
    patientmodel.cpp \
    timebutton.cpp

HEADERS += \
	patient.h \
    calendar.h \
    qutils.h \
    addappointmentform.h \
    addpatientform.h \
    patientlineedit.h \
    appointmentwidget.h \
    edittabmodel.h \
    patientdelegate.h \
    db.h \
    patientmodel.h \
    timebutton.h

FORMS += \
	calendar.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    database.config \
    ddl.sql \
	notes.txt \
    .gitignore

RESOURCES += \
    images.qrc
