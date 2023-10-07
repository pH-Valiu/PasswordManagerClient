#ifndef PASSWORDBROKER_H
#define PASSWORDBROKER_H
#include "dataentry.h"




class PasswordBroker
{
public:
    PasswordBroker(PasswordBroker& other) = delete;
    void operator = (const PasswordBroker&) = delete;
    /**
     * @brief returns a reference to the only PasswordBroker instance at runtime
     *
     * Required Procedure:
     *
     * fetchFileData(), decryptData() (optional when fetching for the first time),
     *
     * encryptData(), storeFileData()
     * @return
     */
    static PasswordBroker& getInstance();
    bool fetchFileData();
    bool storeFileData();
    bool encryptData(const QByteArray& masterPW);
    bool decryptData(const QByteArray& masterPW);
    bool changerMasterPW(const QByteArray& oldMasterPW, const QByteArray& newMasterPW);
    void addEntry(QSharedPointer<DataEntry> dataEntry);
    /**
     * @brief removeEntryById
     * @param id
     * @return true if one or more entries were removed, false if none
     */
    bool removeEntryById(const QByteArray& id);
    /**
     * @brief removeEntryByName
     * @param name
     * @return true if one or more entries were removed, false if none
     */
    bool removeEntryByName(const QString& name);
    QSharedPointer<DataEntry> getEntryFromId(const QByteArray& id);
    QSharedPointer<DataEntry> getEntryFromName(const QString& name);
    QSharedPointer<DataEntry> getEntryFromWebsite(const QString& website);
    qsizetype entryCount() {return vector.size();}
    ~PasswordBroker();

#ifdef EXECUTE_UNIT_TESTS
    friend class PasswordBrokerTest;
#endif

private:
    PasswordBroker();
    QVector<QSharedPointer<DataEntry>> vector;
    struct FileData{
        QByteArray encryptedEntries;
        QByteArray iv;
        QByteArray mac;
    } fileData;


};

#endif // PASSWORDBROKER_H
