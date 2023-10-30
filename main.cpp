#include "passwordmanageradapter.h"

#include <QFile>
#include <QTextStream>
#include <QApplication>

#ifdef EXECUTE_UNIT_TESTS
#include "unit_test/dataentrytest.h"
#include "unit_test/passwordbrokertest.h"
#endif



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifdef EXECUTE_UNIT_TESTS
    //ATTENTION
    //Executing this code deletes the current database folder
    DataEntryTest dataEntryTest;
    PasswordBrokerTest passwordBrokerTest;
    QTest::qExec(&passwordBrokerTest);
    QTest::qExec(&dataEntryTest);
    qDebug()<<"finished";
    // :)
    return 0;
#else
    PasswordManagerAdapter adapter;

    return a.exec();
#endif
}
