#-------------------------------------------------
#
# Project created by QtCreator 2020-01-30T16:49:09
#
#-------------------------------------------------

QT              += core gui charts widgets

CONFIG += c++17

QMAKE_CXXFLAGS  += -fpermissive

linux: {
	QMAKE_CXXFLAGS  += -I/usr/include/postgresql
	QMAKE_LFLAGS    += -L/usr/lib/x86_64-linux-gnu
}

win32: {
    QMAKE_CXXFLAGS  += -I../postgresql/include
    QMAKE_LFLAGS    += -L../postgresql/lib
}

LIBS    += -lpq #-lstdc++fs

INCLUDEPATH += $$PWD/src

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

SOURCES += \
	src/main.cpp \
	src/patient/patient.cpp \
    src/calendar/calendar.cpp \
    src/common/qutils.cpp \
    src/menubar/addappointmentform.cpp \
    src/menubar/addpatientform.cpp \
    src/calendar/patientlineedit.cpp \
    src/appointment/appointmentwidget.cpp \
    src/menubar/edittabmodel.cpp \
    src/patient/patientdelegate.cpp \
    src/common/db.cpp \
    src/patient/patientmodel.cpp \
    src/widgets/timebutton.cpp \
    src/autosave/autosavetextedit.cpp \
    src/autosave/autosaverestore.cpp \
    src/patient/developmentcurvechart.cpp \
    src/menubar/settingsform.cpp

HEADERS += \
	src/patient/patient.h \
    src/calendar/calendar.h \
    src/common/qutils.h \
    src/menubar/addappointmentform.h \
    src/menubar/addpatientform.h \
    src/calendar/patientlineedit.h \
    src/appointment/appointmentwidget.h \
    src/menubar/edittabmodel.h \
    src/patient/patientdelegate.h \
    src/common/db.h \
    src/patient/patientmodel.h \
    src/widgets/timebutton.h \
    src/autosave/autosavetextedit.h \
    src/autosave/autosaverestore.h \
    src/patient/developmentcurvechart.h \
    src/menubar/settingsform.h

FORMS += \
	src/calendar/calendar.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    .gitignore

RESOURCES += \
    assets/images.qrc
