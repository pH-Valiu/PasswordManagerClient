#ifndef PASSWORDBROKERTEST_H
#define PASSWORDBROKERTEST_H

#include <QObject>
#include <QTest>
#include "dataentry.h"
#include "passwordbroker.h"

class PasswordBrokerTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void testSingleton();
    void testSimpleCallSequence();
    void testSingleStoreFetch();
    void testMultipleStoreFetch();
    void testChangeMasterPW();
    void testLookupEntry();
    void testWrongMasterPW();
    void cleanupTestCase(){}
private:
    QByteArray masterPW;
    QSharedPointer<DataEntry> testEntry1;
    QSharedPointer<DataEntry> testEntry2;
    QSharedPointer<DataEntry> testEntry3;


};

#endif // PASSWORDBROKERTEST_H
