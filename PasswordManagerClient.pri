# Important so that I can just type '#inlcude "qaesencryption.h' without specifying the folder the header file is in
INCLUDEPATH += \
    $$PWD/libraries/QAES \
    $$PWD/libraries/WinCrypt\
    $$PWD/backend \
    $$PWD/gui

LIBS += \
    "-L$$PWD/libraries/QAES" -lqaesencryption \
    "-L$$PWD/libraries/WinCrypt" -lcrypt32


SOURCES += \
    $$PWD/backend/dataentrymodulator.cpp \
    $$PWD/gui/advancedpushbutton.cpp \
    $$PWD/gui/dataentrymodulatordialog.cpp \
    $$PWD/gui/initialsetupdialog.cpp \
    $$PWD/gui/passwordmanagerview.cpp \
    $$PWD/gui/dataentrywidget.cpp \
    $$PWD/gui/settingsdialog.cpp \
    $$PWD/gui/startupdialog.cpp \
    $$PWD/backend/messagehandler.cpp \
    $$PWD/backend/passwordmanageradapter.cpp \
    $$PWD/backend/passwordmanagermodel.cpp \
    $$PWD/unit_test/passwordbrokertest.cpp \
    $$PWD/unit_test/dataentrytest.cpp \
    $$PWD/backend/dataentry.cpp \
    $$PWD/backend/passwordbroker.cpp \
    $$PWD/main.cpp

HEADERS += \
    $$PWD/backend/dataentrymodulator.h \
    $$PWD/constants.h \
    $$PWD/gui/advancedpushbutton.h \
    $$PWD/gui/dataentrymodulatordialog.h \
    $$PWD/gui/initialsetupdialog.h \
    $$PWD/gui/passwordmanagerview.h \
    $$PWD/gui/dataentrywidget.h \
    $$PWD/gui/settingsdialog.h \
    $$PWD/gui/startupdialog.h \
    $$PWD/backend/localbackup.h \
    $$PWD/backend/messagehandler.h \
    $$PWD/backend/passwordmanageradapter.h \
    $$PWD/backend/passwordmanagermodel.h \
    $$PWD/unit_test/passwordbrokertest.h \
    $$PWD/unit_test/dataentrytest.h \
    $$PWD/backend/dataentry.h \
    $$PWD/backend/passwordbroker.h

FORMS +=

DISTFILES += \
    $$PWD/gui/ico/copy.ico \
    $$PWD/gui/ico/save.ico \
    $$PWD/gui/ico/edit.ico \
    $$PWD/gui/ico/show.ico \
    $$PWD/gui/ico/dont-show.ico \
    $$PWD/gui/ico/delete.ico \
    $$PWD/gui/ico/search.ico \
    $$PWD/gui/ico/add.ico \
    $$PWD/gui/ico/enter.ico \
    $$PWD/gui/ico/backup.ico \
    $$PWD/gui/ico/settings.ico \
    $$PWD/gui/ico/remote.ico \
    $$PWD/gui/ico/vault-shield.ico
