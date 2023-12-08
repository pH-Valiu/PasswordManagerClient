#include "passwordmanageradapter.h"

#include <QFile>
#include <QApplication>

#include "masterpasswordmonitor.h"

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
    QTest::qExec(&dataEntryTest);
    QTest::qExec(&passwordBrokerTest);
    qDebug()<<"FINISHED";
    // :)
    return 0;
#else
    PasswordManagerAdapter adapter;
    int retCode = adapter.start();
    switch(retCode){
    case 0:
        //all fine nice
        break;
    case 1:
        //new user -> all fine
        break;
    case 2:
        //error -> quit
        return 2;
    }
    a.setApplicationName("Vault Shield");

    QSharedPointer<QByteArray> ptr = QSharedPointer<QByteArray>(new QByteArray(QString("12345678901234567890123456789012").toUtf8()));
    MasterPasswordMonitor m(ptr);
    m.example();

    return a.exec();

#endif
}
