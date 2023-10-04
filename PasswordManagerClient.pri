# Important so that I can just type '#inlcude "qaesencryption.h' without specifying the folder the header file is in
INCLUDEPATH += $$PWD/libraries/QAES

LIBS += "-L$$PWD/libraries/QAES" -lqaesencryption


SOURCES += \
    $$PWD/unit_test/passwordbrokertest.cpp \
    dataentry.cpp \
    passwordbroker.cpp \
    view.cpp \
    main.cpp \
    unit_test/dataentrytest.cpp

HEADERS += \
    $$PWD/unit_test/passwordbrokertest.h \
    dataentry.h \
    passwordbroker.h \
    view.h \
    unit_test/dataentrytest.h

FORMS += \
    view.ui
