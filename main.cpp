#include "view.h"
#include "passwordbroker.h"
#include "qaesencryption.h"

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
    DataEntryTest dataEntryTest;
    PasswordBrokerTest passwordBrokerTest;
    QTest::qExec(&passwordBrokerTest);
    QTest::qExec(&dataEntryTest);
    // :)
    return 0;
#else
    view w;
    PasswordBroker& broker = PasswordBroker::getInstance();
    qDebug() <<"return: "<<broker.fetchFileData();

    w.show();
    return a.exec();
#endif
}
