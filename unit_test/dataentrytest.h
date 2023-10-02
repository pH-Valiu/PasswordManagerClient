#ifndef DATAENTRYTEST_H
#define DATAENTRYTEST_H

#include <QObject>
#include <QTest>
#include "dataentry.h"

class DataEntryTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testBuilderFull();
    void testBuilderJSON();
    void cleanupTestCase(){}

public:
    QByteArray masterPW;
    QSharedPointer<DataEntry> appleEntry;

};

#endif // DATAENTRYTEST_H
