#ifndef INTEGRITYCHECK_H
#define INTEGRITYCHECK_H

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QThread>
#include "dataentry.h"


class IntegrityCheckWorker : public QObject
{
    Q_OBJECT
public slots:
    void doWork(const QSharedPointer<QByteArray>& masterPW, const QVector<QSharedPointer<DataEntry>>& entries);
private:
    QByteArray masterPW;
    /**
     * @brief tuples with (midKey, midKeySalt, ivMidKey, midKeyHash)
     */
    QList<struct DataEntry::IntegrityCheckData> data;
signals:
    /**
     * @brief workFinished
     * @param returnCode:
     *
     * 0 if entries and masterPW are ok
     *
     * 1 if entries and/or masterPW are not ok
     */
    void workFinished(int returnCode);
};



class IntegrityCheckController : public QObject
{
    Q_OBJECT
public:
    IntegrityCheckController();
    ~IntegrityCheckController();
public slots:
    void checkIntegrity(const QSharedPointer<QByteArray>& masterPW, const QVector<QSharedPointer<DataEntry>>& entries);
private:
    QThread integrityCheckThread;
signals:
    void integrityCheckFinished(int returnCode);
    void operate(const QSharedPointer<QByteArray>& masterPW, const QVector<QSharedPointer<DataEntry>>& entries);
};

#endif // INTEGRITYCHECK_H
