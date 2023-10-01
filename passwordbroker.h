#ifndef PASSWORDBROKER_H
#define PASSWORDBROKER_H
#include "dataentry.h"



class PasswordBroker
{
public:
    static QSharedPointer<PasswordBroker> getInstance();
    //void addEntry(PasswordBroker::DataEntry);
    void newEntry();
    QSharedPointer<DataEntry> getEntryFromId(QByteArray id);
    QSharedPointer<DataEntry> getEntryFromName(QString name);
    QSharedPointer<DataEntry> getEntryFromWebsite(QString website);
    ~PasswordBroker();
private:
    PasswordBroker();
    QScopedPointer<QVector<QSharedPointer<DataEntry>>> vector;

};

#endif // PASSWORDBROKER_H
