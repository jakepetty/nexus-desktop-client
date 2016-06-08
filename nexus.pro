#-------------------------------------------------
#
# Project created by QtCreator 2015-11-04T09:59:51
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = nexus
TEMPLATE = app
LIBS += -lUser32

SOURCES += main.cpp \
           $$files(LoginDialog/*.cpp) \
           $$files(UpdateDialog/*.cpp) \
           $$files(SignupDialog/*.cpp) \
           $$files(HandlesDialog/*.cpp) \
           $$files(RealTimeWindow/*.cpp) \
           $$files(PreferencesWindow/*.cpp) \
           $$files(SingleApplication/*.cpp) \
           $$files(ScreenshotDialog/*.cpp) \
           $$files(FleetDialog/*.cpp) \
           $$files(SetupWizard/*.cpp) \
           $$files(ExistingDialog/*.cpp)

HEADERS += app.h \
           common.h \
           structs.h \
           $$files(Website/*.h) \
           $$files(TrayIcon/*.h) \
           $$files(LoginDialog/*.h) \
           $$files(Compression/*.h) \
           $$files(UpdateDialog/*.h) \
           $$files(SignupDialog/*.h) \
           $$files(HandlesDialog/*.h) \
           $$files(RealTimeWindow/*.h) \
           $$files(StarTrekOnline/*.h) \
           $$files(PreferencesWindow/*.h) \
           $$files(BackgroundWorkers/*.h) \
           $$files(SingleApplication/*.h) \
           $$files(ScreenshotDialog/*.h) \
           $$files(FleetDialog/*.h) \
           $$files(SetupWizard/*.h) \
           $$files(ExistingDialog/*.h)

FORMS += $$files(LoginDialog/*.ui) \
         $$files(updatedialog/*.ui) \
         $$files(SignupDialog/*.ui) \
         $$files(HandlesDialog/*.ui) \
         $$files(RealTimeWindow/*.ui) \
         $$files(PreferencesWindow/*.ui) \
         $$files(ScreenshotDialog/*.ui) \
         $$files(FleetDialog/*.ui) \
         $$files(SetupWizard/*.ui) \
         $$files(ExistingDialog/*.ui)

#RC_FILE = nexus.rc
RESOURCES += nexus.qrc
QMAKE_LFLAGS += /INCREMENTAL:NO

VERSION = 1.0.0.7
DEFINES += VERSION_STR='"\\\"$$VERSION\\\""'
QMAKE_TARGET_COMPANY = "STO-Nexus"
QMAKE_TARGET_PRODUCT = "STO-Nexus Desktop Client"
QMAKE_TARGET_DESCRIPTION = "Desktop Client for STO-Nexus"
QMAKE_TARGET_COPYRIGHT = "Copyright 2016 STO-Nexus.com"
win32:RC_ICONS += "images\\favicon.ico"

