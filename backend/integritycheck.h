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
    /**
     * @brief doWork checks integrity of all DataEntries in entries
     * @param masterPW in unprotected form
     * @param entries QVector containing QSharedPointer to the DataEntries
     *
     * Emits workFinished
     */
    void doWork(const QByteArray& masterPW, const QVector<QSharedPointer<DataEntry>>& entries);
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
    /**
     * @brief checkIntegrity takes the masterPW and makes a deep copy of it on purpose due to thread-safetieness
     * @param masterPW QSharedPointer to the QByteArray
     * @param entries
     */
    void checkIntegrity(const QSharedPointer<QByteArray>& masterPW, const QVector<QSharedPointer<DataEntry>>& entries);
private:
    QThread integrityCheckThread;
    QByteArray masterPW;
signals:
    void integrityCheckFinished(int returnCode);
    void operate(const QByteArray& masterPW, const QVector<QSharedPointer<DataEntry>>& entries);
};

#endif // INTEGRITYCHECK_H
