QT += core gui network widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 gui windows
CONFIG += depend_includepath
CONFIG -= debug_and_release

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include($$PWD/PasswordManagerClient.pri)

TARGET = VaultShield
VERSION = 1.0.0.2
QMAKE_TARGET_COPYRIGHT = GNU GPL 3.0
RC_ICONS = "/gui/ico/vault-shield.ico"


# Un-Comment this line when you want to test your code
# Comment this line when you want to deploy your code
# ATTENTION: Executing unit test will delete all your DataEntries, therefore all your passwords aswell
# Please save your databse folder somewhere else first
# Add 'testlib' to QT += if you want to run the tests
#DEFINES += EXECUTE_UNIT_TESTS




# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# When deploying: take the .exe and get all the dlls needed (DependenciesGUI) --> QT/6.2.2/mingw_64/bin
# If you still can't start it, make you sure you got a folder 'platforms' parallel to your .exe with qwindows.dll inside the folder

DISTFILES += \
    PasswordManagerClient.pri

