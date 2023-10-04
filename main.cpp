#include "view.h"
#include "passwordbroker.h"
#include "qaesencryption.h"

#include <QFile>
#include <QTextStream>
#include <QApplication>

#ifdef EXECUTE_UNIT_TESTS
#include "unit_test/dataentrytest.h"
#endif


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifdef EXECUTE_UNIT_TESTS
    DataEntryTest dataEntryTest;
    return QTest::qExec(&dataEntryTest);
#else
    view w;
    QSharedPointer<PasswordBroker> broker = PasswordBroker::getInstance();
    qDebug() <<"return: "<<broker->fetchFileData();

    w.show();
    return a.exec();
#endif
}
