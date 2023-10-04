#ifndef PASSWORDBROKER_H
#define PASSWORDBROKER_H
#include "dataentry.h"




class PasswordBroker
{
public:
    static QSharedPointer<PasswordBroker> getInstance();
    QString fetchFileData();
    QString storeFileData();
    QString encryptData(const QByteArray& masterPW);
    QString decryptData(const QByteArray& masterPW);
    QString changerMasterPW(const QByteArray& oldMasterPW, const QByteArray& newMasterPW);
    void addEntry(QSharedPointer<DataEntry> dataEntry);
    bool removeEntry(const QByteArray& id);
    QSharedPointer<DataEntry> getEntryFromId(const QByteArray& id);
    QSharedPointer<DataEntry> getEntryFromName(const QString& name);
    QSharedPointer<DataEntry> getEntryFromWebsite(const QString& website);
    ~PasswordBroker();
private:
    PasswordBroker();
    QScopedPointer<QVector<QSharedPointer<DataEntry>>> vector;
    struct FileData{
        QByteArray encryptedEntries;
        QByteArray iv;
        QByteArray mac;
    } fileData;


};

#endif // PASSWORDBROKER_H
