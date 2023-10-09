# Important so that I can just type '#inlcude "qaesencryption.h' without specifying the folder the header file is in
INCLUDEPATH += $$PWD/libraries/QAES

LIBS += "-L$$PWD/libraries/QAES" -lqaesencryption


SOURCES += \
    $$PWD/messagehandler.cpp \
    $$PWD/passwordmanagermodel.cpp \
    $$PWD/unit_test/passwordbrokertest.cpp \
    $$PWD/unit_test/dataentrytest.cpp \
    $$PWD/dataentry.cpp \
    $$PWD/passwordbroker.cpp \
    $$PWD/view.cpp \
    $$PWD/main.cpp

HEADERS += \
    $$PWD/localbackup.h \
    $$PWD/messagehandler.h \
    $$PWD/passwordmanagermodel.h \
    $$PWD/unit_test/passwordbrokertest.h \
    $$PWD/unit_test/dataentrytest.h \
    $$PWD/dataentry.h \
    $$PWD/passwordbroker.h \
    $$PWD/view.h

FORMS += \
    $$PWD/view.ui
