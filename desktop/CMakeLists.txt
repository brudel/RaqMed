cmake_minimum_required(VERSION 3.21)

project(RaqMed VERSION 1.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(UNIX AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Install prefix" FORCE)
endif()

include(GNUInstallDirs)

find_package(Qt6 6.7 REQUIRED COMPONENTS Widgets Charts)
find_package(PostgreSQL REQUIRED)

# Where curves.csv is read from at runtime.
if(WIN32)
  set(_raqmed_default_datadir ".")
else()
  set(_raqmed_default_datadir "${CMAKE_INSTALL_FULL_DATADIR}/raqmed")
endif()
set(RAQMED_DATADIR "${_raqmed_default_datadir}" CACHE STRING
    "Directory curves.csv is read from")
option(RAQMED_LOCAL_DATA "Test build: read curves.csv from the source tree" OFF)
if(RAQMED_LOCAL_DATA)
  set(RAQMED_DATADIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
endif()

add_executable(raqmed
  src/main.cpp
  src/appointment/appointmentwidget.cpp
  src/appointment/appointmentwidget.h
  src/autosave/autosaverestore.cpp
  src/autosave/autosaverestore.h
  src/autosave/autosavetextedit.cpp
  src/autosave/autosavetextedit.h
  src/calendar/calendar.cpp
  src/calendar/calendar.h
  src/calendar/calendar.ui
  src/calendar/patientlineedit.cpp
  src/calendar/patientlineedit.h
  src/common/db.cpp
  src/common/db.h
  src/common/qutils.cpp
  src/common/qutils.h
  src/menubar/addappointmentform.cpp
  src/menubar/addappointmentform.h
  src/menubar/addpatientform.cpp
  src/menubar/addpatientform.h
  src/menubar/edittabmodel.cpp
  src/menubar/edittabmodel.h
  src/menubar/settingsform.cpp
  src/menubar/settingsform.h
  src/patient/developmentcurvechart.cpp
  src/patient/developmentcurvechart.h
  src/patient/patient.cpp
  src/patient/patient.h
  src/patient/patientdelegate.cpp
  src/patient/patientdelegate.h
  src/patient/patientmodel.cpp
  src/patient/patientmodel.h
  src/widgets/timebutton.cpp
  src/widgets/timebutton.h
  assets/images.qrc
)

target_include_directories(raqmed PRIVATE src)

target_compile_definitions(raqmed PRIVATE
  QT_DEPRECATED_WARNINGS
  DATADIR="${RAQMED_DATADIR}"
)

if(NOT MSVC)
  target_compile_options(raqmed PRIVATE -fpermissive)
endif()

target_link_libraries(raqmed PRIVATE
  Qt6::Widgets
  Qt6::Charts
  PostgreSQL::PostgreSQL
)

install(TARGETS raqmed RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES assets/curves.csv DESTINATION ${CMAKE_INSTALL_DATADIR}/raqmed)
install(FILES assets/icon.png
  DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps
  RENAME raqmed.png)
install(FILES assets/raqmed.desktop
  DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE
  DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/raqmed
  RENAME copyright)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "raqmed")
set(CPACK_PACKAGE_CONTACT "brudel <brudel13742@gmail.com>")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/brudel/RaqMed")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Suíte de gerenciamento de pacientes RaqMed.")
set(CPACK_DEBIAN_PACKAGE_SECTION "misc")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
# Fallback when dpkg-shlibdeps is unavailable (e.g. building on NixOS)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libpq5, libqt6widgets6, libqt6gui6, libqt6core6, libqt6charts6")
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
endif()
include(CPack)
