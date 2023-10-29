# Important so that I can just type '#inlcude "qaesencryption.h' without specifying the folder the header file is in
INCLUDEPATH += \
    $$PWD/libraries/QAES \
    $$PWD/libraries/WinCrypt

LIBS += \
    "-L$$PWD/libraries/QAES" -lqaesencryption \
    "-L$$PWD/libraries/WinCrypt" -lcrypt32


SOURCES += \
    $$PWD/dataentrymodulator.cpp \
    $$PWD/gui/dataentrymodulatordialog.cpp \
    $$PWD/gui/passwordmanagerview.cpp \
    $$PWD/gui/dataentrywidget.cpp \
    $$PWD/messagehandler.cpp \
    $$PWD/passwordmanageradapter.cpp \
    $$PWD/passwordmanagermodel.cpp \
    $$PWD/unit_test/passwordbrokertest.cpp \
    $$PWD/unit_test/dataentrytest.cpp \
    $$PWD/dataentry.cpp \
    $$PWD/passwordbroker.cpp \
    $$PWD/main.cpp

HEADERS += \
    $$PWD/dataentrymodulator.h \
    $$PWD/gui/dataentrymodulatordialog.h \
    $$PWD/gui/passwordmanagerview.h \
    $$PWD/gui/dataentrywidget.h \
    $$PWD/localbackup.h \
    $$PWD/messagehandler.h \
    $$PWD/passwordmanageradapter.h \
    $$PWD/passwordmanagermodel.h \
    $$PWD/unit_test/passwordbrokertest.h \
    $$PWD/unit_test/dataentrytest.h \
    $$PWD/dataentry.h \
    $$PWD/passwordbroker.h

FORMS +=

DISTFILES += \
    $$PWD/gui/ico/copy.ico \
    $$PWD/gui/ico/save.ico \
    $$PWD/gui/ico/edit.ico \
    $$PWD/gui/ico/show.ico \
    $$PWD/gui/ico/dont-show.ico \
    $$PWD/gui/ico/delete.ico \
    $$PWD/gui/ico/search.ico \
    $$PWD/gui/ico/add.ico
