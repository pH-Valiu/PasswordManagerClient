#ifndef DATAENTRYTEST_H
#define DATAENTRYTEST_H

#ifdef EXECUTE_UNIT_TESTS
#include <QObject>
#include <QTest>
#include "dataentry.h"
#include "dataentrymodulator.h"

class DataEntryTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testBuilderFull();
    void testBuilderPartial();
    void testBuilderJSON();
    void testRegex();
    void testModulator();
    void testModulatorChangeMasterPW();
    void cleanupTestCase(){}

public:
    QSharedPointer<QByteArray> masterPW;
    QSharedPointer<DataEntry> appleEntry;

};
#endif

#endif // DATAENTRYTEST_H
