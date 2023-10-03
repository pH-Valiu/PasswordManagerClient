#ifndef PASSWORDBROKER_H
#define PASSWORDBROKER_H
#include "dataentry.h"



class PasswordBroker
{
public:
    static QSharedPointer<PasswordBroker> getInstance();
    bool encryptData(const QByteArray& masterPW);
    bool decryptData(const QByteArray& masterPW);
    bool changerMasterPW(const QByteArray& oldMasterPW, const QByteArray& newMasterPW);
    void addEntry(QSharedPointer<DataEntry> dataEntry);
    bool removeEntry(const QByteArray& id);
    QSharedPointer<DataEntry> getEntryFromId(const QByteArray& id);
    QSharedPointer<DataEntry> getEntryFromName(const QString& name);
    QSharedPointer<DataEntry> getEntryFromWebsite(const QString& website);
    ~PasswordBroker();
private:
    PasswordBroker();
    QScopedPointer<QVector<QSharedPointer<DataEntry>>> vector;

};

#endif // PASSWORDBROKER_H
