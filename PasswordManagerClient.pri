# Important so that I can just type '#inlcude "qaesencryption.h' without specifying the folder the header file is in
INCLUDEPATH += $$PWD/libraries/QAES

LIBS += "-L$$PWD/libraries/QAES" -lqaesencryption


SOURCES += \
    dataentry.cpp \
    main.cpp \
    passwordbroker.cpp \
    view.cpp \
    unit_test/dataentrytest.cpp

HEADERS += \
    dataentry.h \
    passwordbroker.h \
    view.h \
    unit_test/dataentrytest.h

FORMS += \
    view.ui
